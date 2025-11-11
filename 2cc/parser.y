%{
#include <stdio.h>
#include <stdlib.h>

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
    expr { 
        printf("  ldr x0, [sp], #16\n");
    }
    ;

expr:
    NUMBER {
        printf("  mov x0, #%d\n", $1);
        printf("  str x0, [sp, #-16]!\n");
    }
    | expr ADD expr {
        printf("  ldr x1, [sp], #16\n");
        printf("  ldr x0, [sp], #16\n");
        printf("  add x0, x0, x1\n");
        printf("  str x0, [sp, #-16]!\n");
    }
    | expr SUB expr {
        printf("  ldr x1, [sp], #16\n");
        printf("  ldr x0, [sp], #16\n");
        printf("  sub x0, x0, x1\n");
        printf("  str x0, [sp, #-16]!\n");
    }
    | expr MUL expr {
        printf("  ldr x1, [sp], #16\n");
        printf("  ldr x0, [sp], #16\n");
        printf("  mul x0, x0, x1\n");
        printf("  str x0, [sp, #-16]!\n");
    }
    | expr DIV expr {
        printf("  ldr x1, [sp], #16\n");
        printf("  ldr x0, [sp], #16\n");
        printf("  sdiv x0, x0, x1\n");
        printf("  str x0, [sp, #-16]!\n");
    }
    | LPAREN expr RPAREN
    ;
%%

void yyerror(char *s) {
    fprintf(stderr, "%s\n", s);
}
