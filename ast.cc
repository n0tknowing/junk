// https://en.wikipedia.org/wiki/Sethi%E2%80%93Ullman_algorithm
// ^-- codegen for arithmetic expression
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

enum TokenKind { // <-- This should belong to the lexer
    T_EOF = 0,

    T_PLUS,
    T_MINUS,
    T_STAR,
    T_SLASH,
    T_TILDE,
    T_PERCENT,

    T_INVALID = 512,
};

struct Identifier {
    const char *name;
    size_t length;
};

struct Identifier *identifier(const char *name, size_t len)
{
    struct Identifier *id = (struct Identifier *)calloc(1, sizeof(*id));
    if (id != NULL) {
        id->name = name; // dup?
        id->length = len;
    }
    return id;
}

enum ASTKind {
    A_NUMBER,
    A_BINARY,
    A_UNARY,
    A_TERNARY,
    A_ASSIGN,
};

// Sum type
struct ASTExpr {
    enum ASTKind kind; // variant
    union {
        struct {
            enum TokenKind kind;
            struct ASTExpr *lhs;
            struct ASTExpr *rhs;
        } binary;
        struct {
            enum TokenKind kind;
            struct ASTExpr *exp;
        } unary;
        struct {
            struct ASTExpr *cond;
            struct ASTExpr *if_true;
            struct ASTExpr *if_false;
        } ternary;
        struct {
            struct Identifier *lval;
            struct ASTExpr *rval;
        } assign;
        int64_t number;
    } as;
};

const char *expr2str(enum ASTKind kind)
{
    switch (kind) {
    case A_NUMBER:
        return "<AST_NUMBER>";
    case A_BINARY:
        return "<AST_BINARY>";
    case A_UNARY:
        return "<AST_UNARY>";
    case A_TERNARY:
        return "<AST_TERNARY>";
    case A_ASSIGN:
        return "<AST_ASSIGN>";
    default:
        return "<AST_UNKNOWN>";
    }
}

static struct ASTExpr *expr_new(enum ASTKind kind)
{
    struct ASTExpr *expr = (struct ASTExpr *)calloc(1, sizeof(*expr));
    if (expr != NULL)
        expr->kind = kind;
    return expr;
}

struct ASTExpr *expr_number(int64_t value)
{
    struct ASTExpr *expr = expr_new(A_NUMBER);
    if (expr != NULL)
        expr->as.number = value;
    return expr;
}

struct ASTExpr *expr_binary(enum TokenKind kind, struct ASTExpr *lhs,
                            struct ASTExpr *rhs)
{
    struct ASTExpr *expr = expr_new(A_BINARY);
    if (expr != NULL) {
        expr->as.binary.kind = kind;
        expr->as.binary.lhs = lhs;
        expr->as.binary.rhs = rhs;
    }
    return expr;
}

struct ASTExpr *expr_unary(enum TokenKind kind, struct ASTExpr *exp)
{
    struct ASTExpr *expr = expr_new(A_UNARY);
    if (expr != NULL) {
        expr->as.unary.kind = kind;
        expr->as.unary.exp = exp;
    }
    return expr;
}

struct ASTExpr *expr_ternary(struct ASTExpr *cond,
                             struct ASTExpr *if_true, struct ASTExpr *if_false)
{
    struct ASTExpr *expr = expr_new(A_TERNARY);
    if (expr != NULL) {
        expr->as.ternary.cond = cond;
        expr->as.ternary.if_true = if_true;
        expr->as.ternary.if_false = if_false;
    }
    return expr;
}

struct ASTExpr *expr_assign(struct Identifier *lval, struct ASTExpr *rval)
{
    struct ASTExpr *expr = expr_new(A_ASSIGN);
    if (expr != NULL) {
        expr->as.assign.lval = lval;
        expr->as.assign.rval = rval;
    }
    return expr;
}

