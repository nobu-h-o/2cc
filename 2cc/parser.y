%{
#include <stdio.h>
#include <stdlib.h>
#include "ast.h"

void yyerror(char *s);
int yylex(void);

ASTNode *root = NULL;
%}

%union {
    int number;
    ASTNode *node;
}

%token NUMBER
%token ADD SUB MUL DIV
%token LPAREN RPAREN

%left ADD SUB
%left MUL DIV
%nonassoc UNARY

%type <node> expr
%type <number> NUMBER

%%
program:
    expr { root = $1; };

expr:
    NUMBER { $$ = ast_number($1); }
    | expr ADD expr { $$ = ast_binary(OP_ADD, $1, $3); }
    | expr SUB expr { $$ = ast_binary(OP_SUB, $1, $3); }
    | expr MUL expr { $$ = ast_binary(OP_MUL, $1, $3); }
    | expr DIV expr { $$ = ast_binary(OP_DIV, $1, $3); }
    | LPAREN expr RPAREN { $$ = $2; };
%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}
