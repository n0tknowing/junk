#include <cstdio>

class noncopyable {
public:
    noncopyable(const noncopyable &) = delete;
    noncopyable& operator=(const noncopyable &) = delete;
protected:
    noncopyable() = default;
    ~noncopyable() = default;
};

class copyable {
protected:
    copyable() = default;
    ~copyable() = default;
};

class foobar : noncopyable {
public:
    foobar(int x) : m_x(x) {}
    int get() { return m_x; }
private:
    int m_x;
};

int main()
{
    foobar x(1);
    printf("%d\n", x.get());

    // foobar y = x; // error
}
