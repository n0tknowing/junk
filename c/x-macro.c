#define X(x) \
    x(eof), x(lparen), x(rparen), x(number), x(plus), x(minus) \
    x(star), x(slash)

#define T(x) tok_##x,
typedef enum {
    X(T)
} token_kind_t;
#undef T
