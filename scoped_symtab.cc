#include <array>
#include <stdexcept>
#include <string>
#include <unordered_set>

#include <fmt/core.h>

struct symtab_t {
    symtab_t() : top(0) {}
    ~symtab_t() {}

    symtab_t(const symtab_t&) = delete;
    symtab_t(symtab_t&&) = delete;
    symtab_t& operator=(const symtab_t&) = delete;
    symtab_t& operator=(symtab_t&&) = delete;

    void push_scope() {
        if (top >= MAX_NESTED_SCOPE)
            throw std::out_of_range("too many nested block scope");
        top++;
    }

    void pop_scope() {
        if (top > 0) {
            remove_all();
            top--;
        }
    }

    bool lookup(const std::string& name) {
        return scope[top].find(name) != scope[top].end();
    }

    bool lookup_all(const std::string& name) {
        for (int t = top; t >= 0; t--) {
            if (scope[t].find(name) != scope[t].end())
                return true;
        }
        return false;
    }

    void remove(const std::string& name) {
        try {
            scope[top].erase(name);
        } catch (std::exception& e) {
            ;
        }
    }

    void remove_all() {
        scope[top].clear();
    }

    void insert(const std::string& name) {
        if (lookup(name))
            throw std::runtime_error(name + " already declared in current scope");
        if (scope[top].size() > MAX_SYMBOLS)
            throw std::runtime_error("too many symbols in current scope");
        scope[top].insert(name);
    }

private:
    static constexpr int MAX_NESTED_SCOPE = 15, MAX_SYMBOLS = 511;
    std::array<std::unordered_set<std::string>, MAX_NESTED_SCOPE + 1> scope;
    int top; // 0 = global, 1..15 = block scope
};

int main() {
    symtab_t sym;

    sym.insert("foo");
//    sym.insert("foo");

    sym.push_scope();

    sym.insert("foo");

    if (sym.lookup("foo"))
        fmt::print("foo exists in current scope\n");
    else
        fmt::print("foo doesn't exist in current scope\n");

    if (sym.lookup("bar"))
        fmt::print("bar exists in current scope\n");
    else
        fmt::print("bar doesn't exist in current scope\n");

    sym.pop_scope();

    if (sym.lookup("foo"))
        fmt::print("foo exists in current scope\n");
    else
        fmt::print("foo doesn't exist in current scope\n");

    if (sym.lookup_all("foo"))
        fmt::print("foo exists in ??? scope\n");
    else
        fmt::print("foo doesn't exist in any scope :(\n");
}
