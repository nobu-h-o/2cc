#ifndef SEMA_H
#define SEMA_H

#include "ast.h"

// Perform semantic analysis on the AST
// Returns the number of errors found
int sema_check(ASTNode *root);

#endif /* SEMA_H */
