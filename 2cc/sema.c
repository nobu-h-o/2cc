#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sema.h"
#include "ast.h"

// Function info for the first pass
typedef struct FuncInfo {
    char *name;
    int param_count;
    struct FuncInfo *next;
} FuncInfo;

// Variable scope for the second pass
typedef struct VarScope {
    char *name;
    struct VarScope *next;
} VarScope;

// Global state
static FuncInfo *functions = NULL;
static VarScope *global_vars = NULL;
static int error_count = 0;

static void add_function(const char *name, int param_count) {
    FuncInfo *info = (FuncInfo *)malloc(sizeof(FuncInfo));
    info->name = strdup(name);
    info->param_count = param_count;
    info->next = functions;
    functions = info;
}

static FuncInfo *find_function(const char *name) {
    FuncInfo *curr = functions;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            return curr;
        }
        curr = curr->next;
    }
    return NULL;
}

static void add_global_var(const char *name) {
    VarScope *var = (VarScope *)malloc(sizeof(VarScope));
    var->name = strdup(name);
    var->next = global_vars;
    global_vars = var;
}

static int is_global_var(const char *name) {
    VarScope *curr = global_vars;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

static void free_functions(void) {
    while (functions) {
        FuncInfo *next = functions->next;
        free(functions->name);
        free(functions);
        functions = next;
    }
}

static void free_global_vars(void) {
    while (global_vars) {
        VarScope *next = global_vars->next;
        free(global_vars->name);
        free(global_vars);
        global_vars = next;
    }
}

static void collect_definitions(ASTNode *node) {
    if (!node) return;

    if (node->type == AST_FUNCTION_DEF) {
        if (find_function(node->data.function_def.name)) {
            fprintf(stderr, "error: function '%s' already defined\n",
                    node->data.function_def.name);
            error_count++;
        } else {
            int param_count = param_list_count(node->data.function_def.params);
            add_function(node->data.function_def.name, param_count);
        }
    } else if (node->type == AST_GLOBAL_VAR) {
        if (is_global_var(node->data.global_var.name)) {
            fprintf(stderr, "error: variable '%s' already declared\n",
                    node->data.global_var.name);
            error_count++;
        } else {
            add_global_var(node->data.global_var.name);
        }
    } else if (node->type == AST_SEQUENCE) {
        collect_definitions(node->data.sequence.first);
        collect_definitions(node->data.sequence.second);
    }
}

// Local variable scope for second pass
static VarScope *local_vars = NULL;

static void push_local_var(const char *name) {
    VarScope *var = (VarScope *)malloc(sizeof(VarScope));
    var->name = strdup(name);
    var->next = local_vars;
    local_vars = var;
}

static int is_local_var(const char *name) {
    VarScope *curr = local_vars;
    while (curr) {
        if (strcmp(curr->name, name) == 0) {
            return 1;
        }
        curr = curr->next;
    }
    return 0;
}

static void free_local_vars(void) {
    while (local_vars) {
        VarScope *next = local_vars->next;
        free(local_vars->name);
        free(local_vars);
        local_vars = next;
    }
}

static void check_expr(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_VARIABLE:
            if (!is_local_var(node->data.variable) &&
                !is_global_var(node->data.variable)) {
                fprintf(stderr, "error: undefined variable '%s'\n",
                        node->data.variable);
                error_count++;
            }
            break;

        case AST_FUNCTION_CALL: {
            FuncInfo *func = find_function(node->data.function_call.name);
            if (!func) {
                fprintf(stderr, "error: undefined function '%s'\n",
                        node->data.function_call.name);
                error_count++;
            } else {
                int arg_count = arg_list_count(node->data.function_call.args);
                if (arg_count != func->param_count) {
                    fprintf(stderr, "error: function '%s' expects %d arguments, got %d\n",
                            node->data.function_call.name,
                            func->param_count, arg_count);
                    error_count++;
                }
            }
            // Check arguments
            ArgList *args = node->data.function_call.args;
            while (args) {
                check_expr(args->expr);
                args = args->next;
            }
            break;
        }

        case AST_BINARY_OP:
            check_expr(node->data.binary.left);
            check_expr(node->data.binary.right);
            break;

        default:
            break;
    }
}

// Check statement and collect local variable declarations
static void check_statement(ASTNode *node) {
    if (!node) return;

    switch (node->type) {
        case AST_VAR_DECL:
            if (is_local_var(node->data.var_decl.name)) {
                fprintf(stderr, "error: variable '%s' already declared\n",
                        node->data.var_decl.name);
                error_count++;
            } else {
                push_local_var(node->data.var_decl.name);
            }
            check_expr(node->data.var_decl.value);
            break;

        case AST_ASSIGNMENT:
            // Check if variable is defined
            if (!is_local_var(node->data.assignment.name) &&
                !is_global_var(node->data.assignment.name)) {
                fprintf(stderr, "error: undefined variable '%s'\n",
                        node->data.assignment.name);
                error_count++;
            }
            check_expr(node->data.assignment.value);
            break;

        case AST_RETURN:
            check_expr(node->data.return_value);
            break;

        case AST_PRINT:
            check_expr(node->data.print_value);
            break;

        case AST_SEQUENCE:
            check_statement(node->data.sequence.first);
            check_statement(node->data.sequence.second);
            break;

        case AST_WHILE:
            check_expr(node->data.while_loop.condition);
            check_statement(node->data.while_loop.body);
            break;

        case AST_FOR:
            check_statement(node->data.for_loop.init);
            check_expr(node->data.for_loop.condition);
            check_statement(node->data.for_loop.increment);
            check_statement(node->data.for_loop.body);
            break;

        case AST_NUMBER:
        case AST_VARIABLE:
        case AST_BINARY_OP:
        case AST_FUNCTION_CALL:
            // Expression statement
            check_expr(node);
            break;

        default:
            break;
    }
}

// Check a function body
static void check_function(ASTNode *node) {
    if (!node || node->type != AST_FUNCTION_DEF) return;

    // Reset local variables
    free_local_vars();
    local_vars = NULL;

    // Add parameters to local scope
    ParamList *params = node->data.function_def.params;
    while (params) {
        push_local_var(params->name);
        params = params->next;
    }

    // Check function body
    check_statement(node->data.function_def.body);

    // Clean up local variables
    free_local_vars();
    local_vars = NULL;
}

// Second pass: check function bodies
static void check_functions(ASTNode *node) {
    if (!node) return;

    if (node->type == AST_FUNCTION_DEF) {
        check_function(node);
    } else if (node->type == AST_SEQUENCE) {
        check_functions(node->data.sequence.first);
        check_functions(node->data.sequence.second);
    }
}

int sema_check(ASTNode *root) {
    error_count = 0;
    functions = NULL;
    global_vars = NULL;
    local_vars = NULL;

    // First pass: collect all function definitions and global variables
    collect_definitions(root);

    // Second pass: check function bodies
    check_functions(root);

    // Clean up
    free_functions();
    free_global_vars();

    return error_count;
}
