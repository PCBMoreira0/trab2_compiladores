#pragma once

typedef struct TreeNode {
    int valor;
    struct TreeNode *filho;
    struct TreeNode *irmao;
} TreeNode;

TreeNode* create_node(int value);
void add_child(TreeNode* parent, TreeNode* child);
void print_tree(TreeNode *root, const char *(*to_string)(int));
void free_tree(TreeNode* root);
