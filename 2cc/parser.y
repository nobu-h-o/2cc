%{
#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

void yyerror(char *s);
int yylex(void);
%}

%token NUMBER
%token ADD SUB MUL DIV
%token LPAREN RPAREN

%left ADD SUB
%left MUL DIV
%nonassoc UNARY

%%
program:
    expr { codegen_finish(); };

expr:
    NUMBER { codegen_number($1); }
    | expr ADD expr { codegen_add(); }
    | expr SUB expr { codegen_sub(); }
    | expr MUL expr { codegen_mul(); }
    | expr DIV expr { codegen_div(); }
    | LPAREN expr RPAREN;
%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}
