%{
#include "parser.tab.h"
%}

%%
[0-9]+      { yylval = atoi(yytext); return NUMBER; }
"+"         { return ADD; }
"-"         { return SUB; }
"*"         { return MUL; }
"/"         { return DIV; }
"("         { return LPAREN; }
")"         { return RPAREN; }
[ \t\n]     { /* ignore whitespace */ }
.           { fprintf(stderr, "Unknown character: %s\n", yytext); exit(1); }
%%

int yywrap(void) {
    return 1;
}
