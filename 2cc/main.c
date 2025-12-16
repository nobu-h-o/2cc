#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "parser.h"
#include "codegen.h"
#include "ast.h"
#include "symtab.h"

extern void yy_scan_string(const char *str);
extern ASTNode *root;
extern SymbolTable *global_symtab;
extern int stack_depth;

static int check_main_exists(ASTNode *node) {
    if (!node) return 0;

    if (node->type == AST_FUNCTION_DEF) {
        if (strcmp(node->data.function_def.name, "main") == 0) {
            return 1;
        }
    } else if (node->type == AST_SEQUENCE) {
        return check_main_exists(node->data.sequence.first) ||
               check_main_exists(node->data.sequence.second);
    }

    return 0;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        fprintf(stderr, "the number of arguments is incorrect\n");
        return 1;
    }

    global_symtab = symtab_create();

    yy_scan_string(argv[1]);
    yyparse();

    if (root) {
        // Check if main function exists (only needed for explicit function definitions)
        // If the root is a function_def with name "main", we're in implicit main mode
        // Otherwise, check if main exists
        if (root->type == AST_FUNCTION_DEF && strcmp(root->data.function_def.name, "main") == 0) {
            // Single main function (implicit or explicit) - OK
        } else if (root->type == AST_SEQUENCE || root->type == AST_FUNCTION_DEF) {
            // Multiple functions or single non-main function - check if main exists
            if (!check_main_exists(root)) {
                fprintf(stderr, "Error: main() function is required\n");
                ast_free(root);
                symtab_free(global_symtab);
                return 1;
            }
        }

        // Generate code for all functions
        codegen_program(root);

        ast_free(root);
    }

    symtab_free(global_symtab);

    return 0;
}
