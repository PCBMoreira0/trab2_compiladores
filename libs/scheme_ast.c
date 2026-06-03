#include "scheme_ast.h"
#include "symbol_tab.h"
#include <stdlib.h>
#include <stdio.h>

/* helper interno: aloca um no' ja' com o tipo preenchido */
static ASTNode *ast_new(ASTNodeType type)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;
    return node;
}

/* ----- listas genericas ----- */

ASTNode *ast_create_list(ASTNode *item, ASTNode *next)
{
    ASTNode *node = ast_new(AST_LIST);
    node->list.item = item;
    node->list.next = next;
    return node;
}

/* adiciona um item ao fim da lista (util nas regras left-recursive da
 * gramatica, que constroem listas da esquerda para a direita) */
ASTNode *ast_list_append(ASTNode *list, ASTNode *item)
{
    ASTNode *novo = ast_create_list(item, NULL);

    if (list == NULL)
        return novo;

    ASTNode *atual = list;
    while (atual->list.next != NULL)
        atual = atual->list.next;
    atual->list.next = novo;

    return list;
}

/* ----- definicoes ----- */

ASTNode *ast_create_define_var(ASTNode *name, ASTNode *value)
{
    ASTNode *node = ast_new(AST_DEFINE_VAR);
    node->define_var.name = name;
    node->define_var.value = value;
    return node;
}

ASTNode *ast_create_define_func(ASTNode *name, ASTNode *params, ASTNode *rest_param, ASTNode *body)
{
    ASTNode *node = ast_new(AST_DEFINE_FUNC);
    node->func.name = name;
    node->func.params = params;
    node->func.rest_param = rest_param;
    node->func.body = body;
    return node;
}

/* ----- expressoes ----- */

ASTNode *ast_create_lambda(ASTNode *params, ASTNode *rest_param, ASTNode *body)
{
    ASTNode *node = ast_new(AST_LAMBDA);
    node->func.name = NULL;
    node->func.params = params;
    node->func.rest_param = rest_param;
    node->func.body = body;
    return node;
}

ASTNode *ast_create_if(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch)
{
    ASTNode *node = ast_new(AST_IF);
    node->if_expr.condition = condition;
    node->if_expr.then_branch = then_branch;
    node->if_expr.else_branch = else_branch;
    return node;
}

ASTNode *ast_create_set(ASTNode *name, ASTNode *value)
{
    ASTNode *node = ast_new(AST_SET);
    node->set_expr.name = name;
    node->set_expr.value = value;
    return node;
}

ASTNode *ast_create_call(ASTNode *operator_, ASTNode *operands)
{
    ASTNode *node = ast_new(AST_CALL);
    node->call.operator_ = operator_;
    node->call.operands = operands;
    return node;
}

ASTNode *ast_create_quote(ASTNode *datum)
{
    ASTNode *node = ast_new(AST_QUOTE);
    node->quote.datum = datum;
    return node;
}

/* ----- expressoes derivadas ----- */

ASTNode *ast_create_cond(ASTNode *clauses, ASTNode *else_clause)
{
    ASTNode *node = ast_new(AST_COND);
    node->cond.clauses = clauses;
    node->cond.else_clause = else_clause;
    return node;
}

ASTNode *ast_create_cond_clause(ASTNode *test, ASTNode *body, ASTNode *recipient)
{
    ASTNode *node = ast_new(AST_COND_CLAUSE);
    node->cond_clause.test = test;
    node->cond_clause.body = body;
    node->cond_clause.recipient = recipient;
    return node;
}

ASTNode *ast_create_case(ASTNode *key, ASTNode *clauses, ASTNode *else_clause)
{
    ASTNode *node = ast_new(AST_CASE);
    node->case_expr.key = key;
    node->case_expr.clauses = clauses;
    node->case_expr.else_clause = else_clause;
    return node;
}

ASTNode *ast_create_case_clause(ASTNode *data, ASTNode *body)
{
    ASTNode *node = ast_new(AST_CASE_CLAUSE);
    node->case_clause.data = data;
    node->case_clause.body = body;
    return node;
}

ASTNode *ast_create_and(ASTNode *tests)
{
    ASTNode *node = ast_new(AST_AND);
    node->logical.tests = tests;
    return node;
}

ASTNode *ast_create_or(ASTNode *tests)
{
    ASTNode *node = ast_new(AST_OR);
    node->logical.tests = tests;
    return node;
}

