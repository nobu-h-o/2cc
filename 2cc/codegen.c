#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "codegen.h"
#include "ast.h"
#include "symtab.h"

SymbolTable *global_symtab = NULL;
int stack_depth = 0;
static int label_counter = 0;
static SymbolTable *current_symtab = NULL;
static int current_local_vars = 0;
static int current_function_end_label = -1;
static GlobalVar *current_globals = NULL;

static int is_global_var(const char *name) {
    GlobalVar *current = current_globals;
    while (current) {
        if (strcmp(current->name, name) == 0) {
            return 1;
        }
        current = current->next;
    }
    return 0;
}

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

static void codegen_cmp(const char *condition) {
    stack_depth -= 8;
    printf("  ldr w1, [sp, %d]\n", stack_depth + 4);
    printf("  ldr w0, [sp, %d]\n", stack_depth);
    printf("  cmp w0, w1\n");
    printf("  cset w0, %s\n", condition);
    printf("  str w0, [sp, %d]\n", stack_depth);
    stack_depth += 4;
}

static void codegen_variable(const char *name) {
    // Check if it's a global variable
    if (is_global_var(name)) {
        // Load global variable
        printf("  adrp x0, _%s@PAGE\n", name);
        printf("  add x0, x0, _%s@PAGEOFF\n", name);
        printf("  ldr w0, [x0]\n");
        printf("  str w0, [sp, %d]\n", stack_depth);
        stack_depth += 4;
        return;
    }

    // Local variable
    SymbolTable *symtab = current_symtab ? current_symtab : global_symtab;

    // If variable not in local table, add it
    if (current_symtab && symtab_lookup(current_symtab, name) < 0) {
        symtab_add(current_symtab, name);
    }

    int offset = symtab_get_offset(symtab, name);
    printf("  ldr w0, [x29, #%d]\n", -(4 + offset));
    printf("  str w0, [sp, %d]\n", stack_depth);
    stack_depth += 4;
}

static void codegen_assignment(const char *name, ASTNode *value) {
    codegen_from_ast(value);

    // Check if it's a global variable
    if (is_global_var(name)) {
        // Store to global variable
        stack_depth -= 4;
        printf("  ldr w0, [sp, %d]\n", stack_depth);
        printf("  adrp x1, _%s@PAGE\n", name);
        printf("  add x1, x1, _%s@PAGEOFF\n", name);
        printf("  str w0, [x1]\n");
        return;
    }

    // Local variable
    SymbolTable *symtab = current_symtab ? current_symtab : global_symtab;

    // If variable not in local table, add it
    if (current_symtab && symtab_lookup(current_symtab, name) < 0) {
        symtab_add(current_symtab, name);
    }

    int offset = symtab_get_offset(symtab, name);
    stack_depth -= 4;
    printf("  ldr w0, [sp, %d]\n", stack_depth);
    printf("  str w0, [x29, #%d]\n", -(4 + offset));
}

static void codegen_while(ASTNode *condition, ASTNode *body) {
    int loop_label = label_counter++;
    int end_label = label_counter++;

    printf("L%d:\n", loop_label);

    codegen_from_ast(condition);
    stack_depth -= 4;
    printf("  ldr w0, [sp, %d]\n", stack_depth);
    printf("  cbz w0, L%d\n", end_label);

    codegen_from_ast(body);

    printf("  b L%d\n", loop_label);
    printf("L%d:\n", end_label);
}

static void codegen_for(ASTNode *init, ASTNode *condition, ASTNode *increment, ASTNode *body) {
    int loop_label = label_counter++;
    int end_label = label_counter++;

    codegen_from_ast(init);

    printf("L%d:\n", loop_label);

    codegen_from_ast(condition);
    stack_depth -= 4;
    printf("  ldr w0, [sp, %d]\n", stack_depth);
    printf("  cbz w0, L%d\n", end_label);

    codegen_from_ast(body);
    codegen_from_ast(increment);

    printf("  b L%d\n", loop_label);
    printf("L%d:\n", end_label);
}

static void codegen_print(ASTNode *value) {
    codegen_from_ast(value);
    stack_depth -= 4;
    printf("  ldr w0, [sp, %d]\n", stack_depth);
    printf("  bl put_int\n");
}

