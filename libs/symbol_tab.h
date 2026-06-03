#pragma once

typedef struct Symbol {
    char *name;
    int initialized;        // já recebeu um valor?
    int is_primitive;       // procedimento embutido do Scheme (+, car, ...)?
    struct Symbol *next;    // próximo símbolo do mesmo escopo
} Symbol;

typedef struct Scope {
    Symbol *symbols;        // lista ligada de símbolos deste escopo
    struct Scope *parent;   // escopo que contém este (encadeamento léxico)
} Scope;

typedef struct {
    Scope *current;         // topo da pilha
} SymbolTable;

SymbolTable *symtab_create(void);
void symtab_enter_scope(SymbolTable *t);
void symtab_exit_scope(SymbolTable *t);

// declara no escopo atual os procedimentos primitivos do Scheme (+, car, ...).
// chamada automaticamente por symtab_create no escopo global.
void symtab_seed_primitives(SymbolTable *t);

// insere no escopo atual; NULL se já existir NO MESMO escopo
Symbol *symtab_insert(SymbolTable *t, const char *name);

// busca do escopo atual até o global; NULL se não achar
Symbol *symtab_lookup(SymbolTable *t, const char *name);
Symbol *symtab_lookup_scope(Scope *t, const char *name);

// busca só no escopo atual (para detectar redeclaração)
Symbol *symtab_lookup_current(SymbolTable *t, const char *name);
Symbol *symtab_lookup_current_scope(Scope *t, const char *name);

// imprime a tabela de símbolos (todos os escopos da pilha) para depuração
void symtab_print(SymbolTable *t);

void symtab_destroy(SymbolTable *t);

// cópia profunda de um escopo, incluindo seus símbolos e toda a cadeia de
// escopos pais (encadeamento léxico). Usada para guardar o escopo visível
// dentro de cada nó da AST.
Scope *scope_copy(const Scope *scope);

// libera um escopo copiado por scope_copy (e toda a sua cadeia de pais).
void scope_free(Scope *scope);