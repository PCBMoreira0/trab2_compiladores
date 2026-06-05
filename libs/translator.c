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
    char *var_name = node->define_var.name->value_str;

    translator_print_indent(file, identation);
    fprintf(file, "%s", var_name);
    fprintf(file, " = ");

    translate(node->define_var.value, file, 0);

    fprintf(file, "\n");
}

void define_func(ASTNode *node, FILE *file, int identation){
    char *func_name = node->func.name->value_str;
    fprintf(file, "def %s(",func_name);
    ASTNode *p = node->func.params;
    if (p != NULL) {
        for (;p->list.next != NULL; p = p->list.next){
            fprintf(file, "%s, ",p->list.item->value_str);
        }
        
        fprintf(file, "%s", p->list.item->value_str);
        
    }
    fprintf(file, "):\n");
    ASTNode *b = node->func.body;
    for (; b->list.next != NULL; b = b->list.next){
        translate(b->list.item, file, identation + 1);
    }
    translator_print_indent(file, identation + 1);
    fprintf(file, "return ");
    translate(b->list.item, file, identation);
    

}

// void op(ASTNode *node, FILE *file)
// {
//     char operation[3];
//     operation[0] = node->op.operation;
//     operation[1] = '\0';

//     if (strcmp(operation, "=") == 0)
//     {
//         strcpy(operation, "==");
//     }

//     translate(node->op.left, file, 0);
//     fprintf(file, " %s ", operation);
//     translate(node->op.right, file, 0);
// }

// void if_stmt(ASTNode *node, FILE *file, int identation)
// {
//     translator_print_indent(file, identation);
//     fprintf(file, "if ");
//     translate(node->if_stmt.condition, file, 0);
//     fprintf(file, ":\n");
//     translate(node->if_stmt.then_branch, file, identation + 1);
//     if (node->if_stmt.else_branch != NULL)
//     {
//         translator_print_indent(file, identation);
//         fprintf(file, "else:\n");
//         translate(node->if_stmt.else_branch, file, identation + 1);
//     }
// }


// void while_stmt(ASTNode *node, FILE *file, int identation)
// {
//     translator_print_indent(file, identation);
//     fprintf(file, "while ");
//     translate(node->while_stmt.condition, file, 0);
//     fprintf(file, ":\n");
//     translate(node->while_stmt.body, file, identation + 1);
// }

 

void translate(ASTNode *node, FILE *file, int identation)
{
    if (node == NULL) return;
    switch (node->type)
    {
        case AST_LIST:
            translate(node->list.item, file, identation);
            translate(node->list.next, file, identation);
            break;
        case AST_DEFINE_VAR:
            assign(node, file, identation);
            break;
        case AST_VARIABLE:
            fprintf(file, "%s",node->value_str);
            break;
        case AST_NUMBER:
            fprintf(file, "%s", node->value_str);
            break;
        case AST_DEFINE_FUNC:
            define_func(node, file, identation);
            break;
        
        default:
            break;
    }
}