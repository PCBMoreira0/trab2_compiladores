#pragma once

/*
 * AST da linguagem Scheme.
 *
 * Segue o mesmo modelo do ast.h/ast.c: um no' unico (ASTNode) com um campo
 * "type" que identifica a variante e uma union com os dados de cada variante.
 * Cada construcao da gramatica (scheme.y) tem um construtor ast_create_*.
 *
 * Como Scheme e' uma linguagem baseada em listas, varias producoes da
 * gramatica sao listas (corpo, operandos, ligacoes de let, clausulas de cond,
 * etc). Todas elas sao representadas pelo no' generico AST_LIST, que e' uma
 * lista ligada de (item, next) - exatamente como o AST_BLOCK do modelo base.
 */

typedef enum
{
    /* lista ligada generica (program, body, operandos, bindings, ...) */
    AST_LIST,

    /* definicoes */
    AST_DEFINE_VAR,  /* (define var expr)                 */
    AST_DEFINE_FUNC, /* (define (nome args ...) body)     */

    /* expressoes */
    AST_LAMBDA,      /* (lambda formals body)             */
    AST_IF,          /* (if teste entao senao)            */
    AST_SET,         /* (set! var expr)                   */
    AST_CALL,        /* (operador operandos ...)          */
    AST_QUOTE,       /* 'datum  /  (quote datum)          */

    /* expressoes derivadas */
    AST_COND,        /* (cond clausulas ...)              */
    AST_COND_CLAUSE, /* (teste ...) / (teste => recipiente) */
    AST_CASE,        /* (case chave clausulas ...)        */
    AST_CASE_CLAUSE, /* ((dados ...) sequencia)           */
    AST_AND,         /* (and testes ...)                  */
    AST_OR,          /* (or testes ...)                   */
    AST_LET,         /* (let ((v e) ...) body)            */
    AST_LET_STAR,    /* (let* ((v e) ...) body)           */
    AST_LETREC,      /* (letrec ((v e) ...) body)         */
    AST_NAMED_LET,   /* (let nome ((v e) ...) body)       */
    AST_BINDING,     /* (var expr)  -- ligacao de um let  */
    AST_BEGIN,       /* (begin forms ...)                 */
    AST_DO,          /* (do (specs) (teste result) cmds)  */
    AST_ITER_SPEC,   /* (var init step) -- spec do do     */
    AST_DELAY,       /* (delay expr)                      */
    AST_VECTOR,      /* #(datum ...)                      */

    /* folhas */
    AST_VARIABLE,    /* identificador                     */
    AST_NUMBER,      /* literal numerico                  */
    AST_BOOLEAN,     /* #t / #f                           */
    AST_STRING,      /* "..."                             */
    AST_CHARACTER    /* #\c                               */
} ASTNodeType;

