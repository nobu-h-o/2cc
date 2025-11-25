#include <stdlib.h>
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

void ast_free(ASTNode *node) {
    if (!node) return;

    if (node->type == AST_BINARY_OP) {
        ast_free(node->data.binary.left);
        ast_free(node->data.binary.right);
    }

    free(node);
}
