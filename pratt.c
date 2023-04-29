//  Operator          Associativity       Description
//  --------          -------------       -----------
//  +x -x ~x not x    right to left       Positive, negative, bitwise NOT,
//                                        logical NOT
//  * / %             left to right       Multiplication, division, modulo
//  + -               left to right       Addition, subtraction
//  << >>             left to right       Bitwise shifts
//  &                 left to right       Bitwise AND
//  ^                 left to right       Bitwise XOR
//  |                 left to right       Bitwise OR
//  == != < > <= >=   left to right       Relational operators
//  and               left to right       Logical AND
//  or                left to right       Logical OR
//  ?:                right to left       Ternary conditional


#define _GNU_SOURCE // needed for getchar_unlocked()
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "c/my_ctype/ctype.h"

// === Lexer ================================================
// ==========================================================

typedef enum {
    tok_eof, tok_lparen, tok_rparen, tok_number, tok_plus, tok_minus,
    tok_star, tok_slash, tok_percent, tok_ternary_if, tok_ternary_else,
    tok_bitand, tok_bitxor, tok_bitor, tok_bitnot, tok_logand, tok_logor,
    tok_lognot, tok_lshift, tok_rshift, tok_eq, tok_ne, tok_lt, tok_gt,
    tok_le, tok_ge,
    tok_unknown = 256,
} token_kind_t;

typedef struct {
    const char *lexeme;
    size_t len, line;
    token_kind_t kind;
} token_t;

typedef struct {
    token_t tok;
    const char *cur;
    const char *end;
} lexer_t;

void lexer_next(lexer_t *l)
{
    while (l->cur < l->end && is_space(l->cur[0])) {
        if (l->cur[0] == '\n') l->tok.line++;
        l->cur++;
    }

    l->tok.lexeme = l->cur;
    l->tok.len = 0;

    if (l->cur >= l->end) {
        l->tok.kind = tok_eof;
        return;
    } else if (is_digit(l->cur[0])) {
        do {
            l->tok.len++;
            l->cur++;
        } while (l->cur < l->end && is_digit(l->cur[0]));
        l->tok.kind = tok_number;
        return;
    } else if (is_alpha(l->cur[0])) {
        if (l->cur[0] == 'a' && strncmp(l->cur, "and", 3) == 0) {
            l->tok.kind = tok_logand;
            l->tok.len = 3;
            l->cur += 3;
        } else if (l->cur[0] == 'n' && strncmp(l->cur, "not", 3) == 0)  {
            l->tok.kind = tok_lognot;
            l->tok.len = 3;
            l->cur += 3;
        } else if (l->cur[0] == 'o' && strncmp(l->cur, "or", 2) == 0) {
            l->tok.kind = tok_logor;
            l->tok.len = 2;
            l->cur += 2;
        } else { // identifier :-)
            l->tok.kind = tok_unknown;
            l->tok.len = l->end - l->cur; // eat all
            l->cur += l->tok.len;
        }
        return;
    }

    intptr_t off = 1;

    switch (l->cur[0]) {
    case '(':
        l->tok.kind = tok_lparen;
        break;
    case ')':
        l->tok.kind = tok_rparen;
        break;
    case '+':
        l->tok.kind = tok_plus;
        break;
    case '-':
        l->tok.kind = tok_minus;
        break;
    case '*':
        l->tok.kind = tok_star;
        break;
    case '/':
        l->tok.kind = tok_slash;
        break;
    case '%':
        l->tok.kind = tok_percent;
        break;
    case '?':
        l->tok.kind = tok_ternary_if;
        break;
    case ':':
        l->tok.kind = tok_ternary_else;
        break;
    case '&':
        l->tok.kind = tok_bitand;
        break;
    case '^':
        l->tok.kind = tok_bitxor;
        break;
    case '|':
        l->tok.kind = tok_bitor;
        break;
    case '~':
        l->tok.kind = tok_bitnot;
        break;
    case '>':
        if (l->cur[1] == '>') {
            l->tok.kind = tok_rshift;
            off = 2;
        } else if (l->cur[1] == '=') {
            l->tok.kind = tok_ge;
            off = 2;
        } else {
            l->tok.kind = tok_gt;
        }
        break;
    case '<':
        if (l->cur[1] == '<') {
            l->tok.kind = tok_lshift;
            off = 2;
        } else if (l->cur[1] == '=') {
            l->tok.kind = tok_le;
            off = 2;
        } else {
            l->tok.kind = tok_lt;
        }
        break;
    case '=':
        if (l->cur[1] == '=') {
            l->tok.kind = tok_eq;
            off = 2;
        }
        break;
    case '!':
        if (l->cur[1] == '=') {
            l->tok.kind = tok_ne;
            off = 2;
        }
        break;
    default:
        l->tok.kind = tok_unknown;
        off = l->end - l->cur; // eat all
        break;
    }

    l->tok.len = (size_t)off;
    l->cur += off;
}

