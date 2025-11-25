#ifndef AST_H
#define AST_H

typedef enum {
    AST_NUMBER,
    AST_BINARY_OP,
} ASTNodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
} BinaryOp;

typedef struct ASTNode {
    ASTNodeType type;
    union {
        int number;
        struct {
            BinaryOp op;
            struct ASTNode *left;
            struct ASTNode *right;
        } binary;
    } data;
} ASTNode;

ASTNode* ast_number(int value);
ASTNode* ast_binary(BinaryOp op, ASTNode *left, ASTNode *right);
void ast_free(ASTNode *node);

#endif /* AST_H */
