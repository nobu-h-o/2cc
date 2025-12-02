#include <stdio.h>
#include "codegen.h"
#include "ast.h"
#include "symtab.h"

SymbolTable *global_symtab = NULL;

static void codegen_number(int value) {
    printf("  mov x0, #%d\n", value);
    printf("  str x0, [sp, #-16]!\n");
}

static void codegen_add(void) {
    printf("  ldr x1, [sp], #16\n");
    printf("  ldr x0, [sp], #16\n");
    printf("  add x0, x0, x1\n");
    printf("  str x0, [sp, #-16]!\n");
}

static void codegen_sub(void) {
    printf("  ldr x1, [sp], #16\n");
    printf("  ldr x0, [sp], #16\n");
    printf("  sub x0, x0, x1\n");
    printf("  str x0, [sp, #-16]!\n");
}

static void codegen_mul(void) {
    printf("  ldr x1, [sp], #16\n");
    printf("  ldr x0, [sp], #16\n");
    printf("  mul x0, x0, x1\n");
    printf("  str x0, [sp, #-16]!\n");
}

static void codegen_div(void) {
    printf("  ldr x1, [sp], #16\n");
    printf("  ldr x0, [sp], #16\n");
    printf("  sdiv x0, x0, x1\n");
    printf("  str x0, [sp, #-16]!\n");
}

static void codegen_variable(const char *name) {
    int offset = symtab_get_offset(global_symtab, name);
    printf("  ldr x0, [x29, #%d]\n", -(16 + offset));
    printf("  str x0, [sp, #-16]!\n");
}

static void codegen_assignment(const char *name, ASTNode *value) {
    if (symtab_lookup(global_symtab, name) < 0) {
        symtab_add(global_symtab, name);
    }

    codegen_from_ast(value);

    int offset = symtab_get_offset(global_symtab, name);
    printf("  ldr x0, [sp], #16\n");
    printf("  str x0, [x29, #%d]\n", -(16 + offset));
}

void codegen_from_ast(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_NUMBER:
            codegen_number(node->data.number);
            break;
        case AST_VARIABLE:
            codegen_variable(node->data.variable);
            break;
        case AST_ASSIGNMENT:
            codegen_assignment(node->data.assignment.name, node->data.assignment.value);
            break;
        case AST_RETURN:
            codegen_from_ast(node->data.return_value);
            break;
        case AST_SEQUENCE:
            codegen_from_ast(node->data.sequence.first);
            codegen_from_ast(node->data.sequence.second);
            break;
        case AST_BINARY_OP:
            codegen_from_ast(node->data.binary.left);
            codegen_from_ast(node->data.binary.right);

            switch (node->data.binary.op) {
                case OP_ADD:
                    codegen_add();
                    break;
                case OP_SUB:
                    codegen_sub();
                    break;
                case OP_MUL:
                    codegen_mul();
                    break;
                case OP_DIV:
                    codegen_div();
                    break;
            }
            break;
    }
}

void codegen_finish(void) {
    printf("  ldr x0, [sp], #16\n");
}
