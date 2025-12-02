#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"
#include "symtab.h"

extern SymbolTable *global_symtab;
extern int stack_depth;

void codegen_from_ast(ASTNode *node);
void codegen_finish(void);

#endif /* CODEGEN_H */
