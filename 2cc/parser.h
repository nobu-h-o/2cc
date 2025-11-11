#ifndef PARSER_H
#define PARSER_H

int yyparse(void);
int yylex(void);
void yyerror(char *s);
void yy_scan_string(const char *str);

#endif
