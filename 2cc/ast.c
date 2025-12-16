#include <stdlib.h>
#include <string.h>
#include "ast.h"

ASTNode* ast_number(int value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_NUMBER;
    node->data.number = value;
    return node;
}

ASTNode* ast_binary(BinaryOp op, ASTNode *left, ASTNode *right) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_BINARY_OP;
    node->data.binary.op = op;
    node->data.binary.left = left;
    node->data.binary.right = right;
    return node;
}

ASTNode* ast_variable(char *name) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_VARIABLE;
    node->data.variable = strdup(name);
    return node;
}

ASTNode* ast_assignment(char *name, ASTNode *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_ASSIGNMENT;
    node->data.assignment.name = strdup(name);
    node->data.assignment.value = value;
    return node;
}

ASTNode* ast_return(ASTNode *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_RETURN;
    node->data.return_value = value;
    return node;
}

ASTNode* ast_sequence(ASTNode *first, ASTNode *second) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_SEQUENCE;
    node->data.sequence.first = first;
    node->data.sequence.second = second;
    return node;
}

ASTNode* ast_while(ASTNode *condition, ASTNode *body) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_WHILE;
    node->data.while_loop.condition = condition;
    node->data.while_loop.body = body;
    return node;
}

ASTNode* ast_for(ASTNode *init, ASTNode *condition, ASTNode *increment, ASTNode *body) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_FOR;
    node->data.for_loop.init = init;
    node->data.for_loop.condition = condition;
    node->data.for_loop.increment = increment;
    node->data.for_loop.body = body;
    return node;
}

ASTNode* ast_print(ASTNode *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_PRINT;
    node->data.print_value = value;
    return node;
}

ASTNode* ast_function_def(char *name, ParamList *params, ASTNode *body) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_FUNCTION_DEF;
    node->data.function_def.name = strdup(name);
    node->data.function_def.params = params;
    node->data.function_def.body = body;
    return node;
}

ASTNode* ast_function_call(char *name, ArgList *args) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_FUNCTION_CALL;
    node->data.function_call.name = strdup(name);
    node->data.function_call.args = args;
    return node;
}

ASTNode* ast_global_var(char *name, ASTNode *value) {
    ASTNode *node = (ASTNode *)malloc(sizeof(ASTNode));
    node->type = AST_GLOBAL_VAR;
    node->data.global_var.name = strdup(name);
    node->data.global_var.value = value;
    return node;
}

ParamList* param_list_create(char *name, ParamList *next) {
    ParamList *param = (ParamList *)malloc(sizeof(ParamList));
    param->name = strdup(name);
    param->next = next;
    return param;
}

ArgList* arg_list_create(ASTNode *expr, ArgList *next) {
    ArgList *arg = (ArgList *)malloc(sizeof(ArgList));
    arg->expr = expr;
    arg->next = next;
    return arg;
}

int param_list_count(ParamList *params) {
    int count = 0;
    while (params) {
        count++;
        params = params->next;
    }
    return count;
}

int arg_list_count(ArgList *args) {
    int count = 0;
    while (args) {
        count++;
        args = args->next;
    }
    return count;
}

void param_list_free(ParamList *params) {
    while (params) {
        ParamList *next = params->next;
        free(params->name);
        free(params);
        params = next;
    }
}

void arg_list_free(ArgList *args) {
    while (args) {
        ArgList *next = args->next;
        ast_free(args->expr);
        free(args);
        args = next;
    }
}

void ast_free(ASTNode *node) {
    if (!node) return;

    if (node->type == AST_BINARY_OP) {
        ast_free(node->data.binary.left);
        ast_free(node->data.binary.right);
    } else if (node->type == AST_VARIABLE) {
        free(node->data.variable);
    } else if (node->type == AST_ASSIGNMENT) {
        free(node->data.assignment.name);
        ast_free(node->data.assignment.value);
    } else if (node->type == AST_RETURN) {
        ast_free(node->data.return_value);
    } else if (node->type == AST_SEQUENCE) {
        ast_free(node->data.sequence.first);
        ast_free(node->data.sequence.second);
    } else if (node->type == AST_WHILE) {
        ast_free(node->data.while_loop.condition);
        ast_free(node->data.while_loop.body);
    } else if (node->type == AST_FOR) {
        ast_free(node->data.for_loop.init);
        ast_free(node->data.for_loop.condition);
        ast_free(node->data.for_loop.increment);
        ast_free(node->data.for_loop.body);
    } else if (node->type == AST_PRINT) {
        ast_free(node->data.print_value);
    } else if (node->type == AST_FUNCTION_DEF) {
        free(node->data.function_def.name);
        param_list_free(node->data.function_def.params);
        ast_free(node->data.function_def.body);
    } else if (node->type == AST_FUNCTION_CALL) {
        free(node->data.function_call.name);
        arg_list_free(node->data.function_call.args);
    } else if (node->type == AST_GLOBAL_VAR) {
        free(node->data.global_var.name);
        ast_free(node->data.global_var.value);
    }

    free(node);
}

static GlobalVar *global_vars_list = NULL;

static void add_global_var(const char *name, int value) {
    GlobalVar *new_var = (GlobalVar *)malloc(sizeof(GlobalVar));
    new_var->name = strdup(name);
    new_var->value = value;
    new_var->next = global_vars_list;
    global_vars_list = new_var;
}

static void collect_globals(ASTNode *node) {
    if (!node) return;

    if (node->type == AST_GLOBAL_VAR) {
        // Extract constant value if it's a simple number
        int init_value = 0;
        if (node->data.global_var.value && node->data.global_var.value->type == AST_NUMBER) {
            init_value = node->data.global_var.value->data.number;
        }
        add_global_var(node->data.global_var.name, init_value);
    } else if (node->type == AST_SEQUENCE) {
        collect_globals(node->data.sequence.first);
        collect_globals(node->data.sequence.second);
    }
    // Don't recurse into function bodies
}

GlobalVar* collect_global_vars(ASTNode *root) {
    global_vars_list = NULL;
    collect_globals(root);
    return global_vars_list;
}

void global_vars_free(GlobalVar *globals) {
    while (globals) {
        GlobalVar *next = globals->next;
        free(globals->name);
        free(globals);
        globals = next;
    }
}
