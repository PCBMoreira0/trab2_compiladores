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

#define TRANSLATOR_MAX_ERRORS 64
static const char *translator_errors_list[TRANSLATOR_MAX_ERRORS];
static int translator_errors_n = 0;

const char **translator_get_errors(void)  { return translator_errors_list; }
int translator_get_error_count(void) { return translator_errors_n; }

static int translator_expr_depth = 0;

static void translator_emit_unsupported(FILE *file, const char *msg)
{
    if (translator_errors_n < TRANSLATOR_MAX_ERRORS)
        translator_errors_list[translator_errors_n++] = msg;
    fprintf(file, "None");
}

unsigned int get_counter()
{
    static unsigned int count = 0;
    return count++;
}

static char sanitized_buf[512];

const char *sanitize_name(const char *name)
{
    int j = 0;
    for (int i = 0; name[i] != '\0' && j < (int)sizeof(sanitized_buf) - 3; i++)
    {
        if (name[i] == '-' && name[i + 1] == '>')
        {
            sanitized_buf[j++] = '_';
            sanitized_buf[j++] = 't';
            sanitized_buf[j++] = 'o';
            sanitized_buf[j++] = '_';
            i++;
        }
        else if (name[i] == '-') sanitized_buf[j++] = '_';
        else if (name[i] == '?') { sanitized_buf[j++] = '_'; sanitized_buf[j++] = 'p'; }
        else if (name[i] == '!') { sanitized_buf[j++] = '_'; sanitized_buf[j++] = 'x'; }
        else                      sanitized_buf[j++] = name[i];
    }
    sanitized_buf[j] = '\0';
    return sanitized_buf;
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

    case AST_COND:
    {
        int first = 1;
        for (ASTNode *cl = node->cond.clauses; cl != NULL; cl = cl->list.next)
        {
            ASTNode *clause = cl->list.item; // AST_COND_CLAUSE

            translator_print_indent(file, identation);
            fprintf(file, first ? "if " : "elif ");
            first = 0;
            translate(clause->cond_clause.test, file, file_defs, 0);
            fprintf(file, ":\n");

            ASTNode *b = clause->cond_clause.body;
            if (b == NULL)
            {
                fprintf(file, "None  # forma => nao suportada\n");
                continue;
            }
            for (; b->list.next != NULL; b = b->list.next)
                translate(b->list.item, file, file_defs, identation + 1);
            translate_as_block(b->list.item, file, file_defs,
                               identation + 1, mode, var_name);
        }

        if (node->cond.else_clause != NULL)
        {
            translator_print_indent(file, identation);
            fprintf(file, "else:\n");
            ASTNode *b = node->cond.else_clause;
            for (; b->list.next != NULL; b = b->list.next)
                translate(b->list.item, file, file_defs, identation + 1);
            translate_as_block(b->list.item, file, file_defs,
                               identation + 1, mode, var_name);
        }
        break;
    }

    case AST_CASE:
    {
        int first = 1;
        for (ASTNode *cl = node->case_expr.clauses; cl != NULL; cl = cl->list.next)
        {
            ASTNode *clause = cl->list.item; // AST_CASE_CLAUSE

            translator_print_indent(file, identation);
            fprintf(file, first ? "if " : "elif ");
            first = 0;
            translate(node->case_expr.key, file, file_defs, 0);
            fprintf(file, " in (");

            ASTNode *d = clause->case_clause.data;
            for (; d != NULL && d->list.next != NULL; d = d->list.next)
            {
                translate(d->list.item, file, file_defs, 0);
                fprintf(file, ", ");
            }
            if (d != NULL)
                translate(d->list.item, file, file_defs, 0);
            fprintf(file, "):\n");

            ASTNode *b = clause->case_clause.body;
            for (; b->list.next != NULL; b = b->list.next)
                translate(b->list.item, file, file_defs, identation + 1);
            translate_as_block(b->list.item, file, file_defs,
                               identation + 1, mode, var_name);
        }

        if (node->case_expr.else_clause != NULL)
        {
            translator_print_indent(file, identation);
            fprintf(file, "else:\n");
            ASTNode *b = node->case_expr.else_clause;
            for (; b->list.next != NULL; b = b->list.next)
                translate(b->list.item, file, file_defs, identation + 1);
            translate_as_block(b->list.item, file, file_defs,
                               identation + 1, mode, var_name);
        }
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
    if (node->type == AST_COND || node->type == AST_CASE)
        return 1;
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
        fprintf(file, "%s = ", sanitize_name(var_name));
        translate(value, file, file_defs, 0);
        fprintf(file, "\n");
    }
}

