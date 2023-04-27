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


#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    token_kind_t kind;
    const char *cur;
    const char *end;
} lexer_t;

void lexer_next(lexer_t *l)
{
    while (l->cur < l->end && isspace(l->cur[0]))
        l->cur++;

    if (l->cur >= l->end) {
        l->kind = tok_eof;
        return;
    }

    intptr_t off = 1;

    switch (l->cur[0]) {
    case 'a':
        if (strncmp(l->cur, "and", 3) == 0) {
            off = 3;
            l->kind = tok_logand;
        }
        break;
    case 'n':
        if (strncmp(l->cur, "not", 3) == 0) {
            off = 3;
            l->kind = tok_lognot;
        }
        break;
    case 'o':
        if (strncmp(l->cur, "or", 2) == 0) {
            off = 2;
            l->kind = tok_logor;
        }
        break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        l->kind = tok_number;
        off = 0;
        break;
    case '(':
        l->kind = tok_lparen;
        break;
    case ')':
        l->kind = tok_rparen;
        break;
    case '+':
        l->kind = tok_plus;
        break;
    case '-':
        l->kind = tok_minus;
        break;
    case '*':
        l->kind = tok_star;
        break;
    case '/':
        l->kind = tok_slash;
        break;
    case '%':
        l->kind = tok_percent;
        break;
    case '?':
        l->kind = tok_ternary_if;
        break;
    case ':':
        l->kind = tok_ternary_else;
        break;
    case '&':
        l->kind = tok_bitand;
        break;
    case '^':
        l->kind = tok_bitxor;
        break;
    case '|':
        l->kind = tok_bitor;
        break;
    case '~':
        l->kind = tok_bitnot;
        break;
    case '>':
        if (l->cur[1] == '>') {
            l->kind = tok_rshift;
            off = 2;
        } else if (l->cur[1] == '=') {
            l->kind = tok_ge;
            off = 2;
        } else {
            l->kind = tok_gt;
        }
        break;
    case '<':
        if (l->cur[1] == '<') {
            l->kind = tok_lshift;
            off = 2;
        } else if (l->cur[1] == '=') {
            l->kind = tok_le;
            off = 2;
        } else {
            l->kind = tok_lt;
        }
        break;
    case '=':
        if (l->cur[1] == '=') {
            l->kind = tok_eq;
            off = 2;
        }
        break;
    case '!':
        if (l->cur[1] == '=') {
            l->kind = tok_ne;
            off = 2;
        }
        break;
    default:
        l->kind = tok_unknown;
        break;
    }

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
        int64_t literal;
    };
};

expr_t *expr_new(expr_kind_t kind)
{
    expr_t *E = calloc(1, sizeof(*E));
    assert(E != NULL);
    E->kind = kind;
    return E;
}

expr_t *expr_literal(int64_t v)
{
    expr_t *E = expr_new(literal_kind);
    E->literal = v;
    return E;
}

expr_t *expr_unary(token_kind_t kind, expr_t *operand)
{
    expr_t *E = expr_new(unary_kind);
    E->unary.operator = kind;
    E->unary.operand = operand;
    return E;
}

expr_t *expr_binary(token_kind_t kind, expr_t *lhs, expr_t *rhs)
{
    expr_t *E = expr_new(binary_kind);
    E->binary.operator = kind;
    E->binary.lhs = lhs;
    E->binary.rhs = rhs;
    return E;
}

