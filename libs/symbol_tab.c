#include "symbol_tab.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* procedimentos primitivos do Scheme: ja' vem ligados no ambiente inicial da
 * linguagem (R5RS). Sao identificadores como qualquer outro (ex: '+'), por isso
 * precisam estar declarados no escopo global antes das passadas semanticas.
 *
 * Esta lista contem apenas PROCEDIMENTOS (que sao variaveis). As "special forms"
 * (define, lambda, if, set!, quote, begin, let, let*, letrec, cond, case, and,
 * or, do, delay, else, =>) NAO entram aqui: sao palavras-chave sintaticas
 * tratadas diretamente pela gramatica, nunca aparecem como variavel/operador. */
static const char *PRIMITIVOS[] = {
    /* equivalencia */
    "eq?", "eqv?", "equal?",

    /* booleanos */
    "not", "boolean?",

    /* numeros */
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

    /* pares e listas */
    "pair?", "cons", "car", "cdr", "set-car!", "set-cdr!",
    "caar", "cadr", "cdar", "cddr",
    "caaar", "caadr", "cadar", "caddr", "cdaar", "cdadr", "cddar", "cdddr",
    "caaaar", "caaadr", "caadar", "caaddr", "cadaar", "cadadr", "caddar", "cadddr",
    "cdaaar", "cdaadr", "cdadar", "cdaddr", "cddaar", "cddadr", "cdddar", "cddddr",
    "null?", "list?", "list", "length", "append", "reverse",
    "list-tail", "list-ref",
    "memq", "memv", "member", "assq", "assv", "assoc",

    /* simbolos */
    "symbol?", "symbol->string", "string->symbol",

    /* caracteres */
    "char?", "char=?", "char<?", "char>?", "char<=?", "char>=?",
    "char-ci=?", "char-ci<?", "char-ci>?", "char-ci<=?", "char-ci>=?",
    "char-alphabetic?", "char-numeric?", "char-whitespace?",
    "char-upper-case?", "char-lower-case?",
    "char->integer", "integer->char", "char-upcase", "char-downcase",

    /* strings */
    "string?", "make-string", "string", "string-length", "string-ref",
    "string-set!",
    "string=?", "string-ci=?", "string<?", "string>?", "string<=?", "string>=?",
    "string-ci<?", "string-ci>?", "string-ci<=?", "string-ci>=?",
    "substring", "string-append", "string->list", "list->string",
    "string-copy", "string-fill!",

    /* vetores */
    "vector?", "make-vector", "vector", "vector-length", "vector-ref",
    "vector-set!", "vector->list", "list->vector", "vector-fill!",

    /* controle */
    "procedure?", "apply", "map", "for-each", "force",
    "call-with-current-continuation", "values", "call-with-values",
    "dynamic-wind",

    /* eval */
    "eval", "scheme-report-environment", "null-environment",
    "interaction-environment",

    /* entrada e saida */
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
    size_t n = sizeof(PRIMITIVOS) / sizeof(PRIMITIVOS[0]);
    for (size_t i = 0; i < n; i++)
    {
        Symbol *s = symtab_insert(t, PRIMITIVOS[i]);
        if (s)
        {
            s->initialized = 1;  // primitivos ja' vem "inicializados"
            s->is_primitive = 1; // marca para nao serem impressos nos escopos
        }
    }
}

SymbolTable *symtab_create(void)
{
    SymbolTable *t = malloc(sizeof(SymbolTable));
    t->current = NULL;
    symtab_enter_scope(t); // escopo global
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
    Symbol *sym = s->symbols;
    // while (sym)
    // {
    //     Symbol *next = sym->next;
    //     free(sym->name);
    //     free(sym);
    //     sym = next;
    // }
    // free(s);
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
    Symbol *s = malloc(sizeof(Symbol));

    if (symtab_lookup_current(t, name) != NULL)
    {
        s->initialized = 1;
    }
    else
    {
        s->initialized = 0;
    }
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

void symtab_print(SymbolTable *t)
{
    printf("===== Tabela de simbolos =====\n");
    if (!t || !t->current)
    {
        printf("(vazia)\n");
        printf("==============================\n");
        return;
    }

    // descobre a profundidade do topo: global = nivel 0
    int depth = 0;
    for (Scope *sc = t->current; sc; sc = sc->parent)
        depth++;

    // imprime do escopo mais interno (topo da pilha) ate o global
    int level = depth - 1;
    for (Scope *sc = t->current; sc; sc = sc->parent, level--)
    {
        printf("-- Escopo %d%s --\n", level, level == 0 ? " (global)" : "");
        if (!sc->symbols)
        {
            printf("   (nenhum simbolo)\n");
        }
        for (Symbol *s = sc->symbols; s; s = s->next)
        {
            printf("   %-20s [%s]\n", s->name,
                   s->initialized ? "inicializado" : "nao inicializado");
        }
    }
    printf("==============================\n");
}

void symtab_destroy(SymbolTable *t)
{
    while (t->current)
        symtab_exit_scope(t);
    free(t);
}

// copia a lista ligada de símbolos de um escopo
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

// libera a lista ligada de símbolos de um escopo copiado
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