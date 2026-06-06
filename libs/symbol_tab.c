#include "symbol_tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static const char *PRIMITIVES[] = {

    "eq?", "eqv?", "equal?",

    "not", "boolean?",

    "number?", "complex?", "real?", "rational?", "integer?",
    "exact?", "inexact?",
    "=", "<", ">", "<=", ">=",
    "zero?", "positive?", "negative?", "odd?", "even?",
    "max", "min", "+", "*", "-", "/",
    "abs", "quotient", "remainder", "modulo", "gcd", "lcm",
    "numerator", "denominator",
    "floor", "ceiling", "truncate", "round", "rationalize",
    "exp", "log", "sin", "cos", "tan", "asin", "acos", "atan",
    "sqrt", "expt",
    "make-rectangular", "make-polar", "real-part", "imag-part",
    "magnitude", "angle",
    "exact->inexact", "inexact->exact",
    "number->string", "string->number",

    "pair?", "cons", "car", "cdr", "set-car!", "set-cdr!",
    "caar", "cadr", "cdar", "cddr",
    "caaar", "caadr", "cadar", "caddr", "cdaar", "cdadr", "cddar", "cdddr",
    "caaaar", "caaadr", "caadar", "caaddr", "cadaar", "cadadr", "caddar", "cadddr",
    "cdaaar", "cdaadr", "cdadar", "cdaddr", "cddaar", "cddadr", "cdddar", "cddddr",
    "null?", "list?", "list", "length", "append", "reverse",
    "list-tail", "list-ref",
    "memq", "memv", "member", "assq", "assv", "assoc",

    "symbol?", "symbol->string", "string->symbol",

    "char?", "char=?", "char<?", "char>?", "char<=?", "char>=?",
    "char-ci=?", "char-ci<?", "char-ci>?", "char-ci<=?", "char-ci>=?",
    "char-alphabetic?", "char-numeric?", "char-whitespace?",
    "char-upper-case?", "char-lower-case?",
    "char->integer", "integer->char", "char-upcase", "char-downcase",

    "string?", "make-string", "string", "string-length", "string-ref",
    "string-set!",
    "string=?", "string-ci=?", "string<?", "string>?", "string<=?", "string>=?",
    "string-ci<?", "string-ci>?", "string-ci<=?", "string-ci>=?",
    "substring", "string-append", "string->list", "list->string",
    "string-copy", "string-fill!",

    "vector?", "make-vector", "vector", "vector-length", "vector-ref",
    "vector-set!", "vector->list", "list->vector", "vector-fill!",

    "procedure?", "apply", "map", "for-each", "force",
    "call-with-current-continuation", "values", "call-with-values",
    "dynamic-wind",

    "eval", "scheme-report-environment", "null-environment",
    "interaction-environment",

    "call-with-input-file", "call-with-output-file",
    "input-port?", "output-port?", "current-input-port", "current-output-port",
    "with-input-from-file", "with-output-to-file",
    "open-input-file", "open-output-file",
    "close-input-port", "close-output-port",
    "read", "read-char", "peek-char", "eof-object?", "char-ready?",
    "write", "display", "newline", "write-char",
    "load", "transcript-on", "transcript-off",
};

void symtab_seed_primitives(SymbolTable *t)
{
    size_t n = sizeof(PRIMITIVES) / sizeof(PRIMITIVES[0]);
    for (size_t i = 0; i < n; i++)
    {
        Symbol *s = symtab_insert(t, PRIMITIVES[i]);
        if (s)
        {
            s->initialized = 1;
            s->is_primitive = 1;
        }
    }
}

SymbolTable *symtab_create(void)
{
    SymbolTable *t = malloc(sizeof(SymbolTable));
    t->current = NULL;
    symtab_enter_scope(t);
    symtab_seed_primitives(t);
    return t;
}

void symtab_enter_scope(SymbolTable *t)
{
    Scope *s = malloc(sizeof(Scope));
    s->symbols = NULL;
    s->parent = t->current;
    t->current = s;
}

void symtab_exit_scope(SymbolTable *t)
{
    Scope *s = t->current;
    if (!s)
        return;
    t->current = s->parent;

}

Symbol *symtab_lookup_current(SymbolTable *t, const char *name)
{
    for (Symbol *s = t->current->symbols; s; s = s->next)
        if (strcmp(s->name, name) == 0)
            return s;
    return NULL;
}

Symbol *symtab_lookup_current_scope(Scope *t, const char *name)
{
    for (Symbol *s = t->symbols; s; s = s->next)
        if (strcmp(s->name, name) == 0)
            return s;
    return NULL;
}

Symbol *symtab_insert(SymbolTable *t, const char *name)
{

    if (symtab_lookup_current(t, name) != NULL) return NULL;
    Symbol *s = malloc(sizeof(Symbol));
    s->name = strdup(name);
    s->is_primitive = 0;
    s->next = t->current->symbols;
    t->current->symbols = s;
    return s;
}

Symbol *symtab_lookup(SymbolTable *t, const char *name)
{
    for (Scope *sc = t->current; sc; sc = sc->parent)
        for (Symbol *s = sc->symbols; s; s = s->next)
            if (strcmp(s->name, name) == 0)
                return s;
    return NULL;
}

Symbol *symtab_lookup_scope(Scope *t, const char *name)
{
    for (Scope *sc = t; sc; sc = sc->parent)
        for (Symbol *s = sc->symbols; s; s = s->next)
            if (strcmp(s->name, name) == 0)
                return s;
    return NULL;
}

void symtab_destroy(SymbolTable *t)
{
    while (t->current)
        symtab_exit_scope(t);
    free(t);
}

static Symbol *symbol_copy_list(const Symbol *src)
{
    if (!src)
        return NULL;
    Symbol *s = malloc(sizeof(Symbol));
    s->name = strdup(src->name);
    s->initialized = src->initialized;
    s->is_primitive = src->is_primitive;
    s->next = symbol_copy_list(src->next);
    return s;
}

Scope *scope_copy(const Scope *scope)
{
    if (!scope)
        return NULL;
    Scope *c = malloc(sizeof(Scope));
    c->symbols = symbol_copy_list(scope->symbols);
    c->parent = scope_copy(scope->parent);
    return c;
}

static void symbol_free_list(Symbol *s)
{
    while (s)
    {
        Symbol *next = s->next;
        free(s->name);
        free(s);
        s = next;
    }
}

void scope_free(Scope *scope)
{
    while (scope)
    {
        Scope *parent = scope->parent;
        symbol_free_list(scope->symbols);
        free(scope);
        scope = parent;
    }
}