static void codegen_function_def(char *name, ParamList *params, ASTNode *body) {
    // Create a local symbol table for this function
    SymbolTable *prev_symtab = current_symtab;
    current_symtab = symtab_create();

    // Create an end label for this function
    int end_label = label_counter++;
    int prev_end_label = current_function_end_label;
    current_function_end_label = end_label;

    // Count and register parameters
    int param_count = param_list_count(params);

    // Add parameters to local symbol table
    ParamList *temp = params;
    while (temp) {
        symtab_add_parameter(current_symtab, temp->name);
        temp = temp->next;
    }

    // Generate function label
    printf(".global _%s\n", name);
    printf(".align 2\n");
    printf("_%s:\n", name);

    // Function prologue
    printf("  stp x29, x30, [sp, #-16]!\n");
    printf("  mov x29, sp\n");
    printf("  sub sp, sp, #256\n");

    // Save parameters from registers to stack
    // First 8 parameters come in w0-w7
    int param_idx = 0;
    temp = params;
    while (temp && param_idx < 8) {
        int offset = symtab_get_offset(current_symtab, temp->name);
        printf("  str w%d, [x29, #%d]\n", param_idx, -(4 + offset));
        param_idx++;
        temp = temp->next;
    }

    // Parameters beyond 8 are on the stack (passed by caller)
    // We need to copy them from the caller's frame
    param_idx = 8;
    while (temp) {
        int offset = symtab_get_offset(current_symtab, temp->name);
        int caller_offset = 16 + (param_idx - 8) * 4;
        printf("  ldr w0, [x29, #%d]\n", caller_offset);
        printf("  str w0, [x29, #%d]\n", -(4 + offset));
        param_idx++;
        temp = temp->next;
    }

    // Generate function body
    stack_depth = 0;
    codegen_from_ast(body);

    // Function epilogue
    printf("L%d:\n", end_label);
    printf("  add sp, sp, #256\n");
    printf("  ldp x29, x30, [sp], #16\n");
    printf("  ret\n");

    // Restore previous symbol table and end label
    symtab_free(current_symtab);
    current_symtab = prev_symtab;
    current_function_end_label = prev_end_label;
}

static void codegen_function_call(char *name, ArgList *args) {
    // Count arguments
    int arg_count = arg_list_count(args);

    // Calculate space needed for stack arguments (beyond 8)
    int stack_args_size = 0;
    if (arg_count > 8) {
        stack_args_size = (arg_count - 8) * 4;
    }

    // Evaluate all arguments first and save them temporarily
    int saved_stack_depth = stack_depth;
    ArgList *temp = args;
    while (temp) {
        codegen_from_ast(temp->expr);
        temp = temp->next;
    }

    // Now move arguments to proper locations
    // Arguments are on our stack at [sp, saved_stack_depth] to [sp, stack_depth-4]

    // Move first 8 arguments to w0-w7
    for (int i = 0; i < arg_count && i < 8; i++) {
        printf("  ldr w%d, [sp, %d]\n", i, saved_stack_depth + i * 4);
    }

    // Move arguments beyond 8 to the stack for callee
    // These need to be pushed in reverse order
    if (arg_count > 8) {
        // Allocate space on stack
        printf("  sub sp, sp, #%d\n", stack_args_size);
        for (int i = 8; i < arg_count; i++) {
            printf("  ldr w9, [sp, #%d]\n", saved_stack_depth + i * 4 + stack_args_size);
            printf("  str w9, [sp, #%d]\n", (i - 8) * 4);
        }
    }

    // Call the function
    printf("  bl _%s\n", name);

    // Clean up stack arguments if any
    if (arg_count > 8) {
        printf("  add sp, sp, #%d\n", stack_args_size);
    }

    // Reset stack depth to before arguments
    stack_depth = saved_stack_depth;

    // Return value is in w0, push it on our stack
    printf("  str w0, [sp, %d]\n", stack_depth);
    stack_depth += 4;
}

static void codegen_global_var(char *name, ASTNode *value) {
    // Global variables are handled in a separate pass, nothing to do here
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
            // Pop the return value from stack into w0
            stack_depth -= 4;
            printf("  ldr w0, [sp, %d]\n", stack_depth);
            // Jump to function epilogue
            printf("  b L%d\n", current_function_end_label);
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
                case OP_LT:
                    codegen_cmp("lt");
                    break;
                case OP_GT:
                    codegen_cmp("gt");
                    break;
                case OP_LE:
                    codegen_cmp("le");
                    break;
                case OP_GE:
                    codegen_cmp("ge");
                    break;
                case OP_EQ:
                    codegen_cmp("eq");
                    break;
                case OP_NE:
                    codegen_cmp("ne");
                    break;
            }
            break;
        case AST_WHILE:
            codegen_while(node->data.while_loop.condition, node->data.while_loop.body);
            break;
        case AST_FOR:
            codegen_for(node->data.for_loop.init, node->data.for_loop.condition,
                       node->data.for_loop.increment, node->data.for_loop.body);
            break;
        case AST_PRINT:
            codegen_print(node->data.print_value);
            break;
        case AST_FUNCTION_DEF:
            codegen_function_def(node->data.function_def.name,
                               node->data.function_def.params,
                               node->data.function_def.body);
            break;
        case AST_FUNCTION_CALL:
            codegen_function_call(node->data.function_call.name,
                                node->data.function_call.args);
            break;
        case AST_GLOBAL_VAR:
            codegen_global_var(node->data.global_var.name,
                             node->data.global_var.value);
            break;
    }
}

void codegen_finish(void) {
    if (stack_depth >= 4) {
        printf("  ldr w0, [sp, %d]\n", stack_depth - 4);
    }
}

void codegen_program(ASTNode *root, GlobalVar *globals) {
    // Store globals for use during code generation
    current_globals = globals;

    // Generate .data section for global variables
    if (globals) {
        printf(".data\n");
        GlobalVar *current = globals;
        while (current) {
            printf(".global _%s\n", current->name);
            printf(".p2align 2\n");
            printf("_%s:\n", current->name);
            printf("  .word %d\n", current->value);
            current = current->next;
        }
    }

    // Generate the put_int helper function
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

    // Second pass: generate all functions
    codegen_from_ast(root);
}