typedef struct ASTNode
{
    ASTNodeType type;
    union
    {
        /* lista ligada generica: item atual + resto da lista */
        struct
        {
            struct ASTNode *item;
            struct ASTNode *next;
        } list;

        /* (define var expr) */
        struct
        {
            struct ASTNode *name; /* no' AST_VARIABLE */
            struct ASTNode *value;
        } define_var;

        /* (define (nome args ...) body) e (lambda formals body) */
        struct
        {
            struct ASTNode *name;       /* AST_VARIABLE; NULL no caso de lambda */
            struct ASTNode *params;     /* lista de AST_VARIABLE             */
            struct ASTNode *rest_param; /* AST_VARIABLE variadico (. resto) ou NULL */
            struct ASTNode *body;       /* lista de comandos/definicoes      */
        } func;

        /* (if teste entao senao) */
        struct
        {
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch; /* pode ser NULL */
        } if_expr;

        /* (set! var valor) */
        struct
        {
            struct ASTNode *name; /* no' AST_VARIABLE */
            struct ASTNode *value;
        } set_expr;

        /* (operador operandos ...) */
        struct
        {
            struct ASTNode *operator_; /* expressao do operador  */
            struct ASTNode *operands;  /* lista de operandos     */
        } call;

        /* 'datum  /  (quote datum) */
        struct
        {
            struct ASTNode *datum;
        } quote;

        /* (cond clausulas ... [else]) */
        struct
        {
            struct ASTNode *clauses;     /* lista de AST_COND_CLAUSE */
            struct ASTNode *else_clause; /* sequencia do else ou NULL */
        } cond;

        /* clausula de cond: (teste body) / (teste) / (teste => recipiente) */
        struct
        {
            struct ASTNode *test;
            struct ASTNode *body;      /* sequencia ou NULL              */
            struct ASTNode *recipient; /* alvo do => ou NULL             */
        } cond_clause;

        /* (case chave clausulas ... [else]) */
        struct
        {
            struct ASTNode *key;
            struct ASTNode *clauses;     /* lista de AST_CASE_CLAUSE  */
            struct ASTNode *else_clause; /* sequencia do else ou NULL */
        } case_expr;

        /* clausula de case: ((dados ...) sequencia) */
        struct
        {
            struct ASTNode *data; /* lista de datums */
            struct ASTNode *body; /* sequencia       */
        } case_clause;

        /* (and testes ...) / (or testes ...) */
        struct
        {
            struct ASTNode *tests; /* lista de testes */
        } logical;

        /* let / let* / letrec / named let */
        struct
        {
            struct ASTNode *name;     /* AST_VARIABLE do named let, senao NULL */
            struct ASTNode *bindings; /* lista de AST_BINDING          */
            struct ASTNode *body;     /* lista de comandos/definicoes  */
        } let_expr;

        /* ligacao de let: (var expr) */
        struct
        {
            struct ASTNode *name; /* no' AST_VARIABLE */
            struct ASTNode *value;
        } binding;

        /* (do (specs) (teste result) comandos) */
        struct
        {
            struct ASTNode *specs;    /* lista de AST_ITER_SPEC        */
            struct ASTNode *test;     /* teste de parada               */
            struct ASTNode *result;   /* sequencia de resultado        */
            struct ASTNode *commands; /* lista de comandos do corpo    */
        } do_expr;

        /* spec de iteracao do do: (var init step) */
        struct
        {
            struct ASTNode *name; /* no' AST_VARIABLE */
            struct ASTNode *init;
            struct ASTNode *step; /* pode ser NULL */
        } iter_spec;

        /* (delay expr) */
        struct
        {
            struct ASTNode *expression;
        } delay;

        /* #(datum ...) */
        struct
        {
            struct ASTNode *elements; /* lista de datums */
        } vector;

        /* folhas: variavel, numero, booleano, string, caractere */
        char *value_str;
    };
} ASTNode;

/* ----- listas genericas ----- */
ASTNode *ast_create_list(ASTNode *item, ASTNode *next);
ASTNode *ast_list_append(ASTNode *list, ASTNode *item);

/* ----- definicoes ----- */
ASTNode *ast_create_define_var(ASTNode *name, ASTNode *value);
ASTNode *ast_create_define_func(ASTNode *name, ASTNode *params, ASTNode *rest_param, ASTNode *body);

/* ----- expressoes ----- */
ASTNode *ast_create_lambda(ASTNode *params, ASTNode *rest_param, ASTNode *body);
ASTNode *ast_create_if(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);
ASTNode *ast_create_set(ASTNode *name, ASTNode *value);
ASTNode *ast_create_call(ASTNode *operator_, ASTNode *operands);
ASTNode *ast_create_quote(ASTNode *datum);

/* ----- expressoes derivadas ----- */
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

/* ----- folhas ----- */
ASTNode *ast_create_variable(char *name);
ASTNode *ast_create_number(char *value);
ASTNode *ast_create_boolean(char *value);
ASTNode *ast_create_string(char *value);
ASTNode *ast_create_character(char *value);

/* ----- impressao ----- */
void ast_print(ASTNode *node, int level);
