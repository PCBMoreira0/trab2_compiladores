#pragma once

typedef enum
{
    AST_IF_STMT,
    AST_WHILE_STMT,
    AST_ASSIGN_STMT,

    AST_OP,

    AST_IDENTIFIER,
    AST_LITERAL_INT,

    AST_BLOCK
} ASTNodeType;

typedef struct ASTNode
{
    ASTNodeType type;
    union
    {
        struct
        {
            struct ASTNode *condition;
            struct ASTNode *then_branch;
            struct ASTNode *else_branch;
        } if_stmt;

        struct
        {
            struct ASTNode *condition;
            struct ASTNode *body;
        } while_stmt;

        struct
        {
            char *var_name;
            struct ASTNode *value;
        } assign_stmt;

        struct
        {
            char operation;
            struct ASTNode *left;
            struct ASTNode *right;
        } op;

        struct
        {
            struct ASTNode *command;
            struct ASTNode *next;
        } block;
        
        int int_val;
        char *id_name;
    };
} ASTNode;

ASTNode *ast_create_if_stmt(ASTNode *condition, ASTNode *then_branch, ASTNode *else_branch);
ASTNode *ast_create_while_stmt(ASTNode *condition, ASTNode *body);
ASTNode *ast_create_assign_stmt(char *var_name, ASTNode *value);

ASTNode *ast_create_op(char operation, ASTNode *left, ASTNode *right);

ASTNode *ast_create_block(ASTNode *command, ASTNode *next);

ASTNode *ast_create_identifier(char *id_name);
ASTNode *ast_create_literal_int(int int_val);

void ast_print(ASTNode *node, int level);