/* helper interno para os quatro tipos de let */
static ASTNode *ast_make_let(ASTNodeType type, ASTNode *name, ASTNode *bindings, ASTNode *body)
{
    ASTNode *node = ast_new(type);
    node->let_expr.name = name;
    node->let_expr.bindings = bindings;
    node->let_expr.body = body;
    return node;
}

ASTNode *ast_create_let(ASTNode *bindings, ASTNode *body)
{
    return ast_make_let(AST_LET, NULL, bindings, body);
}

ASTNode *ast_create_let_star(ASTNode *bindings, ASTNode *body)
{
    return ast_make_let(AST_LET_STAR, NULL, bindings, body);
}

ASTNode *ast_create_letrec(ASTNode *bindings, ASTNode *body)
{
    return ast_make_let(AST_LETREC, NULL, bindings, body);
}

ASTNode *ast_create_named_let(ASTNode *name, ASTNode *bindings, ASTNode *body)
{
    return ast_make_let(AST_NAMED_LET, name, bindings, body);
}

ASTNode *ast_create_binding(ASTNode *name, ASTNode *value)
{
    ASTNode *node = ast_new(AST_BINDING);
    node->binding.name = name;
    node->binding.value = value;
    return node;
}

ASTNode *ast_create_begin(ASTNode *forms)
{
    ASTNode *node = ast_new(AST_BEGIN);
    node->list.item = forms;
    node->list.next = NULL;
    return node;
}

ASTNode *ast_create_do(ASTNode *specs, ASTNode *test, ASTNode *result, ASTNode *commands)
{
    ASTNode *node = ast_new(AST_DO);
    node->do_expr.specs = specs;
    node->do_expr.test = test;
    node->do_expr.result = result;
    node->do_expr.commands = commands;
    return node;
}

ASTNode *ast_create_iter_spec(ASTNode *name, ASTNode *init, ASTNode *step)
{
    ASTNode *node = ast_new(AST_ITER_SPEC);
    node->iter_spec.name = name;
    node->iter_spec.init = init;
    node->iter_spec.step = step;
    return node;
}

ASTNode *ast_create_delay(ASTNode *expression)
{
    ASTNode *node = ast_new(AST_DELAY);
    node->delay.expression = expression;
    return node;
}

ASTNode *ast_create_vector(ASTNode *elements)
{
    ASTNode *node = ast_new(AST_VECTOR);
    node->vector.elements = elements;
    return node;
}

/* ----- folhas ----- */

ASTNode *ast_create_variable(char *name)
{
    ASTNode *node = ast_new(AST_VARIABLE);
    node->value_str = name;
    return node;
}

ASTNode *ast_create_number(char *value)
{
    ASTNode *node = ast_new(AST_NUMBER);
    node->value_str = value;
    return node;
}

ASTNode *ast_create_boolean(char *value)
{
    ASTNode *node = ast_new(AST_BOOLEAN);
    node->value_str = value;
    return node;
}

ASTNode *ast_create_string(char *value)
{
    ASTNode *node = ast_new(AST_STRING);
    node->value_str = value;
    return node;
}

ASTNode *ast_create_character(char *value)
{
    ASTNode *node = ast_new(AST_CHARACTER);
    node->value_str = value;
    return node;
}

/* ----- impressao ----- */

static void ast_print_indent(int level)
{
    for (int i = 0; i < level; i++)
        printf("  | ");
}

/* imprime um rotulo indentado seguido de uma sublista, util para os varios
 * campos que sao listas (operandos, corpo, ligacoes, ...) */
static void ast_print_field(const char *label, ASTNode *node, int level)
{
    ast_print_indent(level);
    printf("%s\n", label);
    ast_print(node, level + 1);
}