// === AST ================================================
// ========================================================

typedef struct expr_t expr_t;

typedef enum {
    literal_kind, unary_kind, binary_kind, ternary_kind,
} expr_kind_t;

struct expr_t {
    expr_kind_t kind;
    union {
        struct {
            token_kind_t operator;
            expr_t *operand;
        } unary;
        struct {
            token_kind_t operator;
            expr_t *lhs;
            expr_t *rhs;
        } binary;
        struct {
            expr_t *cond;
            expr_t *vit; // value if true (non-zero)
            expr_t *vif; // value if false
        } ternary;
        int64_t literal; // boxed value like PyObject in CPython is better
    };
};

static expr_t *expr_new(expr_kind_t kind)
{
    expr_t *E = calloc(1, sizeof(*E));
    assert(E != NULL);
    E->kind = kind;
    return E;
}

static expr_t *expr_literal(int64_t v)
{
    expr_t *E = expr_new(literal_kind);
    E->literal = v;
    return E;
}

static expr_t *expr_unary(token_kind_t kind, expr_t *operand)
{
    expr_t *E = expr_new(unary_kind);
    E->unary.operator = kind;
    E->unary.operand = operand;
    return E;
}

static expr_t *expr_binary(token_kind_t kind, expr_t *lhs, expr_t *rhs)
{
    expr_t *E = expr_new(binary_kind);
    E->binary.operator = kind;
    E->binary.lhs = lhs;
    E->binary.rhs = rhs;
    return E;
}

static expr_t *expr_ternary(expr_t *cond, expr_t *vit, expr_t *vif)
{
    expr_t *E = expr_new(ternary_kind);
    E->ternary.cond = cond;
    E->ternary.vit = vit;
    E->ternary.vif = vif;
    return E;
}

void expr_free(expr_t *E)
{
    if (E == NULL) return;

    switch (E->kind) {
    case literal_kind:
        break;
    case unary_kind:
        expr_free(E->unary.operand);
        break;
    case binary_kind:
        expr_free(E->binary.lhs);
        expr_free(E->binary.rhs);
        break;
    case ternary_kind:
        expr_free(E->ternary.cond);
        expr_free(E->ternary.vit);
        expr_free(E->ternary.vif);
        break;
    default:
        break;
    }

    free(E);
}

// === Eval =================================================
// ==========================================================

int64_t expr_eval(expr_t *E);

static int64_t expr_unary_eval(expr_t *E)
{
    int64_t opr_v = expr_eval(E->unary.operand);

    switch (E->unary.operator) {
    case tok_plus:
        return 0 + opr_v;
    case tok_minus:
        return 0 - opr_v;
    case tok_bitnot:
        return ~opr_v;
    case tok_lognot:
        return !opr_v;
    default:
        fprintf(stderr, "invalid unary operator\n");
        exit(1);
    }
}

static int64_t expr_binary_eval(expr_t *E)
{
    int64_t lhs = expr_eval(E->binary.lhs);
    int64_t rhs = expr_eval(E->binary.rhs);

    switch (E->binary.operator) {
    case tok_plus:
        return lhs + rhs;
    case tok_minus:
        return lhs - rhs;
    case tok_star:
        return lhs * rhs;
    case tok_slash:
        if (rhs == 0) {
            fprintf(stderr, "attempt to divide by zero\n");
            exit(1);
        }
        return lhs / rhs;
    case tok_percent:
        if (rhs == 0) {
            fprintf(stderr, "attempt to modulo by zero\n");
            exit(1);
        }
        return lhs % rhs;
    case tok_bitand:
        return lhs & rhs;
    case tok_bitxor:
        return lhs ^ rhs;
    case tok_bitor:
        return lhs | rhs;
    case tok_logand:
        return lhs && rhs;
    case tok_logor:
        return lhs || rhs;
    case tok_lshift:
        if (lhs < 0 || rhs < 0) {
            fprintf(stderr, "bitwise shifts only work on non-negative "
                            "integer\n");
            exit(1);
        } else if (rhs > 63) {
            return 0;
        }
        return lhs << rhs;
    case tok_rshift:
        if (lhs < 0 || rhs < 0) {
            fprintf(stderr, "bitwise shifts only work on non-negative "
                            "integer\n");
            exit(1);
        } else if (lhs > 63) {
            return 0;
        }
        return lhs >> rhs;
    case tok_eq:
        return lhs == rhs;
    case tok_ne:
        return lhs != rhs;
    case tok_lt:
        return lhs < rhs;
    case tok_gt:
        return lhs > rhs;
    case tok_le:
        return lhs <= rhs;
    case tok_ge:
        return lhs >= rhs;
    default:
        fprintf(stderr, "invalid binary operator\n");
        exit(1);
    }
}

