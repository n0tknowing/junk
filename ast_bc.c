// AST -> Bytecode version
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

enum ASTKind {
    A_NUMBER,
    A_BINARY,
    A_UNARY,
    A_TERNARY,
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
    default:
        return "<AST_UNKNOWN>";
    }
}

static struct ASTExpr *expr_new(enum ASTKind kind)
{
    struct ASTExpr *expr = calloc(1, sizeof(*expr));
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
    default:
        break;
    }

    free(expr);
}

static void expr_emit_body(struct ASTExpr *exp);

static void expr_binary_emit(struct ASTExpr *exp)
{
    expr_emit_body(exp->as.binary.lhs);
    expr_emit_body(exp->as.binary.rhs);

    switch (exp->as.binary.kind) {
    case T_PLUS:
        printf("  ADD\n");
        break;
    case T_MINUS:
        printf("  SUB\n");
        break;
    case T_STAR:
        printf("  MUL\n");
        break;
    case T_SLASH:
        printf("  DIV\n");
        break;
    case T_PERCENT:
        printf("  MOD\n");
        break;
    default:
        printf("Invalid binary operator\n");
        exit(1);
    }
}

static void expr_unary_emit(struct ASTExpr *exp)
{
    expr_emit_body(exp->as.unary.exp);

    switch (exp->as.unary.kind) {
    case T_PLUS:
        break;
    case T_MINUS:
        printf("  NEG\n");
        break;
    case T_TILDE:
        printf("  BNOT\n");
        break;
    default:
        printf("Invalid unary operator\n");
        exit(1);
    }
}

static void expr_ternary_emit(struct ASTExpr *exp)
{
    expr_emit_body(exp->as.ternary.cond);
    printf("  JUMP_IF_ZERO A0\n");
    expr_emit_body(exp->as.ternary.if_true);
    printf("  JUMP A1\n");
    printf(" A0:\n");
    expr_emit_body(exp->as.ternary.if_false);
    printf(" A1:\n");
}

static void expr_emit_body(struct ASTExpr *exp)
{
    if (exp == NULL)
        return;

    switch (exp->kind) {
    case A_NUMBER:
        printf("  PUSH %ld\n", exp->as.number);
        break;
    case A_BINARY:
        expr_binary_emit(exp);
        break;
    case A_UNARY:
        expr_unary_emit(exp);
        break;
    case A_TERNARY:
        expr_ternary_emit(exp);
        break;
    default:
        printf("%s is not an expression\n", expr2str(exp->kind));
        exit(1);
    }
}

void expr_emit(struct ASTExpr *exp)
{
    expr_emit_body(exp);
    printf("  HALT\n");
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
    input = "~-1 + 2 * (3 % 2)";
#else
    // 1 + 2 ? 9 + 10 : -2
    exp = expr_ternary(expr_binary(T_PLUS, expr_number(1), expr_number(2)), // cond
            expr_binary(T_PLUS, expr_number(9), expr_number(10)), // if_true
            expr_unary(T_MINUS, expr_number(2)) // if_false
            );
    input = "1 + 2 ? 9 + 10 : -2";
#endif
    if (exp != NULL) {
        printf("input    = %s\n", input);
        printf("bytecode = \n");
        expr_emit(exp);
        expr_free(exp);
        rc = 0;
    }

    return rc;
}
