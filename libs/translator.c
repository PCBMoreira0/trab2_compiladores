#include "translator.h"
#include <stdlib.h>
#include <string.h>

typedef enum
{
    TR_RETURN,
    TR_ASSIGN
} TranslateMode;

void translate_as_return(ASTNode *node, FILE *file, FILE *file_defs, int identation);
void translate_as_block(ASTNode *node, FILE *file, FILE *file_defs,
                        int identation, TranslateMode mode, const char *var_name);

unsigned int get_counter()
{
    static unsigned int count = 0;
    return count++;
}

const char *builtin_name(const char *name)
{
    if (strcmp(name, "display") == 0)
        return "print";
    if (strcmp(name, "write") == 0)
        return "print";
    if (strcmp(name, "newline") == 0)
        return "print";
    if (strcmp(name, "expt") == 0)
        return "pow";
    if (strcmp(name, "abs") == 0)
        return "abs";
    if (strcmp(name, "min") == 0)
        return "min";
    if (strcmp(name, "max") == 0)
        return "max";
    if (strcmp(name, "length") == 0)
        return "len";
    return name;
}

void translator_print_indent(FILE *out, int identation)
{
    for (int i = 0; i < identation * 4; i++)
    {
        fputc(' ', out);
    }
}

void translate_as_return(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    translate_as_block(node, file, file_defs, identation, TR_RETURN, NULL);
}

void translate_as_block(ASTNode *node, FILE *file, FILE *file_defs,
                        int identation, TranslateMode mode, const char *var_name)
{
    if (node == NULL)
        return;

    switch (node->type)
    {
    case AST_IF:
        translator_print_indent(file, identation);
        fprintf(file, "if ");
        translate(node->if_expr.condition, file, file_defs, 0);
        fprintf(file, ":\n");
        translate_as_block(node->if_expr.then_branch, file, file_defs,
                           identation + 1, mode, var_name);
        if (node->if_expr.else_branch != NULL)
        {
            translator_print_indent(file, identation);
            fprintf(file, "else:\n");
            translate_as_block(node->if_expr.else_branch, file, file_defs,
                               identation + 1, mode, var_name);
        }
        break;

    case AST_BEGIN:
    {
        ASTNode *f = node->list.item;
        if (f == NULL)
            break;
        for (; f->list.next != NULL; f = f->list.next)
            translate(f->list.item, file, file_defs, identation);
        translate_as_block(f->list.item, file, file_defs, identation, mode, var_name);
        break;
    }

    default:
        translator_print_indent(file, identation);
        if (mode == TR_RETURN)
            fprintf(file, "return ");
        else
            fprintf(file, "%s = ", var_name);
        translate(node, file, file_defs, 0);
        fprintf(file, "\n");
        break;
    }
}

int needs_block(ASTNode *node)
{
    if (node == NULL)
        return 0;
    if (node->type == AST_BEGIN)
        return 1;
    if (node->type == AST_IF)
        return needs_block(node->if_expr.then_branch) ||
               needs_block(node->if_expr.else_branch);
    return 0;
}

void define_var(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    char *var_name = node->define_var.name->value_str;
    ASTNode *value = node->define_var.value;

    if (needs_block(value))
    {
        translate_as_block(value, file, file_defs, identation, TR_ASSIGN, var_name);
    }
    else
    {
        translator_print_indent(file, identation);
        fprintf(file, "%s = ", var_name);
        translate(value, file, file_defs, 0);
        fprintf(file, "\n");
    }
}

void define_func(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    char *func_name = node->func.name->value_str;
    fprintf(file, "def %s(", func_name);
    ASTNode *p = node->func.params;
    if (p != NULL)
    {
        for (; p->list.next != NULL; p = p->list.next)
        {
            translate(p->list.item, file, file_defs, identation);
            fprintf(file, ", ");
        }

        translate(p->list.item, file, file_defs, identation);
    }
    fprintf(file, "):\n");
    ASTNode *b = node->func.body;
    for (; b->list.next != NULL; b = b->list.next)
    {
        translate(b->list.item, file, file_defs, identation + 1);
    }
    translate_as_return(b->list.item, file, file_defs, identation + 1);
}

