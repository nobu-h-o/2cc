#ifndef AST_H
#define AST_H

typedef enum {
    AST_NUMBER,
    AST_BINARY_OP,
    AST_VARIABLE,
    AST_ASSIGNMENT,
    AST_RETURN,
    AST_SEQUENCE,
    AST_WHILE,
    AST_FOR,
    AST_PRINT,
    AST_FUNCTION_DEF,
    AST_FUNCTION_CALL,
    AST_GLOBAL_VAR,
} ASTNodeType;

typedef enum {
    OP_ADD,
    OP_SUB,
    OP_MUL,
    OP_DIV,
    OP_LT,
    OP_GT,
    OP_LE,
    OP_GE,
    OP_EQ,
    OP_NE,
} BinaryOp;

typedef struct ParamList {
    char *name;
    struct ParamList *next;
} ParamList;

typedef struct ArgList {
    struct ASTNode *expr;
    struct ArgList *next;
} ArgList;

typedef struct GlobalVar {
    char *name;
    int value;
    struct GlobalVar *next;
} GlobalVar;

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
        struct {
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_loop;
        struct {
            struct ASTNode *init;
            struct ASTNode *condition;
            struct ASTNode *increment;
            struct ASTNode *body;
        } for_loop;
        struct ASTNode *print_value;
        struct {
            char *name;
            ParamList *params;
            struct ASTNode *body;
        } function_def;
        struct {
            char *name;
            ArgList *args;
        } function_call;
        struct {
            char *name;
            struct ASTNode *value;
        } global_var;
    } data;
} ASTNode;

ASTNode* ast_number(int value);
ASTNode* ast_binary(BinaryOp op, ASTNode *left, ASTNode *right);
ASTNode* ast_variable(char *name);
ASTNode* ast_assignment(char *name, ASTNode *value);
ASTNode* ast_return(ASTNode *value);
ASTNode* ast_sequence(ASTNode *first, ASTNode *second);
ASTNode* ast_while(ASTNode *condition, ASTNode *body);
ASTNode* ast_for(ASTNode *init, ASTNode *condition, ASTNode *increment, ASTNode *body);
ASTNode* ast_print(ASTNode *value);
ASTNode* ast_function_def(char *name, ParamList *params, ASTNode *body);
ASTNode* ast_function_call(char *name, ArgList *args);
ASTNode* ast_global_var(char *name, ASTNode *value);
ParamList* param_list_create(char *name, ParamList *next);
ArgList* arg_list_create(ASTNode *expr, ArgList *next);
int param_list_count(ParamList *params);
int arg_list_count(ArgList *args);
void param_list_free(ParamList *params);
void arg_list_free(ArgList *args);
void ast_free(ASTNode *node);

// Global variable collection
GlobalVar* collect_global_vars(ASTNode *root);
void global_vars_free(GlobalVar *globals);

#endif /* AST_H */
