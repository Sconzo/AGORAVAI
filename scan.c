#include "globals.h"
#include "util.h"
#include "scan.h"

//Estados do DFA scanner
typedef enum
{
    START,
    INDIFF,
    COMMENT,
    COMPARETORS,
    INNUM,
    INID,
    DONE
} StateType;

//lexema do identificador ou palavra reservada
char tokenString[MAXTOKENLEN + 1];

// Tamanho do buffer de entrada para as linhas do codigo fonte
#define BUFLEN 256

//guarda a linha atual
static char lineBuf[BUFLEN];

//posicao atual em lineBuf
static int linepos = 0;

//tamanho atual da cadeia do buffer
static int bufsize = 0;

//getNextchar() captura o caractere seguinte (diferente de espaço) em lineBuf.
//Caso lineBuf ja tenha sido completamente lido, ele le uma nova linha
static char getNextChar(void)
{
    if (!(linepos < bufsize)) //Lendo uma nova linha
    {
        lineno++;
        if (fgets(lineBuf, BUFLEN - 1, source)) // atribui em lineBuf o texto de entrada
        {
            if (EchoSource)
                fprintf(listing, "%4d: %s", lineno, lineBuf);
            bufsize = strlen(lineBuf);
            linepos = 0;
            return lineBuf[linepos++]; //retorna o primeiro caractere da nova linha
        }
        else
            return EOF; //Acabou o arquivo
    }
    else
        return lineBuf[linepos++]; //lendo o proximo caractere
}

//ungetNextChar() retrocede um caractere em lineBuf
static void ungetNextChar(void)
{
    linepos--;
}

// tabela de verificacao de palavras reservadas
static struct
{
    char* str;
    TokenType tok;
} reservedWords[MAXRESERVED] =
            {{"if", IF},  {"else", ELSE}, {"int", INT},
            {"return", RETURN}, {"void", VOID}, {"while", WHILE}};



// reservedLookup() confere se um identificador eh uma palavra reservada
//utiliza busca linear
static TokenType reservedLookup(char*s)
{
    int i;
    for(i=0;i<MAXRESERVED;i++)
    {
        // Se for igual, retorna o valor do TOKEN da palavra reservada
        if(!strcmp(s,reservedWords[i].str)) return reservedWords[i].tok;
    }
    // Se for diferente, nao eh uma palavra reservada, retorna ID 
    return ID;
}

//getToken() retorna o proximo token do arquivo fonte
TokenType getToken(void)
{
    //indice para armazenar em tokenString
    int tokenStringIndex = 0;
    //preserva o token atual para retornar
    TokenType currentToken;
    //estado atual (sempre comeca em START)
    StateType state = START;
    // variavel para indicar a gravacao em tokenString
    int save;

    while (state != DONE)
    {
        char c = getNextChar();
        save = TRUE;
        switch (state)
        { 
        case START:
            if (isdigit(c)) state = INNUM;
            else if (isalpha(c)) state = INID;
            else if (c == '!') state = INDIFF;
            else if ((c==' ') || (c=='\t') || (c=='\n')) save = FALSE;
            else if (c=='/') 
            {
                if (getNextChar() == '*')
                {
                    state = COMMENT;
                    save  = FALSE;
                }
                else
                {
                    ungetNextChar();
                    currentToken = OVER;
                    save = TRUE;
                    state = DONE;
                }
            }
            else
            {
                state = DONE;
                switch (c)
                {
                case EOF:
                    save = FALSE;
                    currentToken = ENDFILE;
                    break;
                case '=':
                    if(getNextChar() == '=')
                        currentToken = EQ;
                    else{currentToken = ASSIGN;ungetNextChar();}
                    break;
                case '<':
                    if(getNextChar() == '=')
                        currentToken = LTE;
                    else{currentToken = LT;ungetNextChar();}
                    break;
                case '>':
                    if(getNextChar() == '=')
                        currentToken = GTE;
                    else{currentToken = GTE;ungetNextChar();}
                    break;
                case '+':
                    currentToken = PLUS;
                    break;
                case '-':
                    currentToken = MINUS;
                    break;
                case '*':
                    currentToken = TIMES;
                    break;
                case ';':
                    currentToken = SEMI;
                    break;
                case ',':
                    currentToken = COMMA;
                    break;
                case '(':
                    currentToken = LPAREN;
                    break;
                case ')':
                    currentToken = RPAREN;
                    break;
                case '[':
                    currentToken = LBRACKET;
                    break;
                case ']':
                    currentToken = RBRACKET;
                    break;
                case '{':
                    currentToken = LKEYS;
                    break;
                case '}':
                    currentToken = RKEYS;
                    break;

                default:
                    currentToken = ERROR;
                    break;
                }
            }
            break;
        case COMMENT:
            save = FALSE;
            if (c=='*')
            {
                if (getNextChar() == '/')
                {
                    state = START;
                }
            } 
            break;
        case INDIFF:
            state = DONE;
            if(c=='=') currentToken = NE;
            else //ocorreu um erro e volta para a entrada
            {
                ungetNextChar();
                save = FALSE;
                currentToken = ERROR;
            }
            break;
        case INNUM:
            if(!isdigit(c)) // volta para a entrada
            { 
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = NUM;
            }
            break;
        case INID:
            if(!isalpha(c)) //volta para a entrada
            {
                ungetNextChar();
                save = FALSE;
                state = DONE;
                currentToken = ID;
            }
            break;
        case DONE:
            default: //nunca deveria acontecer
                fprintf(listing,"Scanner Bug: state %d\n",state);
                state = DONE;
                currentToken = ERROR;
                break;
        }
        if ((save) && (tokenStringIndex <= MAXTOKENLEN)) 
            tokenString[tokenStringIndex++] = c;
        
        if (state == DONE)
        {
            tokenString[tokenStringIndex] = '\0';
            if(currentToken == ID) currentToken = reservedLookup(tokenString);
            
        }
     }
 
     if(TraceScan)
     {
         fprintf(listing,"\t%d: ", lineno);
         printToken(currentToken,tokenString);
     }
     return currentToken;
}