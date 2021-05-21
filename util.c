# include "globals.h"
# include "util.h"

void printToken(TokenType token, const char *tokenString) {
    switch (token) {
        case IF:
            fprintf(listing, "Palavra reservada: %s\n", tokenString);
            break;
        case ELSE:
            fprintf(listing, "Palavra reservada: %s\n", tokenString);
            break;
        case INT:
            fprintf(listing, "Palavra reservada: %s\n", tokenString);
            break;
        case RETURN:
            fprintf(listing, "Palavra reservada: %s\n", tokenString);
            break;
        case VOID:
            fprintf(listing, "Palavra reservada: %s\n", tokenString);
            break;
        case WHILE:
            fprintf(listing, "Palavra reservada: %s\n", tokenString);
            break;
        case ASSIGN:
            fprintf(listing, "Simbolo: =\n");
            break;
        case LT:
            fprintf(listing, "<\n");
            break;
        case EQ:
            fprintf(listing, "==\n");
            break;
        case GT:
            fprintf(listing, ">\n");
            break;
        case LTE:
            fprintf(listing, "<=\n");
            break;
        case GTE:
            fprintf(listing, ">=\n");
            break;
        case NE:
            fprintf(listing, "!=\n");
            break;
        case LBRACKET:
            fprintf(listing, "[\n");
            break;
        case RBRACKET:
            fprintf(listing, "]\n");
            break;
        case LKEYS:
            fprintf(listing, "{\n");
            break;
        case RKEYS:
            fprintf(listing, "}\n");
            break;
        case LPAREN:
            fprintf(listing, "(\n");
            break;
        case RPAREN:
            fprintf(listing, ")\n");
            break;
        case SEMI:
            fprintf(listing, ";\n");
            break;
        case COMMA:
            fprintf(listing, ",\n");
            break;
        case PLUS:
            fprintf(listing, "+\n");
            break;
        case MINUS:
            fprintf(listing, "-\n");
            break;
        case TIMES:
            fprintf(listing, "*\n");
            break;
        case OVER:
            fprintf(listing, "/\n");
            break;
        case ENDFILE:
            fprintf(listing, "EOF\n");
            break;
        case NUM:
            fprintf(listing, "Numero: %s\n", tokenString);
            break;
        case ID:
            fprintf(listing, "ID: %s\n", tokenString);
            break;
        case ERROR:
            fprintf(listing, "ERROR: %s\n", tokenString);
            break;
        default:
            fprintf(listing, "Token não reconhecido: %d\n", token);
    }
}

void aggScope(TreeNode *t, char *scope) {
    int i;
    while (t != NULL) {
        for (i = 0; i < MAXCHILDREN; ++i) {
            t->attr.scope = scope;
            aggScope(t->child[i], scope);
        }
        t = t->sibling;
    }
}

TreeNode *newStmtNode(StatementKind kind) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL)
        fprintf(listing, "Fora da memória, erro na linha %d\n", lineno);
    else {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = statementK;
        t->kind.stmt = kind;
        t->lineno = lineno;
        t->attr.scope = "global";
    }
    return t;
}

TreeNode *newExpNode(ExpressionIdentifier kind) {
    TreeNode *t = (TreeNode *)malloc(sizeof(TreeNode));
    int i;
    if (t == NULL)
        fprintf(listing, "Fora da memória, erro na linha %d\n", lineno);
    else {
        for (i = 0; i < MAXCHILDREN; i++)
            t->child[i] = NULL;
        t->sibling = NULL;
        t->nodekind = expressionK;
        t->kind.exp = kind;
        t->lineno = lineno;
        t->type = VOID;
        t->attr.scope = "global";
    }
    return t;
}

char *copyString(char *s) {
    int n;
    char *t;
    if (s == NULL)
        return NULL;
    n = strlen(s) + 1;
    t = malloc(n);
    if (t == NULL)
        fprintf(listing, "Fora da memória, erro na linha %d\n", lineno);
    else
        strcpy(t, s);
    return t;
}

static int indentno = -4;
int aux = 0;

# define INDENT indentno += 4
# define UNINDENT indentno -= 4

void teste(int aux){
    if (aux==1){
        fprintf(listing, "    ");
        //UNINDENT;
    }
}

static void printSpaces(void) {
    int i;
    //fprintf(listing, "%d",indentno);
    for (i = 0; i < indentno; i++)
        fprintf(listing, " ");
}

void printTree(TreeNode *tree) {
    int i;
    INDENT;
    extern int aux;
    //
    
    while (tree != NULL) {
        printSpaces();
        if (tree->nodekind == statementK) {
            switch (tree->kind.stmt) {
                case ifK:
                    fprintf(listing, "If\n");
                    break;
                case assignK:
                    fprintf(listing, "Atribuição\n");
                    break;
                case whileK:
                    fprintf(listing, "While\n");
                    break;
                case variableK:
                    fprintf(listing, "Variável %s\n", tree->attr.name);
                    break;
                case arrayK:
                    fprintf(listing, "Array %s\n", tree->attr.name);
                    break;
                case paramK:
                    fprintf(listing, "Parâmetro %s\n", tree->attr.name);
                    break;
                case functionK:
                    fprintf(listing, "Função %s\n", tree->attr.name);
                    break;
                case callK:
                    fprintf(listing, "Chamada da função %s \n", tree->attr.name);
                    break;
                case returnK:
                    fprintf(listing, "Return\n");
                    break;

                default:
                    fprintf(listing, "Tipo não conhecido ExpNode\n");
                    break;
            }
        }
        else if (tree->nodekind == expressionK) {
            switch (tree->kind.exp) {
                case operationK:
                    fprintf(listing, "Operação: ");
                    printToken(tree->attr.op, "\0");
                    break;
                case constantK:
                    fprintf(listing, "Constante: %d\n", tree->attr.val);
                    break;
                case idK:
                    fprintf(listing, "Id: %s\n", tree->attr.name);
                    break;
                case vectorK:
                    fprintf(listing, "Vetor: %s\n", tree->attr.name);
                    break;
                case vectorIdK:
                    fprintf(listing, "Index [%d]\n", tree->attr.val);
                    break;
                case typeK:
                    fprintf(listing, "Tipo %s\n", tree->attr.name);
                    break;

                default:
                    fprintf(listing, "Tipo não conhecido ExpNode \n");
                    break;
                }
        }
        else
            fprintf(listing, "Tipo de nó não conhecido\n");
        teste(aux);
        aux = 1;
        for (i = 0; i < MAXCHILDREN; i++)
            
            printTree(tree->child[i]);
           
        tree = tree->sibling;
        
    }
    
    UNINDENT;
}
