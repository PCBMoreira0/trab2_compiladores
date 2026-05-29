#include "ast.h"
#include <stdlib.h>
#include <stdio.h>

ASTNode *ast_create_if_stmt(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch)
{
    ASTNode *node = malloc(sizeof(ASTNode));

    node->type = AST_IF_STMT;
    node->if_stmt.condition = condition;
    node->if_stmt.then_branch = then_branch;
    node->if_stmt.else_branch = else_branch;

    return node;
}

ASTNode *ast_create_while_stmt(ASTNode *condition, ASTNode *body)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_WHILE_STMT;
    node->while_stmt.condition = condition;
    node->while_stmt.body = body;

    return node;
}

ASTNode *ast_create_assign_stmt(char *var_name, ASTNode *value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_ASSIGN_STMT;
    node->assign_stmt.var_name = var_name;
    node->assign_stmt.value = value;

    return node;
}

ASTNode *ast_create_op(char operation, ASTNode *left, ASTNode *right)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_OP;
    node->op.operation = operation;
    node->op.left = left;
    node->op.right = right;

    return node;
}

ASTNode *ast_create_block(ASTNode *command, ASTNode *next)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_BLOCK;
    node->block.command = command;
    node->block.next = next;

    return node;
}

ASTNode *ast_create_identifier(char *id_name)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_IDENTIFIER;
    node->id_name = id_name;

    return node;
}
ASTNode *ast_create_literal_int(int int_val)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = AST_LITERAL_INT;
    node->int_val = int_val;

    return node;
}


void ast_print_indent(int level)
{
    for (int i = 0; i < level; i++)
    {
        printf("  | ");
    }
}

void ast_print(ASTNode *node, int level)
{
    if (node == NULL)
        return;

    ast_print_indent(level);

    switch (node->type)
    {
        case AST_LITERAL_INT:
            printf("Numero: %d\n", node->int_val);
            break;

        case AST_IDENTIFIER:
            printf("Variavel: %s\n", node->id_name);
            break;

        case AST_OP:
            printf("Operacao Binaria (%c)\n", node->op.operation);
            ast_print(node->op.left, level + 1);
            ast_print(node->op.right, level + 1);
            break;

        case AST_ASSIGN_STMT:
            printf("Atribuicao (=)\n");
            ast_print_indent(level + 1);
            printf("Alvo: %s\n", node->assign_stmt.var_name);
            ast_print(node->assign_stmt.value, level + 1);
            break;

        case AST_WHILE_STMT:
            printf("Comando ENQUANTO (While)\n");
            ast_print_indent(level + 1);
            printf("Condicao:\n");
            ast_print(node->while_stmt.condition, level + 2);
            ast_print_indent(level + 1);
            printf("Faca:\n");
            ast_print(node->while_stmt.body, level + 2);
            break;

        case AST_IF_STMT:
            printf("Comando SE (If)\n");
            ast_print_indent(level + 1);
            printf("Condicao:\n");
            ast_print(node->if_stmt.condition, level + 2);

            ast_print_indent(level + 1);
            printf("Entao:\n");
            ast_print(node->if_stmt.then_branch, level + 2);

            if (node->if_stmt.else_branch != NULL)
            {
                ast_print_indent(level + 1);
                printf("Senao:\n");
                ast_print(node->if_stmt.else_branch, level + 2);
            }
            break;

        case AST_BLOCK:
            printf("Lista de Comandos:\n");
            ast_print(node->block.command, level + 1);
            // Continua a lista no mesmo nível de indentação
            ast_print(node->block.next, level + 1);
            break;

        default:
            printf("No desconhecido!\n");
            break;
    }
}