/*
 * Grammar
 * -------
 *  prog   ->  <list>;
 *  list   ->  "(" <exp> ")";
 *  exp    ->  <op> <foo> <foo>;
 *  op     ->  "+" | "-" | "*" | "/" | "^" | "%" | "=" | "~";
 *  foo    ->  <dig> | <list>;
 *  dig    ->  [ "-" ] ( "0" | "1-9" [ { "0-9" } ] );
 *
 *
 * Example of left-derivation
 * --------------------------
 *
 *  1. (+ 1 2)
 *  prog    ->  <list>;
 *          ->  (  <exp>  );
 *          ->  (  <op> <foo> <foo>  );
 *          ->  (   +   <foo> <foo>  );
 *          ->  (   +   <dig> <foo>  );
 *          ->  (   +     1   <foo>  );
 *          ->  (   +     1   <dig>  );
 *          ->  (   +     1     2    );
 *
 *  2. (- -1 (+ 2 1))
 *  prog    ->  <list>;
 *          ->  (  <exp>  );
 *          ->  (  <op> <foo> <foo>  );
 *          ->  (   -   <foo> <foo>  );
 *          ->  (   -   <dig> <foo>  );
 *          ->  (   -    -1   <foo>  );
 *          ->  (   -    -1   <list> );
 *          ->  (   -    -1   ( <exp> ) );
 *          ->  (   -    -1   ( <op> <foo> <foo> ) );
 *          ->  (   -    -1   (  +   <foo> <foo> ) );
 *          ->  (   -    -1   (  +   <dig> <foo> ) );
 *          ->  (   -    -1   (  +     2   <foo> ) );
 *          ->  (   -    -1   (  +     2   <dig> ) );
 *          ->  (   -    -1   (  +     2     1   ) );
 *
 *  3. (* (+ (- 10 2) 8) (- 2 (+ -1 -1)))
 *  Too long
 */

#include <ctype.h>
#include <errno.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#if DEBUG
#define debug_print(s, ...)  fprintf(stderr, s, __VA_ARGS__)
#else
#define debug_print(s, ...)
#endif

#define T_INVALID -1
#define T_LPAREN   0
#define T_RPAREN   1
#define T_DIGIT    2
#define T_ADD      3
#define T_SUB      4
#define T_MUL      5
#define T_DIV      6
#define T_POW      7
#define T_EQU      8
#define T_NEQ      9
#define T_MOD      10
#define T_END      255

static const uint8_t highest_bit_set[] = {
    0, 1, 2, 2, 3, 3, 3, 3,
    4, 4, 4, 4, 4, 4, 4, 4,
    5, 5, 5, 5, 5, 5, 5, 5,
    5, 5, 5, 5, 5, 5, 5, 5,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 6,
    6, 6, 6, 6, 6, 6, 6, 255, // anything past 63 is a guaranteed overflow with base > 1
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
    255, 255, 255, 255, 255, 255, 255, 255,
};

/* From https://gist.github.com/orlp/3551590 */
static int64_t ipow(int64_t base, uint8_t exp)
{
    int64_t result = 1;

    switch (highest_bit_set[exp]) {
    case 255: // we use 255 as an overflow marker and return 0 on overflow/underflow
        if (base == 1)
            return 1;
        if (base == -1)
            return 1 - 2 * (exp & 1);
        return 0;
    case 6:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
        /* FALLTHROUGH */
    case 5:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
        /* FALLTHROUGH */
    case 4:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
        /* FALLTHROUGH */
    case 3:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
        /* FALLTHROUGH */
    case 2:
        if (exp & 1) result *= base;
        exp >>= 1;
        base *= base;
        /* FALLTHROUGH */
    case 1:
        if (exp & 1) result *= base;
        /* FALLTHROUGH */
    default:
        return result;
    }
}

struct lexstate {
    const char *cur;
    int64_t v;
    int tok;
};

static void parse_dig(struct lexstate *ls)
{
    char *end = NULL;
    errno = 0;
    ls->v = strtol(ls->cur, &end, 10);
    if (errno == 0 && end != (char *)ls->cur) {
        ls->cur = end;
        ls->tok = T_DIGIT;
    }
}

static void lex(struct lexstate *ls)
{
    ls->tok = T_INVALID;

    while (ls->cur[0] == ' ')
        ls->cur++;

    switch (ls->cur[0]) {
    case '(':
        ls->tok = T_LPAREN;
        break;
    case ')':
        ls->tok = T_RPAREN;
        break;
    case '+':
        ls->tok = T_ADD;
        break;
    case '-':
        if (isdigit(ls->cur[1])) {
            parse_dig(ls);
            return;
        } else {
            ls->tok = T_SUB;;
        }
        break;
    case '*':
        ls->tok = T_MUL;
        break;
    case '/':
        ls->tok = T_DIV;
        break;
    case '^':
        ls->tok = T_POW;
        break;
    case '%':
        ls->tok = T_MOD;
        break;
    case '=':
        ls->tok = T_EQU;
        break;
    case '~':
        ls->tok = T_NEQ;
        break;
    case '0': case '1': case '2': case '3': case '4':
    case '5': case '6': case '7': case '8': case '9':
        parse_dig(ls);
        return;
    case '\0':
        ls->tok = T_END;
        break;
    }

    ls->cur++;
}

