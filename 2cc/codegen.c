#include <stdio.h>
#include "codegen.h"

void codegen_number(int value) {
    printf("  mov x0, #%d\n", value);
    printf("  str x0, [sp, #-16]!\n");
}

void codegen_add(void) {
    printf("  ldr x1, [sp], #16\n");
    printf("  ldr x0, [sp], #16\n");
    printf("  add x0, x0, x1\n");
    printf("  str x0, [sp, #-16]!\n");
}

void codegen_sub(void) {
    printf("  ldr x1, [sp], #16\n");
    printf("  ldr x0, [sp], #16\n");
    printf("  sub x0, x0, x1\n");
    printf("  str x0, [sp, #-16]!\n");
}

void codegen_mul(void) {
    printf("  ldr x1, [sp], #16\n");
    printf("  ldr x0, [sp], #16\n");
    printf("  mul x0, x0, x1\n");
    printf("  str x0, [sp, #-16]!\n");
}

void codegen_div(void) {
    printf("  ldr x1, [sp], #16\n");
    printf("  ldr x0, [sp], #16\n");
    printf("  sdiv x0, x0, x1\n");
    printf("  str x0, [sp, #-16]!\n");
}

void codegen_finish(void) {
    printf("  ldr x0, [sp], #16\n");
}