expr_t *expr_ternary(expr_t *cond, expr_t *vit, expr_t *vif)
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
            fprintf(stderr, "bitwise shitfts only work on non-negative "
                             "integer\n");
            exit(1);
        } else if (rhs > 63) {
            return 0;
        }
        return lhs << rhs;
    case tok_rshift:
        if (lhs < 0 || rhs < 0) {
            fprintf(stderr, "bitwise shitfts only work on non-negative "
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

// === Parser ===============================================
// ==========================================================

typedef struct {
    int left, right;
} expr_bp;

#define bp_left_assoc(bp)   (expr_bp){(bp)-1, (bp)}
#define bp_right_assoc(bp)  (expr_bp){(bp)+1, (bp)}
#define bp_unary  500

expr_bp bp_lookup(token_kind_t tok)
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

expr_t *expr_parse_led(lexer_t *l, int min_bp);

expr_t *expr_parse_subexpr(lexer_t *l)
{
    lexer_next(l);

    expr_t *E = expr_parse_led(l, 0);

    if (E == NULL) {
        fprintf(stderr, "empty subexpression\n");
        exit(1);
    } else if (l->kind == tok_eof) {
        fprintf(stderr, "unexpected EOF while parsing subexpression\n");
        exit(1);
    } else if (l->kind != tok_rparen) {
        fprintf(stderr, "unterminated subexpression, missing ')'\n");
        exit(1);
    }

    lexer_next(l);
    return E;
}

expr_t *expr_parse_number(lexer_t *l)
{
    expr_t *E = NULL;
    size_t len = 0;
    char tmp[24] = {0};
    char *start = (char *)l->cur;

    do {
        len++;
        l->cur++;
    } while (l->cur < l->end && isdigit(l->cur[0]));

    if (len > 23) {
        fprintf(stderr, "number too big\n");
        exit(1);
    }

    memcpy(tmp, start, len);

    errno = 0;
    char *end = NULL;
    int64_t v = strtol(tmp, &end, 10);
    if (errno == 0 && end != start) {
        E = expr_literal(v);
    } else {
        fprintf(stderr, "number too big\n");
        exit(1);
    }

    lexer_next(l);
    return E;
}

expr_t *expr_parse_binary(lexer_t *l, expr_t *lhs, int rbp)
{
    if (lhs == NULL) {
        fprintf(stderr, "missing LHS while parsing binary expresion\n");
        exit(1);
    }

    token_kind_t op = l->kind;
    lexer_next(l);
    expr_t *rhs = expr_parse_led(l, rbp);
    if (rhs == NULL) {
        fprintf(stderr, "missing RHS while parsing binary expression\n");
        exit(1);
    }
    return expr_binary(op, lhs, rhs);
}

expr_t *expr_parse_unary(lexer_t *l)
{
    token_kind_t kind = l->kind;
    lexer_next(l);

    expr_t *operand = expr_parse_led(l, bp_unary);
    if (operand == NULL) {
        fprintf(stderr, "missing operand while parsing unary expression\n");
        exit(1);
    }

    return expr_unary(kind, operand);
}

expr_t *expr_parse_ternary(lexer_t *l, expr_t *cond)
{
    if (cond == NULL) {
        fprintf(stderr, "missing first operand while parsing ternary "
                        "conditional expression\n");
        exit(1);
    }

    lexer_next(l);

    expr_t *vit = expr_parse_led(l, 0);
    if (vit == NULL) {
        fprintf(stderr, "missing second operand while parsing ternary "
                        "conditional expression\n");
        exit(1);
    } else if (l->kind != tok_ternary_else) {
        fprintf(stderr, "missing ':' after parsed second operand of ternary "
                        "conditional expression\n");
        exit(1);
    }

    lexer_next(l);

    expr_t *vif = expr_parse_led(l, 0);
    if (vif == NULL) {
        fprintf(stderr, "missing third operand while parsing ternary "
                        "conditional expression\n");
        exit(1);
    }

    return expr_ternary(cond, vit, vif);
}

expr_t *expr_parse_nud(lexer_t *l)
{
    expr_t *E = NULL;

    switch (l->kind) {
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

expr_t *expr_parse_led(lexer_t *l, int min_bp)
{
    expr_t *E = expr_parse_nud(l);
    expr_bp bp = bp_lookup(l->kind);

    while (min_bp < bp.left) {
        if (l->kind == tok_ternary_if)
            E = expr_parse_ternary(l, E);
        else
            E = expr_parse_binary(l, E, bp.right);
        bp = bp_lookup(l->kind);
    }

    return E;
}

expr_t *expr_with_len(const char *input, size_t len)
{
    lexer_t l = {tok_unknown, input, input+len};
    lexer_next(&l);
    return expr_parse_led(&l, 0);
}

expr_t *expr(const char *input)
{
    return expr_with_len(input, strlen(input));
}

// === Test =================================================
// ==========================================================

int main(int argc, char **argv)
{
    assert(argc > 1); // lazy edition
    expr_t *E = expr(argv[1]);
    if (E != NULL) {
        int64_t result = expr_eval(E);
        printf("%s = %ld\n", argv[1], result);
        expr_free(E);
    } else {
        fprintf(stderr, "invalid expression: %s\n", argv[1]);
    }
}
