#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "codegen.h"
#include "ast.h"
#include "symtab.h"

extern void yy_scan_string(const char *str);
extern ASTNode *root;
extern SymbolTable *global_symtab;

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "the number of arguments is incorrect\n");
        return 1;
    }

    global_symtab = symtab_create();

    printf(".global _main\n");
    printf(".align 2\n");
    printf("_main:\n");

    yy_scan_string(argv[1]);
    yyparse();

    if (root) {
        /* Emit prologue */
        printf("  stp x29, x30, [sp, #-16]!\n");
        printf("  mov x29, sp\n");

        /* Allocate generous space for variables and work stack */
        printf("  sub sp, sp, #256\n");

        codegen_from_ast(root);
        codegen_finish();

        printf("  add sp, sp, #256\n");

        ast_free(root);
    }

    printf("  ldp x29, x30, [sp], #16\n");
    printf("  ret\n");

    symtab_free(global_symtab);

    return 0;
}
