#include "symbol_tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

SymbolTable *symtab_create(void) {
    SymbolTable *t = malloc(sizeof(SymbolTable));
    t->current = NULL;
    symtab_enter_scope(t);   // escopo global
    return t;
}

void symtab_enter_scope(SymbolTable *t) {
    Scope *s = malloc(sizeof(Scope));
    s->symbols = NULL;
    s->parent = t->current;
    t->current = s;
}

void symtab_exit_scope(SymbolTable *t) {
    Scope *s = t->current;
    if (!s) return;
    t->current = s->parent;
    Symbol *sym = s->symbols;
    // while (sym) {
    //     Symbol *next = sym->next;
    //     free(sym->name);
    //     free(sym);
    //     sym = next;
    // }
    // free(s);
}

Symbol *symtab_lookup_current(SymbolTable *t, const char *name) {
    for (Symbol *s = t->current->symbols; s; s = s->next)
        if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

Symbol *symtab_insert(SymbolTable *t, const char *name) {
    if (symtab_lookup_current(t, name)) return NULL; // redeclaração
    Symbol *s = malloc(sizeof(Symbol));
    s->name = strdup(name);
    s->initialized = 0;
    s->next = t->current->symbols;
    t->current->symbols = s;
    return s;
}

Symbol *symtab_lookup(SymbolTable *t, const char *name) {
    for (Scope *sc = t->current; sc; sc = sc->parent)
        for (Symbol *s = sc->symbols; s; s = s->next)
            if (strcmp(s->name, name) == 0) return s;
    return NULL;
}

void symtab_print(SymbolTable *t) {
    printf("===== Tabela de simbolos =====\n");
    if (!t || !t->current) {
        printf("(vazia)\n");
        printf("==============================\n");
        return;
    }

    // descobre a profundidade do topo: global = nivel 0
    int depth = 0;
    for (Scope *sc = t->current; sc; sc = sc->parent) depth++;

    // imprime do escopo mais interno (topo da pilha) ate o global
    int level = depth - 1;
    for (Scope *sc = t->current; sc; sc = sc->parent, level--) {
        printf("-- Escopo %d%s --\n", level, level == 0 ? " (global)" : "");
        if (!sc->symbols) {
            printf("   (nenhum simbolo)\n");
        }
        for (Symbol *s = sc->symbols; s; s = s->next) {
            printf("   %-20s [%s]\n", s->name,
                   s->initialized ? "inicializado" : "nao inicializado");
        }
    }
    printf("==============================\n");
}

void symtab_destroy(SymbolTable *t) {
    while (t->current) symtab_exit_scope(t);
    free(t);
}