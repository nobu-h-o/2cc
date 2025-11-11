#include <stdio.h>
#include <stdlib.h>
#include "parser.h"

extern void yy_scan_string(const char *str);

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "the number of arguments is incorrect\n");
        return 1;
    }

    printf(".global _main\n");
    printf(".align 2\n");
    printf("_main:\n");
    printf("  stp x29, x30, [sp, #-16]!\n");
    printf("  mov x29, sp\n");

    yy_scan_string(argv[1]);
    yyparse();

    printf("  ldp x29, x30, [sp], #16\n");
    printf("  ret\n");

    return 0;
}
