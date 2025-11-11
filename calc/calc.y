%{
#include <stdio.h>
#include <stdlib.h>
int yylex(void);
void yyerror(char *s);
%}

%token NUMBER
%token PLUS MINUS TIMES DIVIDE
%token LPAREN RPAREN
%token EOL

%left PLUS MINUS
%left TIMES DIVIDE

%%
calclist:
    | calclist exp EOL { printf("= %d\n> ", $2); }
    ;

exp: 
    NUMBER              { $$ = $1; }
    | exp PLUS exp      { $$ = $1 + $3; }
    | exp MINUS exp     { $$ = $1 - $3; }
    | exp TIMES exp     { $$ = $1 * $3; }
    | exp DIVIDE exp    { $$ = $1 / $3; }
    | LPAREN exp RPAREN { $$ = $2; }
    ;
%%

void yyerror(char *s) {
    fprintf(stderr, "error: %s\n", s);
}

int main(void) {
    printf("Simple Calculator (Ctrl+D to exit)\n> ");
    yyparse();
    return 0;
}
