#include "translator.h"
#include <stdlib.h>
#include <string.h>

void translator_print_indent(FILE* out, int identation) {
    for (int i = 0; i < identation * 4; i++) {
        fputc(' ', out);
    }
}

void assign(ASTNode *node, FILE *file, int identation)
{
    char *var_name = node->assign_stmt.var_name;

    translator_print_indent(file, identation);
    fprintf(file, "%s", var_name);
    fprintf(file, " = ");

    translate(node->assign_stmt.value, file, 0);

    fprintf(file, "\n");
}

void op(ASTNode *node, FILE *file)
{
    char operation[3];
    operation[0] = node->op.operation;
    operation[1] = '\0';

    if (strcmp(operation, "=") == 0)
    {
        strcpy(operation, "==");
    }

    translate(node->op.left, file, 0);
    fprintf(file, " %s ", operation);
    translate(node->op.right, file, 0);
}

void if_stmt(ASTNode *node, FILE *file, int identation)
{
    translator_print_indent(file, identation);
    fprintf(file, "if ");
    translate(node->if_stmt.condition, file, 0);
    fprintf(file, ":\n");
    translate(node->if_stmt.then_branch, file, identation + 1);
    if (node->if_stmt.else_branch != NULL)
    {
        translator_print_indent(file, identation);
        fprintf(file, "else:\n");
        translate(node->if_stmt.else_branch, file, identation + 1);
    }
}


void while_stmt(ASTNode *node, FILE *file, int identation)
{
    translator_print_indent(file, identation);
    fprintf(file, "while ");
    translate(node->while_stmt.condition, file, 0);
    fprintf(file, ":\n");
    translate(node->while_stmt.body, file, identation + 1);
}

void translate(ASTNode *node, FILE *file, int identation)
{
    switch (node->type)
    {
    case AST_ASSIGN_STMT:
        assign(node, file, identation);
        break;
    case AST_OP:
        op(node, file);
        break;
    case AST_LITERAL_INT:
        fprintf(file, "%d", node->int_val);
        break;
    case AST_IDENTIFIER:
        fprintf(file, "%s", node->id_name);
        break;
    case AST_BLOCK:
        translate(node->block.command, file, identation);
        translate(node->block.next, file, identation);
        break;
    case AST_IF_STMT:
        if_stmt(node, file, identation);
        break;
    case AST_WHILE_STMT:
        while_stmt(node, file, identation);
        break;
    default:
        break;
    }
}