#pragma once

typedef struct Symbol {
    char *name;
    int initialized;
    int is_primitive;
    struct Symbol *next;
} Symbol;

typedef struct Scope {
    Symbol *symbols;
    struct Scope *parent;
} Scope;

typedef struct {
    Scope *current;
} SymbolTable;

SymbolTable *symtab_create(void);
void symtab_enter_scope(SymbolTable *t);
void symtab_exit_scope(SymbolTable *t);

void symtab_seed_primitives(SymbolTable *t);

Symbol *symtab_insert(SymbolTable *t, const char *name);

Symbol *symtab_lookup(SymbolTable *t, const char *name);
Symbol *symtab_lookup_scope(Scope *t, const char *name);

Symbol *symtab_lookup_current(SymbolTable *t, const char *name);
Symbol *symtab_lookup_current_scope(Scope *t, const char *name);

void symtab_destroy(SymbolTable *t);

Scope *scope_copy(const Scope *scope);

void scope_free(Scope *scope);