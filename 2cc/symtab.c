#include <stdlib.h>
#include <string.h>
#include "symtab.h"

#define INITIAL_CAPACITY 10

SymbolTable* symtab_create(void) {
    SymbolTable *table = (SymbolTable *)malloc(sizeof(SymbolTable));
    table->symbols = (Symbol *)malloc(sizeof(Symbol) * INITIAL_CAPACITY);
    table->count = 0;
    table->capacity = INITIAL_CAPACITY;
    table->stack_offset = 0;
    return table;
}

void symtab_free(SymbolTable *table) {
    if (!table) return;
    for (int i = 0; i < table->count; i++) {
        free(table->symbols[i].name);
    }
    free(table->symbols);
    free(table);
}

int symtab_lookup(SymbolTable *table, const char *name) {
    for (int i = 0; i < table->count; i++) {
        if (strcmp(table->symbols[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int symtab_add(SymbolTable *table, const char *name) {
    if (symtab_lookup(table, name) >= 0) {
        return -1;  /* Variable already exists */
    }

    if (table->count >= table->capacity) {
        table->capacity *= 2;
        table->symbols = (Symbol *)realloc(table->symbols, sizeof(Symbol) * table->capacity);
    }

    table->symbols[table->count].name = strdup(name);
    table->symbols[table->count].value = 0;
    table->symbols[table->count].offset = table->stack_offset;
    table->stack_offset += 16;  /* Each variable takes 16 bytes on stack */

    return table->count++;
}

void symtab_set(SymbolTable *table, const char *name, int value) {
    int idx = symtab_lookup(table, name);
    if (idx >= 0) {
        table->symbols[idx].value = value;
    }
}

int symtab_get(SymbolTable *table, const char *name) {
    int idx = symtab_lookup(table, name);
    if (idx >= 0) {
        return table->symbols[idx].value;
    }
    return 0;  /* Default value */
}

int symtab_get_offset(SymbolTable *table, const char *name) {
    int idx = symtab_lookup(table, name);
    if (idx >= 0) {
        return table->symbols[idx].offset;
    }
    return 0;
}
