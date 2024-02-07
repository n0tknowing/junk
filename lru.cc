#include <cstddef>
#include <unordered_map>
#include <list>

class LRUCache {
public:
    LRUCache(int capacity) {
        C = static_cast<size_t>(capacity);
        M.reserve(C);
    }

    int get(int key) {
        auto M_It = M.find(key);
        if (M_It == M.end())
            return -1;

        std::list<node_pair>::iterator L_It = M_It->second;
        int value = L_It->value;
        L.splice(L.begin(), L, L_It);

        return value;
    }

    void put(int key, int value) {
        if (auto M_It = M.find(key); M_It != M.end()) {
            // key is already in the cache, we can simply move the node to the
            // head
            std::list<node_pair>::iterator L_It = M_It->second;
            L_It->value = value;
            L.splice(L.begin(), L, L_It);
        } else {
            std::list<node_pair>::iterator L_It;
            node_pair P = { key, value };
            if (L.size() == C) {
                L_It = L.end(); L_It--;
                M.erase(L_It->key);
                *L_It = P; // reuse node
                L.splice(L.begin(), L, L_It); // now the node become the head
            } else {
                L.push_front(P);
                L_It = L.begin();
            }
            M[key] = L_It;
        }
    }

private:
    size_t C;
    struct node_pair { int key; int value; };
    std::unordered_map<int, std::list<node_pair>::iterator> M;
    std::list<node_pair> L;
};
