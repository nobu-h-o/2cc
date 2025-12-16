#include <stdio.h>
#include <stdlib.h>
#include "parser.h"
#include "codegen.h"
#include "ast.h"
#include "symtab.h"

extern void yy_scan_string(const char *str);
extern ASTNode *root;
extern SymbolTable *global_symtab;
extern int stack_depth;

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "the number of arguments is incorrect\n");
        return 1;
    }

    global_symtab = symtab_create();

    printf(".text\n");
    printf(".p2align 2\n");
    printf(".LC0:\n");
    printf("  .string \"%%d\\n\"\n");
    printf(".text\n");
    printf(".p2align 2\n");
    printf("put_int:\n");
    printf("  sub sp, sp, #32\n");
    printf("  stp x29, x30, [sp, #16]\n");
    printf("  add x29, sp, #16\n");
    printf("  stur w0, [x29, #-4]\n");
    printf("  ldur w9, [x29, #-4]\n");
    printf("  mov x8, x9\n");
    printf("  adrp x0, .LC0@PAGE\n");
    printf("  add x0, x0, .LC0@PAGEOFF\n");
    printf("  mov x9, sp\n");
    printf("  str x8, [x9]\n");
    printf("  bl _printf\n");
    printf("  ldp x29, x30, [sp, #16]\n");
    printf("  add sp, sp, #32\n");
    printf("  ret\n");

    printf(".global _main\n");
    printf(".align 2\n");
    printf("_main:\n");

    yy_scan_string(argv[1]);
    yyparse();

    if (root) {
        printf("  stp x29, x30, [sp, #-16]!\n");
        printf("  mov x29, sp\n");

        printf("  sub sp, sp, #256\n");

        stack_depth = 0;
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
