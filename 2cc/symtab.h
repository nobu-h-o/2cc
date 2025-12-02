#ifndef SYMTAB_H
#define SYMTAB_H

typedef struct {
    char *name;
    int value;
    int offset;  /* Stack offset from x29 */
} Symbol;

typedef struct {
    Symbol *symbols;
    int count;
    int capacity;
    int stack_offset;
} SymbolTable;

SymbolTable* symtab_create(void);
void symtab_free(SymbolTable *table);
int symtab_add(SymbolTable *table, const char *name);
int symtab_lookup(SymbolTable *table, const char *name);
void symtab_set(SymbolTable *table, const char *name, int value);
int symtab_get(SymbolTable *table, const char *name);
int symtab_get_offset(SymbolTable *table, const char *name);

#endif /* SYMTAB_H */
