#ifndef AST_H
#define AST_H

typedef enum {
    AST_NUMBER,
    AST_BINARY_OP,
    AST_VARIABLE,
    AST_ASSIGNMENT,
    AST_RETURN,
    AST_SEQUENCE,
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
        char *variable;
        struct {
            char *name;
            struct ASTNode *value;
        } assignment;
        struct ASTNode *return_value;
        struct {
            struct ASTNode *first;
            struct ASTNode *second;
        } sequence;
    } data;
} ASTNode;

ASTNode* ast_number(int value);
ASTNode* ast_binary(BinaryOp op, ASTNode *left, ASTNode *right);
ASTNode* ast_variable(char *name);
ASTNode* ast_assignment(char *name, ASTNode *value);
ASTNode* ast_return(ASTNode *value);
ASTNode* ast_sequence(ASTNode *first, ASTNode *second);
void ast_free(ASTNode *node);

#endif /* AST_H */
