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

enum ASTKind {
    A_BINARY = 0,
    A_UNARY,
    A_NUMBER,
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
        int64_t number;
    } as;
};

const char *expr2str(enum ASTKind kind)
{
    switch (kind) {
    case A_BINARY:
        return "<AST_BINARY>";
    case A_UNARY:
        return "<AST_UNARY>";
    case A_NUMBER:
        return "<AST_NUMBER>";
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
    default:
        printf("%s is not an expression\n", expr2str(exp->kind));
        exit(1);
    }
}

// Constant folding.
//
// In each expr_*_fold() function, the function may recursively call
// expr_fold() to rewrite the AST node if the node is nested.
void expr_fold(struct ASTExpr *exp);

static void expr_binary_fold(struct ASTExpr *exp)
{
    if (exp == NULL)
        return;

    struct ASTExpr *lhs = exp->as.binary.lhs;
    if (lhs->kind == A_UNARY) // try fold this unary operator
        expr_fold(lhs);
    else if (lhs->kind == A_BINARY) // nested binary operators
        expr_fold(lhs);

    if (lhs->kind != A_NUMBER) // not a constant?
        return;

    struct ASTExpr *rhs = exp->as.binary.rhs;
    if (rhs->kind == A_UNARY) // try fold this unary operator
        expr_fold(rhs);
    else if (rhs->kind == A_BINARY) // nested binary operators
        expr_fold(rhs);

    if (rhs->kind != A_NUMBER) // not a constant?
        return;

    exp->kind = A_NUMBER;
    exp->as.number = expr_binary_eval(exp);

    free(rhs);
    free(lhs);
}

static void expr_unary_fold(struct ASTExpr *exp)
{
    if (exp == NULL)
        return;

    struct ASTExpr *uexpr = exp->as.unary.exp;
    if (uexpr->kind == A_UNARY) // check for nested unary operators
        expr_fold(uexpr);
    else if (uexpr->kind == A_BINARY) // -(1 + 2) is unary with binary...
        expr_fold(uexpr);             // ...operator as its operand

    if (uexpr->kind != A_NUMBER) // not a constant?
        return;

    exp->kind = A_NUMBER;
    exp->as.number = expr_unary_eval(exp);

    free(uexpr);
}

void expr_fold(struct ASTExpr *exp)
{
    if (exp == NULL)
        return;

    switch (exp->kind) {
    case A_BINARY:
        expr_binary_fold(exp);
        break;
    case A_UNARY:
        expr_unary_fold(exp);
        break;
    default:
        break;
    }
}

static int gv_helper(struct ASTExpr *exp, int parent, int id)
{
    if (exp == NULL)
        return id;

    int node_id = id++;
    char data[256] = {0};

    switch (exp->kind) {
    case A_NUMBER:
        snprintf(data, sizeof(data), "%ld", exp->as.number);
        break;
    case A_BINARY:
        switch (exp->as.binary.kind) {
        case T_PLUS:
            data[0] = '+';
            break;
        case T_MINUS:
            data[0] = '-';
            break;
        case T_STAR:
            data[0] = '*';
            break;
        case T_SLASH:
            data[0] = '/';
            break;
        case T_PERCENT:
            data[0] = '%';
            break;
        default:
            snprintf(data, sizeof(data), "???");
            break;
        }
        break;
    case A_UNARY:
        switch (exp->as.unary.kind) {
        case T_PLUS:
            data[0] = '+';
            break;
        case T_MINUS:
            data[0] = '-';
            break;
        case T_TILDE:
            data[0] = '~';
            break;
        default:
            snprintf(data, sizeof(data), "???");
            break;
        }
        break;
    default:
        snprintf(data, sizeof(data), "???");
        break;
    }

    printf("    node%d [label=\"%s\"];\n", node_id, data);
    if (parent >= 0)
        printf("    node%d -- node%d\n", parent, node_id);

    if (exp->kind == A_BINARY) {
        id = gv_helper(exp->as.binary.lhs, node_id, id);
        return gv_helper(exp->as.binary.rhs, node_id, id);
    } else if (exp->kind == A_UNARY) {
        return gv_helper(exp->as.unary.exp, node_id, id);
    } else {
        return id;
    }
}

int expr_draw(struct ASTExpr *exp)
{
    printf("graph ast {\n");
    int rc = gv_helper(exp, -1, 0);
    printf("}\n");
    return rc;
}

int main(void)
{
    struct ASTExpr *exp;
    int rc = 1;

    // ~-1 + 2 * (3 % 2)
    exp = expr_binary(T_PLUS,
                      expr_unary(T_TILDE, expr_unary(T_MINUS, expr_number(1))),
                      expr_binary(T_STAR,
                                  expr_number(2),
                                  expr_binary(T_PERCENT,
                                              expr_number(3),
                                              expr_number(2))));
    if (exp != NULL) {
        expr_fold(exp);
        printf("input  = ~-1 + 2 * (3 %% 2)\n");
        int64_t result = expr_eval(exp);
        printf("output = %ld\n", result);
        expr_draw(exp);
        expr_free(exp);
        rc = 0;
    }

    return rc;
}
