%{
#include "calc.tab.h"
#include <stdlib.h>
%}

%%
[0-9]+          { yylval = atoi(yytext); return NUMBER; }
[ \t]           ; /* skip whitespace */
\n              { return EOL; }
"+"             { return PLUS; }
"-"             { return MINUS; }
"*"             { return TIMES; }
"/"             { return DIVIDE; }
"("             { return LPAREN; }
")"             { return RPAREN; }
.               { printf("Unknown character: %s\n", yytext); }
%%

int yywrap(void) { return 1; }
