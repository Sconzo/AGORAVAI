# include "globals.h"

/* set NO_PARSE to TRUE to get a scanner-only compiler */
# define NO_PARSE FALSE
/* set NO_ANALYZE to TRUE to get a parser-only compiler */
# define NO_ANALYZE FALSE

/* set NO_CODE to TRUE to get a compiler that does not generate code */
# define NO_CODE FALSE

# include "util.h"
# if NO_PARSE
# include "scan.h"
# else
# include "parse.h"
# if !NO_ANALYZE
# include "analyze.h"
# if !NO_CODE
#include "cgen.h"
# endif
# endif
# endif

int lineno = 0;
FILE *source;
FILE *listing;
FILE *code;

int EchoSource = FALSE;
int TraceScan = TRUE;
int TraceParse = TRUE;
int TraceAnalyze = TRUE;
int TraceCode = TRUE;

int Error = FALSE;

int main(int argc, char *argv[]) {
    TreeNode *syntaxTree;
    char pgm[120];
    if (argc != 2) {
        fprintf(stderr, "usage: %s <filename>\n", argv[0]);
        exit(1);
    }
    strcpy(pgm, argv[1]);
    if (strchr(pgm, '.') == NULL)
        strcat(pgm, ".cm");
    source = fopen(pgm, "r");
    if (source == NULL) {
        fprintf(stderr, "Arquivo %s não encontrado\n", pgm);
        exit(1);
    }
    listing = stdout;
    fprintf(listing, "\nC- Compilação: %s\n", pgm);
# if NO_PARSE
    while (getToken() != ENDFILE);
# else
    syntaxTree = parse();
    if (TraceParse) {
        fprintf(listing, "\nÁrvore sintática:\n");
        printTree(syntaxTree);
    }
# if !NO_ANALYZE
    if (!Error) {
        if (TraceAnalyze)
            fprintf(listing, "\nConstruindo a Tabela de Simbolos...\n");
        buildSymtab(syntaxTree);
        if (TraceAnalyze)
            fprintf(listing, "\nChecando tipos...\n");
        typeCheck(syntaxTree);
        if (TraceAnalyze)
            fprintf(listing, "\nChecagem de tipos finalizada\n");
    }
# if !NO_CODE
    if (!Error) {
        char *codefile;
        int fnlen = strcspn(pgm, ".");
        codefile = (char *)calloc(fnlen + 4, sizeof(char));
        strncpy(codefile, pgm, fnlen);
        strcat(codefile, ".tm");
        code = fopen(codefile, "w");
        if (code == NULL)
        {
            printf("Unable to open %s\n", codefile);
            exit(1);
        }
        fprintf(listing, "\n**CODIGO INTERMEDIARIO**\n");
        codeGen(syntaxTree, codefile);
        fclose(code);
    }
# endif
# endif
# endif
    fclose(source);
    return 0;
}