void define_func(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    char *func_name = node->func.name->value_str;
    fprintf(file, "def %s(", sanitize_name(func_name));
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
        translator_emit_unsupported(file, "let complexo nao suportado");
        return;
    }

    fprintf(file, "(lambda ");
    ASTNode *i;
    for (i = node->let_expr.bindings; i->list.next != NULL; i = i->list.next)
    {
        fprintf(file, "%s, ", sanitize_name(i->list.item->binding.name->value_str));
    }
    fprintf(file, "%s: ", sanitize_name(i->list.item->binding.name->value_str));

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
    fprintf(file, "(lambda %s: ", sanitize_name(bind->binding.name->value_str));
    let_star_bindings(binding_list->list.next, body, file, file_defs, identation);
    fprintf(file, ")(");
    translate(bind->binding.value, file, file_defs, identation);
    fprintf(file, ")");
}

void let_star(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    if (node->let_expr.body->list.next != NULL)
    {
        translator_emit_unsupported(file, "let* complexo nao suportado");
        return;
    }

    let_star_bindings(node->let_expr.bindings, node->let_expr.body, file, file_defs, identation);
}

void let_named(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    ASTNode f;
    f.type = AST_DEFINE_FUNC;
    char buffer[20];
    sprintf(buffer, "%s_%d", sanitize_name(node->let_expr.name->value_str), get_counter());
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
        translator_emit_unsupported(file, "lambda complexo nao suportado");
        return;
    }

    fprintf(file, "lambda ");
    ASTNode *p = node->func.params;
    if (p != NULL)
    {
        for (; p->list.next != NULL; p = p->list.next)
            fprintf(file, "%s, ", sanitize_name(p->list.item->value_str));
        fprintf(file, "%s", sanitize_name(p->list.item->value_str));
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

    translator_expr_depth++;
    ASTNode *i;
    for (i = node->call.operands; i->list.next != NULL; i = i->list.next)
    {
        translate(i->list.item, file, NULL, identation);
        fprintf(file, " %s ", op);
    }
    translate(i->list.item, file, NULL, identation);
    translator_expr_depth--;
}

int call_logic(ASTNode *node, FILE *file, int identation)
{
    translator_expr_depth++;
    ASTNode *i;
    for (i = node->logical.tests; i->list.next != NULL; i = i->list.next)
    {
        translate(i->list.item, file, NULL, identation);
        fprintf(file, " %s ", node->type == AST_AND ? "and" : "or");
    }
    translate(i->list.item, file, NULL, identation);
    translator_expr_depth--;
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
            fprintf(file, translator_expr_depth > 0 ? ")" : ")\n");
            return;
        }
        translator_expr_depth++;
        ASTNode *i;
        for (i = node->call.operands; i->list.next != NULL; i = i->list.next)
        {
            translate(i->list.item, file, file_defs, 0);
            fprintf(file, ", ");
        }
        translate(i->list.item, file, file_defs, 0);
        translator_expr_depth--;
        fprintf(file, translator_expr_depth > 0 ? ")" : ")\n");
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
    fprintf(file, "%s", sanitize_name(name));
}

void translate_character(ASTNode *node, FILE *file)
{
    const char *s = node->value_str + 2;  // pula "#\"
    if      (strcmp(s, "space")   == 0) fprintf(file, "' '");
    else if (strcmp(s, "newline") == 0) fprintf(file, "'\\n'");
    else if (strcmp(s, "tab")     == 0) fprintf(file, "'\\t'");
    else                                fprintf(file, "'%s'", s);
}

