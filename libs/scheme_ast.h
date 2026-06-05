#pragma once

#include "symbol_tab.h"

typedef enum
{

    AST_LIST,

    AST_DEFINE_VAR,
    AST_DEFINE_FUNC,

    AST_LAMBDA,
    AST_IF,
    AST_SET,
    AST_CALL,
    AST_QUOTE,

    AST_COND,
    AST_COND_CLAUSE,
    AST_CASE,
    AST_CASE_CLAUSE,
    AST_AND,
    AST_OR,
    AST_LET,
    AST_LET_STAR,
    AST_LETREC,
    AST_NAMED_LET,
    AST_BINDING,
    AST_BEGIN,
    AST_DO,
    AST_ITER_SPEC,
    AST_DELAY,
    AST_VECTOR,

    AST_VARIABLE,
    AST_NUMBER,
    AST_BOOLEAN,
    AST_STRING,
    AST_CHARACTER
} ASTNodeType;

typedef struct ASTNode
{
    ASTNodeType type;
    int linha;
    Scope *scope;
    union
    {

        struct
        {
            struct ASTNode *item;
            struct ASTNode *next;
        } list;

        struct
        {
            struct ASTNode *name;
            struct ASTNode *value;
        } define_var;

        struct
        {
            struct ASTNode *name;
            struct ASTNode *params;
            struct ASTNode *rest_param;
            struct ASTNode *body;
        } func;

        struct
        {
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch;
        } if_expr;

        struct
        {
            struct ASTNode *name;
            struct ASTNode *value;
        } set_expr;

        struct
        {
            struct ASTNode *operator_;
            struct ASTNode *operands;
        } call;

        struct
        {
            struct ASTNode *datum;
        } quote;

        struct
        {
            struct ASTNode *clauses;
            struct ASTNode *else_clause;
        } cond;

        struct
        {
            struct ASTNode *test;
            struct ASTNode *body;
            struct ASTNode *recipient;
        } cond_clause;

        struct
        {
            struct ASTNode *key;
            struct ASTNode *clauses;
            struct ASTNode *else_clause;
        } case_expr;

        struct
        {
            struct ASTNode *data;
            struct ASTNode *body;
        } case_clause;

        struct
        {
            struct ASTNode *tests;
        } logical;

        struct
        {
            struct ASTNode *name;
            struct ASTNode *bindings;
            struct ASTNode *body;
        } let_expr;

        struct
        {
            struct ASTNode *name;
            struct ASTNode *value;
        } binding;

        struct
        {
            struct ASTNode *specs;
            struct ASTNode *test;
            struct ASTNode *result;
            struct ASTNode *commands;
        } do_expr;

        struct
        {
            struct ASTNode *name;
            struct ASTNode *init;
            struct ASTNode *step;
        } iter_spec;

        struct
        {
            struct ASTNode *expression;
        } delay;

        struct
        {
            struct ASTNode *elements;
        } vector;

        char *value_str;
    };

    
} ASTNode;

ASTNode *ast_create_list(ASTNode *item, ASTNode *next);
ASTNode *ast_list_append(ASTNode *list, ASTNode *item);

ASTNode *ast_create_define_var(ASTNode *name, ASTNode *value);
ASTNode *ast_create_define_func(ASTNode *name, ASTNode *params, ASTNode *rest_param, ASTNode *body);

ASTNode *ast_create_lambda(ASTNode *params, ASTNode *rest_param, ASTNode *body);
ASTNode *ast_create_if(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);
ASTNode *ast_create_set(ASTNode *name, ASTNode *value);
ASTNode *ast_create_call(ASTNode *operator_, ASTNode *operands);
ASTNode *ast_create_quote(ASTNode *datum);

ASTNode *ast_create_cond(ASTNode *clauses, ASTNode *else_clause);
ASTNode *ast_create_cond_clause(ASTNode *test, ASTNode *body, ASTNode *recipient);
ASTNode *ast_create_case(ASTNode *key, ASTNode *clauses, ASTNode *else_clause);
ASTNode *ast_create_case_clause(ASTNode *data, ASTNode *body);
ASTNode *ast_create_and(ASTNode *tests);
ASTNode *ast_create_or(ASTNode *tests);
ASTNode *ast_create_let(ASTNode *bindings, ASTNode *body);
ASTNode *ast_create_let_star(ASTNode *bindings, ASTNode *body);
ASTNode *ast_create_letrec(ASTNode *bindings, ASTNode *body);
ASTNode *ast_create_named_let(ASTNode *name, ASTNode *bindings, ASTNode *body);
ASTNode *ast_create_binding(ASTNode *name, ASTNode *value);
ASTNode *ast_create_begin(ASTNode *forms);
ASTNode *ast_create_do(ASTNode *specs, ASTNode *test, ASTNode *result, ASTNode *commands);
ASTNode *ast_create_iter_spec(ASTNode *name, ASTNode *init, ASTNode *step);
ASTNode *ast_create_delay(ASTNode *expression);
ASTNode *ast_create_vector(ASTNode *elements);

ASTNode *ast_create_variable(char *name);
ASTNode *ast_create_number(char *value);
ASTNode *ast_create_boolean(char *value);
ASTNode *ast_create_string(char *value);
ASTNode *ast_create_character(char *value);

void ast_print(ASTNode *node, int level);

void ast_type_to_string(ASTNodeType type, char *buffer);

void ast_print_scope(ASTNode *node);

void ast_print_scopes(ASTNode *node);

void ast_dfs(ASTNode *node, SymbolTable *table);
void ast_dfs_second(ASTNode *node, SymbolTable *table);

void ast_free(ASTNode *node);
