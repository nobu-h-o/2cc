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
    }

    free(node);
}
