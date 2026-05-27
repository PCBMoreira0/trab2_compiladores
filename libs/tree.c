#include <stdio.h>
#include <stdlib.h>
#include "tree.h"

TreeNode *create_node(void *data)
{
    TreeNode *new_node = (TreeNode *)malloc(sizeof(TreeNode));
    if (new_node == NULL)
    {
        printf("Memory allocation error\n");
        exit(1);
    }

    new_node->data = data;
    new_node->filho = NULL;
    new_node->irmao = NULL;

    return new_node;
}

void add_child(TreeNode *parent, TreeNode *child)
{
    if (parent == NULL || child == NULL)
        return;

    if (parent->filho == NULL)
    {
        parent->filho = child;
    }
    else
    {
        TreeNode *temp = parent->filho;
        while (temp->irmao != NULL)
        {
            temp = temp->irmao;
        }
        temp->irmao = child;
    }
}

static void print_tree_recursive(
    TreeNode *root,
    int level,
    int is_last,
    const char *(*to_string)(void *))
{
    if (root == NULL)
        return;

    for (int i = 0; i < level - 1; i++)
    {
        printf("│   ");
    }

    if (level > 0)
    {
        printf(is_last ? "└── " : "├── ");
    }

    printf("%s\n", to_string(root->data));

    TreeNode *child = root->filho;

    while (child != NULL)
    {
        int last = (child->irmao == NULL);
        print_tree_recursive(child, level + 1, last, to_string);
        child = child->irmao;
    }
}

void print_tree(TreeNode *root, const char *(*to_string)(void *))
{
    print_tree_recursive(root, 0, 1, to_string);
}

void free_tree(TreeNode *root, void (*free_data)(void *))
{
    if (root == NULL)
        return;

    free_tree(root->filho, free_data);
    free_tree(root->irmao, free_data);

    if (free_data != NULL && root->data != NULL)
    {
        free_data(root->data);
    }

    free(root);
}