void let(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    if (node->let_expr.body->list.next != NULL)
    {
        fprintf(file, "NÃO SUPORTADO");
        return;
    }

    fprintf(file, "(lambda ");
    ASTNode *i;
    for (i = node->let_expr.bindings; i->list.next != NULL; i = i->list.next)
    {
        fprintf(file, "%s, ", i->list.item->binding.name->value_str);
    }
    fprintf(file, "%s: ", i->list.item->binding.name->value_str);

    translate(node->let_expr.body->list.item, file, file_defs, identation);

    fprintf(file, ")(");
    for (i = node->let_expr.bindings; i->list.next != NULL; i = i->list.next)
    {
        translate(i->list.item->binding.value, file, file_defs, identation);
        fprintf(file, ", ");
    }
    translate(i->list.item->binding.value, file, file_defs, identation);
    fprintf(file, ")");
}

void let_star_bindings(ASTNode *binding_list, ASTNode *body, FILE *file, FILE *file_defs, int identation)
{
    // Sem mais ligações: chegamos ao corpo do let*
    if (binding_list == NULL)
    {
        translate(body->list.item, file, file_defs, identation);
        return;
    }

    ASTNode *bind = binding_list->list.item;

    // (lambda nome: <restante das ligações + corpo>)(valor)
    fprintf(file, "(lambda %s: ", bind->binding.name->value_str);
    let_star_bindings(binding_list->list.next, body, file, file_defs, identation);
    fprintf(file, ")(");
    translate(bind->binding.value, file, file_defs, identation);
    fprintf(file, ")");
}

void let_star(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    if (node->let_expr.body->list.next != NULL)
    {
        fprintf(file, "NÃO SUPORTADO");
        return;
    }

    let_star_bindings(node->let_expr.bindings, node->let_expr.body, file, file_defs, identation);
}

void let_named(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    ASTNode f;
    f.type = AST_DEFINE_FUNC;
    char buffer[20];
    sprintf(buffer, "%s_%d", node->let_expr.name->value_str, get_counter());
    ASTNode name;
    name.value_str = buffer;
    f.func.name = &name;
    f.func.body = node->let_expr.body;
    ASTNode *params = NULL;
    for (ASTNode *b = node->let_expr.bindings; b != NULL; b = b->list.next)
        params = ast_list_append(params, b->list.item->binding.name);
    f.func.params = params;
    define_func(&f, file, file_defs, identation);

    fprintf(file, "%s(", f.func.name->value_str);
    for (ASTNode *b = node->let_expr.bindings; b != NULL; b = b->list.next)
    {
        translate(b->list.item->binding.value, file, file_defs, 0);
        if (b->list.next != NULL)
            fprintf(file, ", ");
    }
    fprintf(file, ")\n");
}

void lambda(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    if (node->func.body->list.next != NULL ||
        node->func.body->list.item->type == AST_BEGIN)
    {
        fprintf(file, "None  # lambda complexo nao suportado");
        return;
    }

    fprintf(file, "lambda ");
    ASTNode *p = node->func.params;
    if (p != NULL)
    {
        for (; p->list.next != NULL; p = p->list.next)
            fprintf(file, "%s, ", p->list.item->value_str);
        fprintf(file, "%s", p->list.item->value_str);
    }
    fprintf(file, ": ");
    translate(node->func.body->list.item, file, file_defs, 0);
}

char *get_op(char *value)
{
    if (strcmp(value, "+") == 0)
        return "+";
    if (strcmp(value, "-") == 0)
        return "-";
    if (strcmp(value, "*") == 0)
        return "*";
    if (strcmp(value, "/") == 0)
        return "/";
    if (strcmp(value, "<") == 0)
        return "<";
    if (strcmp(value, ">") == 0)
        return ">";
    if (strcmp(value, "<=") == 0)
        return "<=";
    if (strcmp(value, ">=") == 0)
        return ">=";
    if (strcmp(value, "=") == 0)
        return "==";
    return NULL;
}

int call_operator(ASTNode *node, FILE *file, int identation)
{
    char *op = get_op(node->call.operator_->value_str);

    ASTNode *i;
    for (i = node->call.operands; i->list.next != NULL; i = i->list.next)
    {
        translate(i->list.item, file, NULL, identation);
        fprintf(file, " %s ", op);
    }
    translate(i->list.item, file, NULL, identation);
}