static int64_t expr_ternary_eval(expr_t *E)
{
    int64_t cond = expr_eval(E->ternary.cond);
    if (cond != 0) return expr_eval(E->ternary.vit);
    return expr_eval(E->ternary.vif);
}

int64_t expr_eval(expr_t *E)
{
    if (E == NULL) abort();

    switch (E->kind) {
    case literal_kind:
        return E->literal;
    case unary_kind:
        return expr_unary_eval(E);
    case binary_kind:
        return expr_binary_eval(E);
    case ternary_kind:
        return expr_ternary_eval(E);
    default:
        abort();
    }
}

// === Expression Parser ====================================
// ==========================================================

typedef struct {
    int left, right;
} expr_bp;

#define bp_left_assoc(bp)   (expr_bp){(bp)-1, (bp)}
#define bp_right_assoc(bp)  (expr_bp){(bp)+1, (bp)}
#define bp_unary  500

static expr_bp bp_lookup(token_kind_t tok)
{
    switch (tok) {
    case tok_ternary_if:
        return bp_right_assoc(20);
    case tok_logor:
        return bp_left_assoc(30);
    case tok_logand:
        return bp_left_assoc(40);
    case tok_eq:
    case tok_ne:
    case tok_lt:
    case tok_gt:
    case tok_le:
    case tok_ge:
        return bp_left_assoc(50);
    case tok_bitor:
        return bp_left_assoc(60);
    case tok_bitxor:
        return bp_left_assoc(70);
    case tok_bitand:
        return bp_left_assoc(80);
    case tok_lshift:
    case tok_rshift:
        return bp_left_assoc(90);
    case tok_plus:
    case tok_minus:
        return bp_left_assoc(100);
    case tok_star:
    case tok_slash:
    case tok_percent:
        return bp_left_assoc(200);
    default:
        break;
    }

    return (expr_bp){0, 0};
}

static expr_t *expr_parse_led(lexer_t *l, int min_bp);

static expr_t *expr_parse_subexpr(lexer_t *l)
{
    lexer_next(l);

    expr_t *E = expr_parse_led(l, 0);

    if (E == NULL) {
        fprintf(stderr, "missing expression after '('\n");
        exit(1);
    } else if (l->tok.kind == tok_eof) {
        fprintf(stderr, "unexpected EOF while parsing subexpression\n");
        exit(1);
    } else if (l->tok.kind != tok_rparen) {
        fprintf(stderr, "unterminated subexpression, missing ')'\n");
        exit(1);
    }

    lexer_next(l);
    return E;
}

// A somewhat naive number parser.
// In pratice, you should handle more than this, e.g. floating-point and other
// non-10 base integers like hexadecimal, octal and binary.
static expr_t *expr_parse_number(lexer_t *l)
{
    expr_t *E = NULL;
    char tmp[24] = {0};

    if (l->tok.len > 23) {
        fprintf(stderr, "number too big\n");
        exit(1);
    }

    memcpy(tmp, l->tok.lexeme, l->tok.len);

    errno = 0;
    char *end = NULL;
    int64_t v = strtol(tmp, &end, 10);
    if (errno == 0 && end != (char *)l->tok.lexeme) {
        E = expr_literal(v);
    } else {
        fprintf(stderr, "number too big\n");
        exit(1);
    }

    lexer_next(l);
    return E;
}

static expr_t *expr_parse_binary(lexer_t *l, expr_t *lhs, int rbp)
{
    token_t tok = l->tok;

    if (lhs == NULL) {
        fprintf(stderr, "missing expression for LHS of infix operator '%.*s'\n",
                        (int)tok.len, tok.lexeme);
        exit(1);
    }

    lexer_next(l);

    expr_t *rhs = expr_parse_led(l, rbp);
    if (rhs == NULL) {
        fprintf(stderr, "missing expression for RHS of infix operator '%.*s'\n",
                        (int)tok.len, tok.lexeme);
        exit(1);
    }

    return expr_binary(tok.kind, lhs, rhs);
}

