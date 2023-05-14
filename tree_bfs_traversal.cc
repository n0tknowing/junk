// BFS Tree traversal

#include <cassert>
#include <cstdio>
#include <memory>
#include <vector>
#include <queue>

struct Node {
    std::shared_ptr<Node> left;
    std::shared_ptr<Node> right;
    int val;
};

using node_ptr = std::shared_ptr<Node>;

node_ptr new_node(node_ptr left, node_ptr right, int v) {
    node_ptr node = std::make_shared<Node>();
    node->left = left;
    node->right = right;
    node->val = v;
    return node;
}

node_ptr new_leaf(int v) {
    return new_node(nullptr, nullptr, v);
}

std::vector<int> traverse(node_ptr root) {
    std::vector<int> res;
    res.reserve(15);

    std::queue<node_ptr> q;
    q.push(root);

    while (!q.empty()) {
        node_ptr& node = q.front();
        q.pop();
        res.push_back(node->val);
        if (node->left != nullptr)
            q.push(node->left);
        if (node->right != nullptr)
            q.push(node->right);
    }

    return res;
}

int main(void)
{
    node_ptr node0, node1, node2, node3, node4, node5, node;

    node0 = new_node(new_leaf(1), new_leaf(3), 2);
    node1 = new_node(new_leaf(5), new_leaf(7), 6);
    node2 = new_node(new_leaf(9), new_leaf(11), 10);
    node3 = new_node(new_leaf(13), new_leaf(15), 14);
    node4 = new_node(node0, node1, 4);
    node5 = new_node(node2, node3, 12);
    node = new_node(node4, node5, 8);

    auto v = traverse(node);
    for (const int i : v)
        printf("%d ", i);
    printf("\n");
}