void ast_print(ASTNode *node, int level)
{
    if (node == NULL)
        return;

    ast_print_indent(level);

    switch (node->type)
    {
    case AST_LIST:
        /* a lista nao imprime um cabecalho proprio: apenas encadeia os itens
         * no mesmo nivel (igual ao AST_BLOCK do modelo base) */
        printf("Item da lista:\n");
        ast_print(node->list.item, level + 1);
        ast_print(node->list.next, level);
        break;

    case AST_DEFINE_VAR:
        printf("Definicao de variavel: %s\n", node->define_var.name->value_str);
        ast_print(node->define_var.value, level + 1);
        break;

    case AST_DEFINE_FUNC:
        printf("Definicao de funcao: %s\n", node->func.name->value_str);
        ast_print_field("Parametros:", node->func.params, level + 1);
        if (node->func.rest_param != NULL)
        {
            ast_print_indent(level + 1);
            printf("Parametro variadico: %s\n", node->func.rest_param->value_str);
        }
        ast_print_field("Corpo:", node->func.body, level + 1);
        break;

    case AST_LAMBDA:
        printf("Lambda\n");
        ast_print_field("Parametros:", node->func.params, level + 1);
        if (node->func.rest_param != NULL)
        {
            ast_print_indent(level + 1);
            printf("Parametro variadico: %s\n", node->func.rest_param->value_str);
        }
        ast_print_field("Corpo:", node->func.body, level + 1);
        break;

    case AST_IF:
        printf("Comando SE (If)\n");
        ast_print_field("Condicao:", node->if_expr.condition, level + 1);
        ast_print_field("Entao:", node->if_expr.then_branch, level + 1);
        if (node->if_expr.else_branch != NULL)
            ast_print_field("Senao:", node->if_expr.else_branch, level + 1);
        break;

    case AST_SET:
        printf("Atribuicao (set!): %s\n", node->set_expr.name->value_str);
        ast_print(node->set_expr.value, level + 1);
        break;

    case AST_CALL:
        printf("Chamada de procedimento\n");
        ast_print_field("Operador:", node->call.operator_, level + 1);
        ast_print_field("Operandos:", node->call.operands, level + 1);
        break;

    case AST_QUOTE:
        printf("Quote\n");
        ast_print(node->quote.datum, level + 1);
        break;

    case AST_COND:
        printf("Cond\n");
        ast_print(node->cond.clauses, level + 1);
        if (node->cond.else_clause != NULL)
            ast_print_field("Else:", node->cond.else_clause, level + 1);
        break;

    case AST_COND_CLAUSE:
        printf("Clausula de cond\n");
        ast_print_field("Teste:", node->cond_clause.test, level + 1);
        if (node->cond_clause.recipient != NULL)
            ast_print_field("Recipiente (=>):", node->cond_clause.recipient, level + 1);
        if (node->cond_clause.body != NULL)
            ast_print_field("Corpo:", node->cond_clause.body, level + 1);
        break;

    case AST_CASE:
        printf("Case\n");
        ast_print_field("Chave:", node->case_expr.key, level + 1);
        ast_print(node->case_expr.clauses, level + 1);
        if (node->case_expr.else_clause != NULL)
            ast_print_field("Else:", node->case_expr.else_clause, level + 1);
        break;

    case AST_CASE_CLAUSE:
        printf("Clausula de case\n");
        ast_print_field("Dados:", node->case_clause.data, level + 1);
        ast_print_field("Corpo:", node->case_clause.body, level + 1);
        break;

    case AST_AND:
        printf("And\n");
        ast_print(node->logical.tests, level + 1);
        break;

    case AST_OR:
        printf("Or\n");
        ast_print(node->logical.tests, level + 1);
        break;

    case AST_LET:
        printf("Let\n");
        ast_print_field("Ligacoes:", node->let_expr.bindings, level + 1);
        ast_print_field("Corpo:", node->let_expr.body, level + 1);
        break;

    case AST_LET_STAR:
        printf("Let*\n");
        ast_print_field("Ligacoes:", node->let_expr.bindings, level + 1);
        ast_print_field("Corpo:", node->let_expr.body, level + 1);
        break;

    case AST_LETREC:
        printf("Letrec\n");
        ast_print_field("Ligacoes:", node->let_expr.bindings, level + 1);
        ast_print_field("Corpo:", node->let_expr.body, level + 1);
        break;

    case AST_NAMED_LET:
        printf("Named let: %s\n", node->let_expr.name->value_str);
        ast_print_field("Ligacoes:", node->let_expr.bindings, level + 1);
        ast_print_field("Corpo:", node->let_expr.body, level + 1);
        break;

    case AST_BINDING:
        printf("Ligacao: %s\n", node->binding.name->value_str);
        ast_print(node->binding.value, level + 1);
        break;

    case AST_BEGIN:
        printf("Begin\n");
        ast_print(node->list.item, level + 1);
        break;

    case AST_DO:
        printf("Do\n");
        ast_print_field("Specs:", node->do_expr.specs, level + 1);
        ast_print_field("Teste:", node->do_expr.test, level + 1);
        ast_print_field("Resultado:", node->do_expr.result, level + 1);
        ast_print_field("Comandos:", node->do_expr.commands, level + 1);
        break;

    case AST_ITER_SPEC:
        printf("Spec de iteracao: %s\n", node->iter_spec.name->value_str);
        ast_print_field("Init:", node->iter_spec.init, level + 1);
        if (node->iter_spec.step != NULL)
            ast_print_field("Step:", node->iter_spec.step, level + 1);
        break;

    case AST_DELAY:
        printf("Delay\n");
        ast_print(node->delay.expression, level + 1);
        break;

    case AST_VECTOR:
        printf("Vetor\n");
        ast_print(node->vector.elements, level + 1);
        break;

    case AST_VARIABLE:
        printf("Variavel: %s\n", node->value_str);
        break;

    case AST_NUMBER:
        printf("Numero: %s\n", node->value_str);
        break;

    case AST_BOOLEAN:
        printf("Booleano: %s\n", node->value_str);
        break;

    case AST_STRING:
        printf("String: %s\n", node->value_str);
        break;

    case AST_CHARACTER:
        printf("Caractere: %s\n", node->value_str);
        break;

    default:
        printf("No desconhecido!\n");
        break;
    }
}

