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

void print_tree_preorder(struct Node *root, int indent)
{
    if (root == NULL)
        return;

    printf("%*s%c\n", indent, " ", root->val);
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
    printf("%*s%c\n", indent, " ", root->val);
    fflush(stdout);
}

void print_tree_inorder(struct Node *root, int indent)
{
    if (root == NULL)
        return;

    print_tree_inorder(root->left, indent + 4);
    printf("%*s%c\n", indent, " ", root->val);
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
    struct Node *a, *b, *c, *d, *e, *f, *g;

    a = new_node(NULL, NULL, '1');
    b = new_node(NULL, NULL, '2');
    c = new_node(NULL, NULL, '3');
    d = new_node(NULL, NULL, '4');
    e = new_node(b, c, '*');
    f = new_node(a, e, '+');
    g = new_node(f, d, '-');

    printf("Pre-order:\n\n");
    print_tree_preorder(g, 0);
    printf("\n");

    printf("In-order:\n\n");
    print_tree_inorder(g, 0);
    printf("\n");

    printf("Post-order:\n\n");
    print_tree_postorder(g, 0);
    printf("\n");

    destroy_tree(g);
}
