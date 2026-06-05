#include "translator.h"
#include <stdlib.h>
#include <string.h>

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

void assign(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    char *var_name = node->define_var.name->value_str;

    translator_print_indent(file, identation);
    fprintf(file, "%s", var_name);
    fprintf(file, " = ");

    translate(node->define_var.value, file, file_defs, 0);

    fprintf(file, "\n");
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
    translator_print_indent(file, identation + 1);
    fprintf(file, "return ");
    translate(b->list.item, file, file_defs, identation);
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
    ASTNode *name;
    name->value_str = buffer;
    f.func.name = name;
    f.func.body = node->let_expr.body;
    f.func.params = node->let_expr.bindings;
    define_func(&f, file, file_defs, identation);
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
        fprintf(file, "%s %s ", i->list.item->value_str, op);
    }
    fprintf(file, "%s", i->list.item->value_str);
}

void call(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    translator_print_indent(file, identation);
    if (get_op(node->call.operator_->value_str) != NULL)
    {
        call_operator(node, file, identation);
    }
    else
    {
        translate(node->call.operator_, file, file_defs, identation);
        fprintf(file, "(");

        if (node->call.operands == NULL)
        {
            fprintf(file, ")\n");
            return;
        }
        ASTNode *i;
        for (i = node->call.operands; i->list.next != NULL; i = i->list.next)
        {
            fprintf(file, "%s, ", i->list.item->value_str);
        }
        fprintf(file, "%s)\n", i->list.item->value_str);
    }
}

void if_ternary(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    translate(node->if_expr.then_branch, file, file_defs, identation);
    fprintf(file, " if ");
    translate(node->if_expr.condition, file, file_defs, identation);
    fprintf(file, " else ");
    translate(node->if_expr.else_branch, file, file_defs, identation);
}

void if_block(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    translator_print_indent(file, identation + 1);
    fprintf(file, "if ");
    translate(node->if_expr.condition, file, file_defs, identation);
    fprintf(file, ":\n");
    ASTNode *i;
    if (node->if_expr.then_branch->type == AST_LIST)
    {
        for (i = node->if_expr.then_branch->list.item; i->list.next != NULL; i = i->list.next)
        {
            translate(i->list.item, file, file_defs, identation + 2);
        }

        translator_print_indent(file, identation + 2);
        fprintf(file, "return ");
        translate(i->list.item, file, file_defs, identation);
    }
    else
    {
        translator_print_indent(file, identation + 2);
        fprintf(file, "return ");
        translate(node->if_expr.then_branch, file, file_defs, identation + 2);
    }

    fprintf(file, "\n");

    if (node->if_expr.else_branch != NULL)
    {
        translator_print_indent(file, identation + 1);
        fprintf(file, "else:\n");
        for (i = node->if_expr.else_branch->list.item; i->list.next != NULL; i = i->list.next)
        {
            translate(i->list.item, file, file_defs, identation + 2);
        }
        translator_print_indent(file, identation + 2);
        fprintf(file, "return ");
        translate(i->list.item, file, file_defs, identation);
        fprintf(file, "\n");
    }
    fprintf(file, "\n");
}

void if_statement(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    translator_print_indent(file, identation);
    if (node->if_expr.then_branch->type != AST_BEGIN && node->if_expr.else_branch->type != AST_BEGIN)
    {
        if_ternary(node, file, file_defs, identation);
    }
    else
    {
        char func_name[20];
        sprintf(func_name, "if_block_%d", get_counter());
        fprintf(file_defs, "def %s():\n", func_name);
        if_block(node, file_defs, file_defs, identation);

        fprintf(file, "%s()\n", func_name);
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
        assign(node, file, file_defs, identation);
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
    default:
        break;
    }
}