static expr_t *expr_parse_unary(lexer_t *l)
{
    token_t tok = l->tok;
    lexer_next(l);

    expr_t *operand = expr_parse_led(l, bp_unary);
    if (operand == NULL) {
        fprintf(stderr, "missing expression for prefix operator '%.*s'\n",
                        (int)tok.len, tok.lexeme);
        exit(1);
    }

    return expr_unary(tok.kind, operand);
}

static expr_t *expr_parse_ternary(lexer_t *l, expr_t *cond)
{
    if (cond == NULL) {
        fprintf(stderr, "missing expression before '?'\n");
        exit(1);
    }

    lexer_next(l);

    expr_t *vit = expr_parse_led(l, 0);
    if (vit == NULL) {
        fprintf(stderr, "missing expression after '?'\n");
        exit(1);
    } else if (l->tok.kind != tok_ternary_else) {
        fprintf(stderr, "missing ':' in ternary conditional expression\n");
        exit(1);
    }

    lexer_next(l);

    expr_t *vif = expr_parse_led(l, 0);
    if (vif == NULL) {
        fprintf(stderr, "missing expression after ':'\n");
        exit(1);
    }

    return expr_ternary(cond, vit, vif);
}

static expr_t *expr_parse_nud(lexer_t *l)
{
    expr_t *E = NULL;

    switch (l->tok.kind) {
    case tok_lparen:
        E = expr_parse_subexpr(l);
        break;
    case tok_number:
        E = expr_parse_number(l);
        break;
    case tok_plus:
    case tok_minus:
    case tok_bitnot:
    case tok_lognot:
        E = expr_parse_unary(l);
        break;
    default:
        break;
    }

    return E;
}

static expr_t *expr_parse_led(lexer_t *l, int min_bp)
{
    expr_t *E = expr_parse_nud(l);
    expr_bp bp = bp_lookup(l->tok.kind);

    while (min_bp < bp.left) {
        if (l->tok.kind == tok_ternary_if)
            E = expr_parse_ternary(l, E);
        else
            E = expr_parse_binary(l, E, bp.right);
        bp = bp_lookup(l->tok.kind);
    }

    return E;
}

expr_t *expr_with_len(const char *input, size_t len)
{
    token_t t = {NULL,0,1,tok_unknown};
    lexer_t l = {t, input, input+len};
    lexer_next(&l);
    return expr_parse_led(&l, 0);
}

expr_t *expr(const char *input)
{
    return expr_with_len(input, strlen(input));
}

// === REPL =================================================
// ==========================================================

#define unlikely(x) (__builtin_expect(!!(x), 0))

static size_t getinput(char *in, size_t n, FILE *fp)
{
    int ch;
    unsigned char *p;

    if (unlikely(in == NULL) || unlikely(fp == NULL)) {
        return 0;
    } else if (unlikely(n == 0)) {
        return 0;
    } else if (unlikely(n == 1)) {
        *in = '\0';
        return 0;
    }

    p = (unsigned char *)in;
    n -= 1; /* for '\0' */

    /**
     * note that character encoding other than ASCII is accepted here.
     * so make sure your buffer large enough to handles it otherwise it will
     * splitted and you will get garbage result.
     */
    while (n > 1 && (ch = getc_unlocked(fp)) != EOF && ch != '\0') {
        *p++ = (unsigned char)ch; /* integer promotions are annoying */
        if (ch == '\n')
            break;
        n--;
    }

    if (n == 1) /* truncated or empty input or maybe done */
        *p++ = '\n';

    *p = '\0';
    return (size_t)(p - (unsigned char *)in); /* always > 0 */
}

static void repl(void)
{
    char input[256];

    while (1) {
        printf(">>> "); fflush(stdout);
        size_t nread = getinput(input, sizeof(input), stdin);
        if (nread == 0) {
            putchar('\n');
            break;
        } else if (nread == 1) {
            continue;
        }
        expr_t *E = expr_with_len(input, nread);
        if (E == NULL) {
            printf("invalid expression: %s", input);
        } else {
            int64_t result = expr_eval(E);
            printf("%ld\n", result);
            expr_free(E);
        }
    }
}

int main(void)
{
    repl();
}