static int valid_ops(int tok)
{
    return tok == T_ADD || tok == T_SUB || tok == T_MUL || tok == T_DIV ||
           tok == T_POW || tok == T_MOD || tok == T_EQU || tok == T_NEQ;
}

static const char *tok2str(int tok)
{
    switch (tok) {
    case T_LPAREN:
        return "(";
    case T_RPAREN:
        return ")";
    case T_ADD:
        return "+";
    case T_SUB:
        return "-";
    case T_MUL:
        return "*";
    case T_DIV:
        return "/";
    case T_POW:
        return "^";
    case T_MOD:
        return "%";
    case T_EQU:
        return "=";
    case T_NEQ:
        return "~";
    case T_DIGIT:
        return "<digit>";
    case T_END:
        return "<eof>";
    }

    return "invalid";
}

static int64_t parse_list(struct lexstate *ls, int indent);

static int64_t parse_exp(struct lexstate *ls, int indent)
{
    if (!valid_ops(ls->tok)) {
        printf("%*sinvalid operator: %s\n", indent, " ", tok2str(ls->tok));
        exit(1);
    }

    int64_t lhs = 0, rhs = 0;
    int op = ls->tok;

    debug_print("%*soperator is: %s\n", indent, " ", tok2str(op));

    lex(ls);

    if (ls->tok == T_DIGIT) {
        debug_print("%*slhs is digit: %ld\n", indent, " ", ls->v);
        lhs = ls->v;
    } else if (ls->tok == T_LPAREN) {
        debug_print("%*slhs is list\n", indent, " ");
        lhs = parse_list(ls, indent + 2);
    } else {
        fprintf(stderr, "%*sinvalid lhs: %s\n", indent, " ", tok2str(ls->tok));
        exit(1);
    }

    lex(ls);

    if (ls->tok == T_DIGIT) {
        debug_print("%*srhs is digit: %ld\n", indent, " ", ls->v);
        rhs = ls->v;
    } else if (ls->tok == T_LPAREN) {
        debug_print("%*srhs is list\n", indent, " ");
        rhs = parse_list(ls, indent + 2);
    } else {
        fprintf(stderr, "%*sinvalid rhs: %s\n", indent, " ", tok2str(ls->tok));
        exit(1);
    }

    switch (op) {
    case T_ADD:
        return lhs + rhs;
    case T_SUB:
        return lhs - rhs;
    case T_MUL:
        return lhs * rhs;
    case T_DIV:
        if (rhs == 0) {
            fprintf(stderr, "%*sinteger division by zero\n", indent, " ");
            exit(1);
        }
        return lhs / rhs;
    case T_POW:
        if (rhs < 0) {
            fprintf(stderr, "%*snegative exponent\n", indent, " ");
            exit(1);
        } else if (rhs > 63) {
            fprintf(stderr, "%*sexponent too big: %ld\n", indent, " ", rhs);
            exit(1);
        }
        return ipow(lhs, (uint8_t)rhs);
    case T_MOD:
        if (rhs == 0) {
            fprintf(stderr, "%*sinteger modulo by zero\n", indent, " ");
            exit(1);
        }
        return lhs % rhs;
    case T_EQU:
        return lhs == rhs;
    case T_NEQ:
        return lhs != rhs;
    default:
        /* NOTREACHED */
        return 0;
    }
}

static int64_t parse_list(struct lexstate *ls, int indent)
{
    lex(ls);

    int64_t r = parse_exp(ls, indent);

    lex(ls);

    if (ls->tok != T_RPAREN) {
        fprintf(stderr, "%*sUnterminated list: %s\n", indent, " ", tok2str(ls->tok));
        exit(1);
    }

    return r;
}

static int64_t parse_prog(struct lexstate *ls)
{
    lex(ls);

    if (ls->tok != T_LPAREN) {
        fprintf(stderr, "'(' is expected, but found %s\n", tok2str(ls->tok));
        exit(1);
    }

    return parse_list(ls, 0);
}

int main(int argc, char **argv)
{
    const char *in = (argc < 2) ? "(* 1 2)" : argv[1];
    printf("INPUT =  %s\n\n", in);

    struct lexstate ls;
    ls.cur = in;
    ls.v = 0;

    int64_t r = parse_prog(&ls);
    printf("\nResult = %ld\n", r);

    puts("OK");
}