void expr_free(struct ASTExpr *expr)
{
    if (expr == NULL)
        return;

    switch (expr->kind) {
    case A_BINARY:
        expr_free(expr->as.binary.lhs);
        expr_free(expr->as.binary.rhs);
        break;
    case A_UNARY:
        expr_free(expr->as.unary.exp);
        break;
    case A_TERNARY:
        expr_free(expr->as.ternary.cond);
        expr_free(expr->as.ternary.if_true);
        expr_free(expr->as.ternary.if_false);
        break;
    case A_ASSIGN:
        expr_free(expr->as.assign.rval);
        free(expr->as.assign.lval);
        break;
    default:
        break;
    }

    free(expr);
}

int64_t expr_eval(struct ASTExpr *exp);

static int64_t expr_binary_eval(struct ASTExpr *exp)
{
    int64_t lhs = expr_eval(exp->as.binary.lhs);
    int64_t rhs = expr_eval(exp->as.binary.rhs);

    switch (exp->as.binary.kind) {
    case T_PLUS:
        return lhs + rhs;
    case T_MINUS:
        return lhs - rhs;
    case T_STAR:
        return lhs * rhs;
    case T_SLASH:
        return lhs / rhs;
    case T_PERCENT:
        return lhs % rhs;
    default:
        printf("Invalid binary operator\n");
        exit(1);
    }
}

static int64_t expr_unary_eval(struct ASTExpr *exp)
{
    int64_t v = expr_eval(exp->as.unary.exp);

    switch (exp->as.unary.kind) {
    case T_PLUS:
        return 0 + v;
    case T_MINUS:
        return 0 - v;
    case T_TILDE:
        return ~v;
    default:
        printf("Invalid unary operator\n");
        exit(1);
    }
}

static int64_t expr_ternary_eval(struct ASTExpr *exp)
{
    int64_t cond = expr_eval(exp->as.ternary.cond);
    if (cond != 0)
        return expr_eval(exp->as.ternary.if_true);
    return expr_eval(exp->as.ternary.if_false);
}

static int64_t expr_assign_eval(struct ASTExpr *exp)
{
   return expr_eval(exp->as.assign.rval);
}

int64_t expr_eval(struct ASTExpr *exp)
{
    if (exp == NULL)
        return INT64_MIN;

    switch (exp->kind) {
    case A_NUMBER:
        return exp->as.number;
    case A_BINARY:
        return expr_binary_eval(exp);
    case A_UNARY:
        return expr_unary_eval(exp);
    case A_TERNARY:
        return expr_ternary_eval(exp);
    case A_ASSIGN:
        return expr_assign_eval(exp);
    default:
        printf("%s is not an expression\n", expr2str(exp->kind));
        exit(1);
    }
}

int main(void)
{
    struct ASTExpr *exp;
    const char *input;
    int rc = 1;

#if 0
    // ~-1 + 2 * (3 % 2)
    exp = expr_binary(T_PLUS,
                      expr_unary(T_TILDE, expr_unary(T_MINUS, expr_number(1))),
                      expr_binary(T_STAR,
                                  expr_number(2),
                                  expr_binary(T_PERCENT,
                                              expr_number(3),
                                              expr_number(2))));
    input = "~-1 + 2 * (3 %% 2)";
#else
    struct ASTExpr *exp2;
    const char *input2;
    // x = 1 + 2 ? 9 + 10 : -2
    exp = expr_assign(identifier("x", 1),
            expr_ternary(expr_binary(T_PLUS, expr_number(1), expr_number(2)), // cond
            expr_binary(T_PLUS, expr_number(9), expr_number(10)), // if_true
            expr_unary(T_MINUS, expr_number(2)) // if_false
            ));
    input = "x = 1 + 2 ? 9 + 10 : -2";
#endif
    if (exp != NULL) {
        // y = x + 2 * 8
        exp2 = expr_binary(T_PLUS, exp,
                expr_binary(T_STAR, expr_number(2), expr_number(8)));
        if (exp2 == NULL) {
            printf(":-(\n");
            expr_free(exp);
            return 1;
        }
        input2 = "y = x + 2 * 8";
        printf("input  = %s\n", input);
        printf("input2 = %s\n", input2);
        int64_t result = expr_eval(exp2);
        printf("output = %ld\n", result);
        expr_free(exp2);
        expr_free(exp);
        rc = 0;
    }

    return rc;
}
