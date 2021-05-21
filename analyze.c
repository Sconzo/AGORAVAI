# include "globals.h"
# include "symtab.h"
# include "analyze.h"

static void typeError(TreeNode *t, char *message) {
    fprintf(listing, "Erro semântico em %s, linha %d: %s\n", t->attr.name, t->lineno, message);
    Error = TRUE;
}

static int location = 0;

static void traverse(TreeNode *t, void (*preProc)(TreeNode *), void (*postProc)(TreeNode *)) {
    if (t != NULL) {
        preProc(t);
        int i;
        for (i = 0; i < MAXCHILDREN; i++)
            traverse(t->child[i], preProc, postProc);
        postProc(t);
        traverse(t->sibling, preProc, postProc);
    }
}

static void nullProc(TreeNode *t) {
    if (t == NULL)
        return;
    else
        return;
}

static void insertNode(TreeNode *t) {
    switch (t->nodekind) {
        case statementK:
            switch (t->kind.stmt) {
                case variableK:
                    if (t->type == voidK)
                            typeError(t, "Error 3: Declaração inválida. Tipo de variável deve ser inteiro.");
                    else {
                        if (st_lookup(t->attr.name, t->attr.scope) == -1) {
                            if (st_lookup(t->attr.name, "global") == -1 || strcmp(st_lookup_id(t->attr.name, "global"), "function") != 0)
                                st_insert(t->attr.name, t->lineno, location++, t->attr.scope, "variable", "integer");
                            else 
                                typeError(t, "Error 7: Declaração inválida. Nome da variável já declarada como uma função.");
                        }
                        else 
                            typeError(t, "Error 4: Declaração inválida. Variável já declarada.");
                    }
                    break;
                case arrayK:
                    if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1)
                        st_insert(t->attr.name, t->lineno, location++, t->attr.scope, "vectordecl", "integer");
                    else
                        typeError(t, "Error 4: Declaração inválida. Vetor já declarado.");
                    break;
                case paramK:
                    if (st_lookup(t->attr.name, t->attr.scope) == -1)
                        st_insert(t->attr.name, t->lineno, location++, t->attr.scope, "param", "integer");
                    else
                        typeError(t, "Error 4: Declaração inválida. Parâmetro já declarado.");
                    break;
                case functionK:
                    if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1) {
                        if (t->type == integerK)
                            st_insert(t->attr.name, t->lineno, location++, t->attr.scope, "function", "integer");
                        else
                            st_insert(t->attr.name, t->lineno, location++, t->attr.scope, "function", "void");
                    }
                    else
                        typeError(t, "Error 4: Declaração inválida. Função já declarada.");
                    break;
                case callK:
                    if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1)
                        typeError(t, "Error 5: Chamada inválida. Não foi declarada.");
                    else
                        st_insert(t->attr.name, t->lineno, location++, t->attr.scope, "call", st_lookup_type(t->attr.name, "global"));
                case returnK:
                    break;
                default:
                    break;
            }
            break;
        case expressionK:
            switch (t->kind.exp) {
                case idK:
                    if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1)
                        typeError(t, "Error 1: Variável não foi declarada");
                    else
                        st_insert(t->attr.name, t->lineno, 0, t->attr.scope, "variable", "integer");
                    break;
                case vectorK:
                    if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1)
                        typeError(t, "Error 1: Vetor não foi declarado");
                    else
                        st_insert(t->attr.name, t->lineno, 0, t->attr.scope, "vector", "integer");
                    break;
                case vectorIdK:
                    if (st_lookup(t->attr.name, t->attr.scope) == -1 && st_lookup(t->attr.name, "global") == -1)
                        typeError(t, "Error 1: Índice do vetor não foi declarado");
                    else
                        st_insert(t->attr.name, t->lineno, 0, t->attr.scope, "vector index", "integer");
                case typeK:
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void buildSymtab(TreeNode *syntaxTree) {
    st_insert("input", 0, location++, "global", "function", "integer");
    st_insert("output", 0, location++, "global", "function", "integer");
    traverse(syntaxTree, insertNode, nullProc);
    if (st_lookup("main", "global") == -1) {
        printf("Erro semântico 6: main não foi declarado");
        Error = TRUE;
    }
    if (TraceAnalyze) {
        fprintf(listing, "\nTabela de simbolos:\n\n");
        printSymTab(listing);
    }
}


static void checkNode(TreeNode *t) {
    switch (t->nodekind) {
        case expressionK:
            switch (t->kind.exp) {
                case operationK:
                    break;
                default:
                    break;
            }
            break;
        case statementK:
            switch (t->kind.stmt) {
                case ifK:
                    if (t->child[0]->type == integerK && t->child[1]->type == integerK)
                        typeError(t->child[0], "Teste condicional não booleano");
                    break;
                case assignK:
                    if (t->child[0]->type == voidK || t->child[1]->type == voidK)
                        typeError(t->child[0], "atribuição de uma valor não inteiro");
                    else if (t->child[1]->kind.stmt == callK) {
                        if (strcmp(t->child[1]->attr.name, "input") != 0 && strcmp(t->child[1]->attr.name, "output") != 0) {
                            if (strcmp(st_lookup_type(t->child[1]->attr.name, t->child[1]->attr.scope), "void") == 0)
                                typeError(t->child[1], "Error 2: atribuição de um retorno void");
                        }
                    }
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }
}

void typeCheck(TreeNode *syntaxTree) {
    traverse(syntaxTree, nullProc, checkNode);
}
