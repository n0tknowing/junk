// DFS Tree traversal
// I always forgot about this even though they are simple
// For use in AST, post-order is used

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

struct Node {
    struct Node *left;
    struct Node *right;
    int val;
};

struct Node *new_node(struct Node *left, struct Node *right, int v)
{
    struct Node *node = calloc(1, sizeof(*node));
    assert(node != NULL);
    node->left = left;
    node->right = right;
    node->val = v;
    return node;
}

struct Node *new_leaf(int v)
{
    return new_node(NULL, NULL, v);
}

void print_tree_preorder(struct Node *root, int indent)
{
    if (root == NULL)
        return;

    printf("%*s%d\n", indent, " ", root->val);
    fflush(stdout);
    print_tree_preorder(root->left, indent + 4);
    print_tree_preorder(root->right, indent + 4);
}

void print_tree_postorder(struct Node *root, int indent)
{
    if (root == NULL)
        return;

    print_tree_postorder(root->left, indent + 4);
    print_tree_postorder(root->right, indent + 4);
    printf("%*s%d\n", indent, " ", root->val);
    fflush(stdout);
}

void print_tree_inorder(struct Node *root, int indent)
{
    if (root == NULL)
        return;

    print_tree_inorder(root->left, indent + 4);
    printf("%*s%d\n", indent, " ", root->val);
    fflush(stdout);
    print_tree_inorder(root->right, indent + 4);
}

void destroy_tree(struct Node *root)
{
    if (root == NULL)
        return;

    destroy_tree(root->left);
    destroy_tree(root->right);
    free(root);
}

int main(void)
{
    struct Node *node0, *node1, *node2, *node3, *node4, *node5;
    struct Node *node;

    node0 = new_node(new_leaf(1), new_leaf(3), 2);
    node1 = new_node(new_leaf(5), new_leaf(7), 6);
    node2 = new_node(new_leaf(9), new_leaf(11), 10);
    node3 = new_node(new_leaf(13), new_leaf(15), 14);
    node4 = new_node(node0, node1, 4);
    node5 = new_node(node2, node3, 12);
    node = new_node(node4, node5, 8);

    printf("Pre-order:\n\n");
    print_tree_preorder(node, 0);
    printf("\n");

    printf("In-order:\n\n");
    print_tree_inorder(node, 0);
    printf("\n");

    printf("Post-order:\n\n");
    print_tree_postorder(node, 0);
    printf("\n");

    destroy_tree(node);
}
