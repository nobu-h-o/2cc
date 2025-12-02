#include <stdio.h>
#include "codegen.h"
#include "ast.h"
#include "symtab.h"

SymbolTable *global_symtab = NULL;
int stack_depth = 0;

static void codegen_number(int value) {
    printf("  mov w0, #%d\n", value);
    printf("  str w0, [sp, %d]\n", stack_depth);
    stack_depth += 4;
}

static void codegen_add(void) {
    stack_depth -= 8;
    printf("  ldr w1, [sp, %d]\n", stack_depth + 4);
    printf("  ldr w0, [sp, %d]\n", stack_depth);
    printf("  add w0, w0, w1\n");
    printf("  str w0, [sp, %d]\n", stack_depth);
    stack_depth += 4;
}

static void codegen_sub(void) {
    stack_depth -= 8;
    printf("  ldr w1, [sp, %d]\n", stack_depth + 4);
    printf("  ldr w0, [sp, %d]\n", stack_depth);
    printf("  sub w0, w0, w1\n");
    printf("  str w0, [sp, %d]\n", stack_depth);
    stack_depth += 4;
}

static void codegen_mul(void) {
    stack_depth -= 8;
    printf("  ldr w1, [sp, %d]\n", stack_depth + 4);
    printf("  ldr w0, [sp, %d]\n", stack_depth);
    printf("  mul w0, w0, w1\n");
    printf("  str w0, [sp, %d]\n", stack_depth);
    stack_depth += 4;
}

static void codegen_div(void) {
    stack_depth -= 8;
    printf("  ldr w1, [sp, %d]\n", stack_depth + 4);
    printf("  ldr w0, [sp, %d]\n", stack_depth);
    printf("  sdiv w0, w0, w1\n");
    printf("  str w0, [sp, %d]\n", stack_depth);
    stack_depth += 4;
}

static void codegen_variable(const char *name) {
    int offset = symtab_get_offset(global_symtab, name);
    printf("  ldr w0, [x29, #%d]\n", -(4 + offset));
    printf("  str w0, [sp, %d]\n", stack_depth);
    stack_depth += 4;
}

static void codegen_assignment(const char *name, ASTNode *value) {
    codegen_from_ast(value);

    int offset = symtab_get_offset(global_symtab, name);
    stack_depth -= 4;  /* Pop the result */
    printf("  ldr w0, [sp, %d]\n", stack_depth);
    printf("  str w0, [x29, #%d]\n", -(4 + offset));
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
    if (stack_depth >= 4) {
        printf("  ldr w0, [sp, %d]\n", stack_depth - 4);
    }
}