void ast_dfs(ASTNode *node, SymbolTable *table)
{
    if (node == NULL)
        return;

    switch (node->type)
    {
    case AST_LIST:
        ast_dfs(node->list.item, table);
        ast_dfs(node->list.next, table);
        break;

    case AST_DEFINE_VAR:
        ast_dfs(node->define_var.value, table);
        break;

    case AST_DEFINE_FUNC:
        ast_dfs(node->func.params, table);
        ast_dfs(node->func.body, table);
        break;

    case AST_LAMBDA:
        symtab_enter_scope(table);
        for (ASTNode *p = node->func.params; p != NULL; p = p->list.next)
        {
            ASTNode *var = p->list.item;
            if (symtab_insert(table, var->value_str) == NULL)
            {
                fprintf(stderr,
                        "Erro semantico: parametro '%s' duplicado no lambda\n",
                        var->value_str);
            }
        }
        if (node->func.rest_param != NULL)
        {
            symtab_insert(table, node->func.rest_param->value_str);
        }

        ast_dfs(node->func.body, table);

        symtab_exit_scope(table);
        break;

    case AST_IF:
        ast_dfs(node->if_expr.condition, table);
        ast_dfs(node->if_expr.then_branch, table);
        ast_dfs(node->if_expr.else_branch, table);
        break;

    case AST_SET:
        ast_dfs(node->set_expr.value, table);
        break;

    case AST_CALL:
        ast_dfs(node->call.operator_, table);
        ast_dfs(node->call.operands, table);
        break;

    case AST_QUOTE:
        ast_dfs(node->quote.datum, table);
        break;

    case AST_COND:
        ast_dfs(node->cond.clauses, table);
        ast_dfs(node->cond.else_clause, table);
        break;

    case AST_COND_CLAUSE:
        ast_dfs(node->cond_clause.test, table);
        ast_dfs(node->cond_clause.recipient, table);
        ast_dfs(node->cond_clause.body, table);
        break;

    case AST_CASE:
        ast_dfs(node->case_expr.key, table);
        ast_dfs(node->case_expr.clauses, table);
        ast_dfs(node->case_expr.else_clause, table);
        break;

    case AST_CASE_CLAUSE:
        ast_dfs(node->case_clause.data, table);
        ast_dfs(node->case_clause.body, table);
        break;

    case AST_AND:
        ast_dfs(node->logical.tests, table);
        break;

    case AST_OR:
        ast_dfs(node->logical.tests, table);
        break;

    case AST_LET:
        ast_dfs(node->let_expr.bindings, table);
        ast_dfs(node->let_expr.body, table);
        break;

    case AST_LET_STAR:
        ast_dfs(node->let_expr.bindings, table);
        ast_dfs(node->let_expr.body, table);
        break;

    case AST_LETREC:
        ast_dfs(node->let_expr.bindings, table);
        ast_dfs(node->let_expr.body, table);
        break;

    case AST_NAMED_LET:
        ast_dfs(node->let_expr.bindings, table);
        ast_dfs(node->let_expr.body, table);
        break;

    case AST_BINDING:
        ast_dfs(node->binding.value, table);
        break;

    case AST_BEGIN:
        ast_dfs(node->list.item, table);
        break;

    case AST_DO:
        ast_dfs(node->do_expr.specs, table);
        ast_dfs(node->do_expr.test, table);
        ast_dfs(node->do_expr.result, table);
        ast_dfs(node->do_expr.commands, table);
        break;

    case AST_ITER_SPEC:
        ast_dfs(node->iter_spec.init, table);
        ast_dfs(node->iter_spec.step, table);
        break;

    case AST_DELAY:
        ast_dfs(node->delay.expression, table);
        break;

    case AST_VECTOR:
        ast_dfs(node->vector.elements, table);
        break;

    default:
        break;
    }
}
