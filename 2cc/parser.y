%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"
#include "symtab.h"

void yyerror(char *s);
int yylex(void);

ASTNode *root = NULL;
extern SymbolTable *global_symtab;
%}

%union {
    int number;
    char *string;
    ASTNode *node;
}

%token NUMBER
%token IDENTIFIER
%token ASSIGN SEMICOLON
%token RETURN WHILE FOR
%token ADD SUB MUL DIV
%token LPAREN RPAREN LBRACE RBRACE
%token LT GT LE GE EQ NE

%left EQ NE
%left LT GT LE GE
%left ADD SUB
%left MUL DIV
%nonassoc UNARY

%type <node> expr statement statements
%type <number> NUMBER
%type <string> IDENTIFIER

%%
program:
    statements { root = $1; }
    | expr { root = $1; }
    | RETURN expr { root = ast_return($2); }
    | IDENTIFIER ASSIGN expr {
        if (symtab_lookup(global_symtab, $1) < 0) {
            symtab_add(global_symtab, $1);
        }
        root = ast_assignment($1, $3);
    };

statements:
    statement { $$ = $1; }
    | statements statement { $$ = ast_sequence($1, $2); };

statement:
    IDENTIFIER ASSIGN expr SEMICOLON {
        if (symtab_lookup(global_symtab, $1) < 0) {
            symtab_add(global_symtab, $1);
        }
        $$ = ast_assignment($1, $3);
    }
    | RETURN expr SEMICOLON { $$ = ast_return($2); }
    | expr SEMICOLON { $$ = $1; }
    | WHILE LPAREN expr RPAREN LBRACE statements RBRACE {
        $$ = ast_while($3, $6);
    }
    | FOR LPAREN statement expr SEMICOLON IDENTIFIER ASSIGN expr RPAREN LBRACE statements RBRACE {
        if (symtab_lookup(global_symtab, $6) < 0) {
            symtab_add(global_symtab, $6);
        }
        $$ = ast_for($3, $4, ast_assignment($6, $8), $11);
    };

expr:
    NUMBER { $$ = ast_number($1); }
    | IDENTIFIER {
        if (symtab_lookup(global_symtab, $1) < 0) {
            symtab_add(global_symtab, $1);
        }
        $$ = ast_variable($1);
    }
    | expr ADD expr { $$ = ast_binary(OP_ADD, $1, $3); }
    | expr SUB expr { $$ = ast_binary(OP_SUB, $1, $3); }
    | expr MUL expr { $$ = ast_binary(OP_MUL, $1, $3); }
    | expr DIV expr { $$ = ast_binary(OP_DIV, $1, $3); }
    | expr LT expr { $$ = ast_binary(OP_LT, $1, $3); }
    | expr GT expr { $$ = ast_binary(OP_GT, $1, $3); }
    | expr LE expr { $$ = ast_binary(OP_LE, $1, $3); }
    | expr GE expr { $$ = ast_binary(OP_GE, $1, $3); }
    | expr EQ expr { $$ = ast_binary(OP_EQ, $1, $3); }
    | expr NE expr { $$ = ast_binary(OP_NE, $1, $3); }
    | LPAREN expr RPAREN { $$ = $2; };
%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}