void translate_datum(ASTNode *node, FILE *file)
{
    if (node == NULL) { fprintf(file, "[]"); return; }

    switch (node->type)
    {
    case AST_VARIABLE:  fprintf(file, "\"%s\"", node->value_str); break;
    case AST_NUMBER:    fprintf(file, "%s",      node->value_str); break;
    case AST_STRING:    fprintf(file, "%s",      node->value_str); break;
    case AST_BOOLEAN:
        fprintf(file, "%s", strcmp(node->value_str, "#t") == 0 ? "True" : "False");
        break;
    case AST_CHARACTER:
        translate_character(node, file);
        break;
    case AST_LIST:
    {
        fprintf(file, "[");
        int first = 1;
        for (ASTNode *i = node; i != NULL; i = i->list.next)
        {
            if (!first) fprintf(file, ", ");
            translate_datum(i->list.item, file);
            first = 0;
        }
        fprintf(file, "]");
        break;
    }
    case AST_VECTOR:
    {
        fprintf(file, "[");
        int first = 1;
        for (ASTNode *i = node->vector.elements; i != NULL; i = i->list.next)
        {
            if (!first) fprintf(file, ", ");
            translate_datum(i->list.item, file);
            first = 0;
        }
        fprintf(file, "]");
        break;
    }
    default:
        translator_emit_unsupported(file, "datum nao suportado");
        break;
    }
}

void translate_do(ASTNode *node, FILE *file, FILE *file_defs, int identation)
{
    // 1. inicializações
    for (ASTNode *s = node->do_expr.specs; s != NULL; s = s->list.next)
    {
        ASTNode *spec = s->list.item; // AST_ITER_SPEC
        translator_print_indent(file, identation);
        fprintf(file, "%s = ", sanitize_name(spec->iter_spec.name->value_str));
        translate(spec->iter_spec.init, file, file_defs, 0);
        fprintf(file, "\n");
    }

    // 2. while not test:
    translator_print_indent(file, identation);
    fprintf(file, "while not (");
    translate(node->do_expr.test, file, file_defs, 0);
    fprintf(file, "):\n");

    // 3. corpo
    for (ASTNode *c = node->do_expr.commands; c != NULL; c = c->list.next)
        translate(c->list.item, file, file_defs, identation + 1);

    // 4. atualização simultânea (só vars que têm step)
    int step_count = 0;
    for (ASTNode *s = node->do_expr.specs; s != NULL; s = s->list.next)
        if (s->list.item->iter_spec.step != NULL)
            step_count++;
    if (step_count > 0)
    {
        translator_print_indent(file, identation + 1);
        int first = 1;
        for (ASTNode *s = node->do_expr.specs; s != NULL; s = s->list.next)
        {
            if (s->list.item->iter_spec.step == NULL)
                continue;
            if (!first)
                fprintf(file, ", ");
            fprintf(file, "%s", sanitize_name(s->list.item->iter_spec.name->value_str));
            first = 0;
        }
        fprintf(file, " = ");
        first = 1;
        for (ASTNode *s = node->do_expr.specs; s != NULL; s = s->list.next)
        {
            if (s->list.item->iter_spec.step == NULL)
                continue;
            if (!first)
                fprintf(file, ", ");
            translate(s->list.item->iter_spec.step, file, file_defs, 0);
            first = 0;
        }
        fprintf(file, "\n");
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
        fprintf(file, "%s = ", sanitize_name(node->set_expr.name->value_str));
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
    case AST_LETREC:
        translator_emit_unsupported(file, "letrec nao suportado");
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
    case AST_COND:
        translate_as_block(node, file, NULL, identation, TR_RETURN, NULL);
        break;
    case AST_CASE:
        translate_as_block(node, file, file_defs, identation, TR_RETURN, NULL);
        break;
    case AST_DO:
        translate_do(node, file, file_defs, identation);
        break;
    case AST_QUOTE:
        translate_datum(node->quote.datum, file);
        break;
    case AST_VECTOR:
    {
        fprintf(file, "[");
        int first = 1;
        for (ASTNode *i = node->vector.elements; i != NULL; i = i->list.next)
        {
            if (!first) fprintf(file, ", ");
            translate_datum(i->list.item, file);
            first = 0;
        }
        fprintf(file, "]");
        break;
    }
    case AST_CHARACTER:
        translate_character(node, file);
        break;
    default:
        break;
    }
}