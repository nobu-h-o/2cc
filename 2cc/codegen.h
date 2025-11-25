#ifndef CODEGEN_H
#define CODEGEN_H

#include "ast.h"

void codegen_from_ast(ASTNode *node);
void codegen_finish(void);

#endif /* CODEGEN_H */