int call_logic(ASTNode *node, FILE *file, int identation)
{
    ASTNode *i;
    for (i = node->logical.tests; i->list.next != NULL; i = i->list.next)
    {
        translate(i->list.item, file, NULL, identation);
        fprintf(file, " %s ", node->type == AST_AND ? "and" : "or");
    }
    translate(i->list.item, file, NULL, identation);
}

void call(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    translator_print_indent(file, identation);
    if (node->call.operator_->type == AST_VARIABLE &&
        get_op(node->call.operator_->value_str) != NULL)
    {
        call_operator(node, file, identation);
    }
    else
    {
        if (node->call.operator_->type == AST_LAMBDA)
        {
            fprintf(file, "(");
            translate(node->call.operator_, file, file_defs, 0);
            fprintf(file, ")");
        }
        else
        {
            translate(node->call.operator_, file, file_defs, 0);
        }
        fprintf(file, "(");

        if (node->call.operands == NULL)
        {
            fprintf(file, ")\n");
            return;
        }
        ASTNode *i;
        for (i = node->call.operands; i->list.next != NULL; i = i->list.next)
        {
            translate(i->list.item, file, file_defs, 0);
            fprintf(file, ", ");
        }
        translate(i->list.item, file, file_defs, 0);
        fprintf(file, ")\n");
    }
}

void if_statement(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    translate(node->if_expr.then_branch, file, file_defs, identation);
    fprintf(file, " if ");
    translate(node->if_expr.condition, file, file_defs, identation);
    fprintf(file, " else ");
    if (node->if_expr.else_branch != NULL)
    {
        translate(node->if_expr.else_branch, file, file_defs, identation);
    }
    else
    {
        fprintf(file, "None");
    }
}

void variable(ASTNode *node, FILE *file, int identation)
{
    const char *name = builtin_name(node->value_str);

    if (strcmp(name, node->value_str) == 0)
    {
        fprintf(file, "%s", node->value_str);
    }
    else
    {
        fprintf(file, "%s", name);
    }
}

void translate(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    if (node == NULL)
        return;
    switch (node->type)
    {
    case AST_LIST:
        translate(node->list.item, file, file_defs, identation);
        translate(node->list.next, file, file_defs, identation);
        break;
    case AST_DEFINE_VAR:
        define_var(node, file, file_defs, identation);
        break;
    case AST_SET:
        translator_print_indent(file, identation);
        fprintf(file, "%s = ", node->set_expr.name->value_str);
        translate(node->set_expr.value, file, file_defs, identation);
        fprintf(file, "\n");
        break;
    case AST_VARIABLE:
        variable(node, file, identation);
        break;
    case AST_NUMBER:
        fprintf(file, "%s", node->value_str);
        break;
    case AST_DEFINE_FUNC:
        define_func(node, file, file_defs, identation);
        break;
    case AST_LET:
        let(node, file, file_defs, identation);
        break;
    case AST_LET_STAR:
        let_star(node, file, file_defs, identation);
        break;
    case AST_NAMED_LET:
        let_named(node, file, file_defs, identation);
        break;
    case AST_CALL:
        call(node, file, file_defs, identation);
        break;
    case AST_BINDING:
        translate(node->binding.name, file, file_defs, identation);
        translate(node->binding.value, file, file_defs, identation);
        break;
    case AST_IF:
        if_statement(node, file, file_defs, identation);
        break;
    case AST_STRING:
        fprintf(file, "%s", node->value_str);
        break;
    case AST_BOOLEAN:
        fprintf(file, "%s", (strcmp(node->value_str, "#t") == 0) ? "True" : "False");
        break;
    case AST_BEGIN:
    {
        ASTNode *f = node->list.item;
        while (f != NULL)
        {
            translate(f->list.item, file, file_defs, identation);
            f = f->list.next;
        }
        break;
    }
    case AST_AND:
    case AST_OR:
        call_logic(node, file, identation);
        break;
    case AST_LAMBDA:
        lambda(node, file, NULL, identation);
        break;
    default:
        break;
    }
}