#pragma once

typedef struct TreeNode {
    void *data; 
    struct TreeNode *filho;
    struct TreeNode *irmao;
} TreeNode;

TreeNode* create_node(void *data);
void add_child(TreeNode* parent, TreeNode* child);
void print_tree(TreeNode *root, const char *(*to_string)(void *));

void free_tree(TreeNode* root, void (*free_data)(void *));