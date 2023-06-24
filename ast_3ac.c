#include <assert.h>
#include <limits.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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
    case A_NUMBER:
        return "<AST_NUMBER>";
    case A_BINARY:
        return "<AST_BINARY>";
    case A_UNARY:
        return "<AST_UNARY>";
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

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

enum IRVarKind {
    IR_KIND_IMM,
    IR_KIND_VAR,
};

struct IRVar {
    enum IRVarKind kind;
    char var[32];
    int64_t imm;
};

enum IROpKind {
    IR_OP_INVALID,
    IR_OP_ADD,
    IR_OP_SUB,
    IR_OP_MUL,
    IR_OP_DIV,
    IR_OP_MOD,
    IR_OP_NEG,
    IR_OP_NOT,
    IR_OP_MOV,
};

struct IR {
    enum IROpKind op;
    struct IRVar dst;
    struct IRVar arg[2];
    struct IR *next;
    struct IR *prev;
};

struct IRList {
    int n_ir;
    struct IR *head;
    struct IR *tail;
};

void irlist_init(struct IRList *irl)
{
    irl->n_ir = 0;
    irl->head = NULL;
    irl->tail = NULL;
}

void irlist_add(struct IRList *irl, struct IR *ir)
{
    struct IR *new_ir;

    new_ir = calloc(1, sizeof(struct IR));
    assert(new_ir != NULL);

    memcpy(new_ir, ir, sizeof(struct IR));
    new_ir->next = NULL;
    new_ir->prev = irl->tail;

    if (irl->n_ir == 0)
        irl->head = new_ir;
    else
        irl->tail->next = new_ir;

    irl->tail = new_ir;
    irl->n_ir++;
}

void irlist_free(struct IRList *irl)
{
    struct IR *next;

    while (irl->head != NULL) {
        next = irl->head->next;
        free(irl->head);
        irl->head = next;
    }

    irl->head = NULL;
    irl->tail = NULL;
    irl->n_ir = 0;
}

//////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////

static void expr_gen(struct IRList *irl, struct ASTExpr *exp);

static void expr_gen_number(struct IRList *irl, struct ASTExpr *exp)
{
    struct IR ir;

    ir.op = IR_OP_MOV;
    ir.dst.kind = IR_KIND_VAR;
    snprintf(ir.dst.var, sizeof(ir.dst.var), "_t%d", irl->n_ir);

    ir.arg[0].kind = IR_KIND_IMM;
    ir.arg[0].imm = exp->as.number;

    irlist_add(irl, &ir);
}

static enum IROpKind binop2irop(enum TokenKind kind)
{
    switch (kind) {
    case T_PLUS:
        return IR_OP_ADD;
    case T_MINUS:
        return IR_OP_SUB;
    case T_STAR:
        return IR_OP_MUL;
    case T_SLASH:
        return IR_OP_DIV;
    case T_PERCENT:
        return IR_OP_MOD;
    default:
        assert(0);
    }
}

static void expr_gen_binary(struct IRList *irl, struct ASTExpr *exp)
{
    struct IR ir;

    expr_gen(irl, exp->as.binary.lhs);
    ir.arg[0] = irl->tail->dst;

    expr_gen(irl, exp->as.binary.rhs);
    ir.arg[1] = irl->tail->dst;

    ir.op = binop2irop(exp->as.binary.kind);
    ir.dst.kind = IR_KIND_VAR;
    snprintf(ir.dst.var, sizeof(ir.dst.var), "_t%d", irl->n_ir);

    irlist_add(irl, &ir);
}

static enum IROpKind unop2irop(enum TokenKind kind)
{
    switch (kind) {
    case T_MINUS:
        return IR_OP_NEG;
    case T_TILDE:
        return IR_OP_NOT;
    default:
        assert(0);
    }
}

static void expr_gen_unary(struct IRList *irl, struct ASTExpr *exp)
{
    struct IR ir;

    expr_gen(irl, exp->as.unary.exp);
    ir.arg[0] = irl->tail->dst;

    ir.op = unop2irop(exp->as.unary.kind);
    ir.dst.kind = IR_KIND_VAR;
    snprintf(ir.dst.var, sizeof(ir.dst.var), "_t%d", irl->n_ir);

    irlist_add(irl, &ir);
}

static void expr_gen(struct IRList *irl, struct ASTExpr *exp)
{
    switch (exp->kind) {
    case A_NUMBER:
        expr_gen_number(irl, exp);
        break;
    case A_BINARY:
        expr_gen_binary(irl, exp);
        break;
    case A_UNARY:
        expr_gen_unary(irl, exp);
        break;
    default:
        printf("%s is not an expression\n", expr2str(exp->kind));
        exit(1);
    }
}

void expr_gen_ir(struct IRList *irl, struct ASTExpr *exp)
{
    expr_gen(irl, exp);
}

const char *irop2str(enum IROpKind kind)
{
    switch (kind) {
    case IR_OP_ADD:
        return "ADD";
    case IR_OP_SUB:
        return "SUB";
    case IR_OP_MUL:
        return "MUL";
    case IR_OP_DIV:
        return "DIV";
    case IR_OP_MOD:
        return "MOD";
    case IR_OP_NEG:
        return "NEG";
    case IR_OP_NOT:
        return "NOT";
    case IR_OP_MOV:
        return "MOV";
    default:
        return "???";
    }
}

void expr_dump_ir(struct IRList *irl, FILE *out)
{
    struct IR *ir, *next;
    struct IRVar *arg0, *arg1;

    ir = irl->head;
    while (ir != NULL) {
        next = ir->next;
        arg0 = &ir->arg[0];
        arg1 = &ir->arg[1];
        // OP DST
        fprintf(out, "    %s %s, ", irop2str(ir->op), ir->dst.var);
        // ARG0
        if (arg0->kind == IR_KIND_IMM) fprintf(out, "%ld", arg0->imm);
        else fprintf(out, "%s", arg0->var);
        // ARG1
        switch (ir->op) {
        case IR_OP_ADD:
        case IR_OP_SUB:
        case IR_OP_MUL:
        case IR_OP_DIV:
        case IR_OP_MOD:
            if (arg1->kind == IR_KIND_IMM) fprintf(out, ", %ld", arg1->imm);
            else fprintf(out, ", %s", arg1->var);
            break;
        case IR_OP_MOV:
        default:
            break;
        }
        fprintf(out, "\n");
        ir = next;
    }
}

int main(void)
{
    struct ASTExpr *exp;
    struct IRList irl;
    const char *input;
    int rc = 1;

#if 1
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
    // 1 + -2
    exp = expr_binary(T_PLUS, expr_number(1),
                              expr_unary(T_MINUS, expr_number(2)));
    input = "1 + -2";
#endif
    if (exp != NULL) {
        irlist_init(&irl);
        printf("Input = %s\n", input);
        printf("TAC IR = \n");
        expr_gen_ir(&irl, exp);
        expr_dump_ir(&irl, stdout);
        irlist_free(&irl);
        expr_free(exp);
        rc = 0;
    }

    return rc;
}
