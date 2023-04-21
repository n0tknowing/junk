//  Operator          Associativity       Description
//  --------          -------------       -----------
//  x!                right to left       Factorial
//  +x -x ~x !x       right to left       Positive, negative, bitwise NOT, logical NOT
//  * / %             left to right       Multiplication, division, modulo
//  + -               left to right       Addition, subtraction
//  << >>             left to right       Bitwise shifts
//  &                 left to right       Bitwise AND
//  ^                 left to right       Bitwise XOR
//  |                 left to right       Bitwise OR
//  == != < > <= >=   left to right       Relational operators
//  &&                left to right       Logical AND
//  ||                left to right       Logical OR

#include <cassert>
#include <cctype>
#include <climits>
#include <charconv>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <utility>

enum class TokenKind {
    T_EOF       = 0,

    T_NUMBER,   T_LPAREN,   T_RPAREN,   T_PLUS,     T_MINUS,
    T_MUL,      T_DIV,      T_MOD,      T_BITAND,   T_BITXOR,
    T_BITOR,    T_BITNOT,   T_LT,       T_GT,       T_EQ,
    T_NE,       T_LE,       T_GE,       T_LOGAND,   T_LOGOR,
    T_LSHIFT,   T_RSHIFT,   T_BANG,

    T_UNKNOWN   = 256,
};

struct Token {
    const char *start;
    const char *end;
    TokenKind kind;
};

struct Lexer {
    Lexer(const char *in) : tok{}, ptr_off(0), cur(in) {}

    TokenKind token_kind() const {
        return tok.kind;
    }

    Token token() {
        return tok;
    }

    void next() {
        while (isspace(*cur)) { 
            cur++;
        }

        tok.start = cur;
        ptr_off = 1;

        switch (*cur) {
        case '0': case '1': case '2': case '3': case '4':
        case '5': case '6': case '7': case '8': case '9':
            while (*cur != '\0') {
                if (!isdigit(*cur)) { break; }
                cur++;
            }
            ptr_off = 0;
            tok.kind = TokenKind::T_NUMBER;
            break;
        case '(':
            tok.kind = TokenKind::T_LPAREN;
            break;
        case ')':
            tok.kind = TokenKind::T_RPAREN;
            break;
        case '+':
            tok.kind = TokenKind::T_PLUS;
            break;
        case '-':
            tok.kind = TokenKind::T_MINUS;
            break;
        case '*':
            tok.kind = TokenKind::T_MUL;
            break;
        case '/':
            tok.kind = TokenKind::T_DIV;
            break;
        case '%':
            tok.kind = TokenKind::T_MOD;
            break;
        case '!':
            if (cur[1] == '=') {
                ptr_off = 2;
                tok.kind = TokenKind::T_NE;
            } else {
                tok.kind = TokenKind::T_BANG;
            }
            break;
        case '&':
            if (cur[1] == '&') {
                ptr_off = 2;
                tok.kind = TokenKind::T_LOGAND;
            } else {
                tok.kind = TokenKind::T_BITAND;
            }
            break;
        case '^':
            tok.kind = TokenKind::T_BITXOR;
            break;
        case '|':
            if (cur[1] == '|') {
                ptr_off = 2;
                tok.kind = TokenKind::T_LOGOR;
            } else {
                tok.kind = TokenKind::T_BITOR;
            }
            break;
        case '~':
            tok.kind = TokenKind::T_BITNOT;
            break;
        case '<':
            if (cur[1] == '=') {
                ptr_off = 2;
                tok.kind = TokenKind::T_LE;
            } else if (cur[1] == '<') {
                ptr_off = 2;
                tok.kind = TokenKind::T_LSHIFT;
            } else {
                tok.kind = TokenKind::T_LT;
            }
            break;
        case '>':
            if (cur[1] == '=') {
                ptr_off = 2;
                tok.kind = TokenKind::T_GE;
            } else if (cur[1] == '>') {
                ptr_off = 2;
                tok.kind = TokenKind::T_RSHIFT;
            } else {
                tok.kind = TokenKind::T_GT;
            }
            break;
        case '=':
            if (cur[1] == '=') {
                ptr_off = 2;
                tok.kind = TokenKind::T_EQ;
            }
            break;
        case '\0':
            tok.kind = TokenKind::T_EOF;
            break;
        default:
            tok.kind = TokenKind::T_UNKNOWN;
            break;
        }

        cur += ptr_off;
        tok.end = cur;

#if DEBUG
        int l = (int)(tok.end - tok.start);
        printf("Token  = %.*s\n", l, tok.start);
#endif
    }

private:
    Token tok;
    intptr_t ptr_off;
    const char *cur;
};

static inline std::pair<int, int> left_assoc(int bp) {
    return std::make_pair(bp - 1, bp);
}

static inline std::pair<int, int> right_assoc(int bp) {
    return std::make_pair(bp + 1, bp);
}

static int prefix_bp(TokenKind kind) {
    switch (kind) {
    case TokenKind::T_PLUS:
    case TokenKind::T_MINUS:
    case TokenKind::T_BITNOT:
    case TokenKind::T_BANG:
        return 400;
    default:
        return 0;
    }
}

static std::pair<int, int> lookup_bp(TokenKind kind) {
    switch (kind) {
    case TokenKind::T_BANG:
        return right_assoc(500);
    case TokenKind::T_MUL:
    case TokenKind::T_DIV:
    case TokenKind::T_MOD:
        return left_assoc(200);
    case TokenKind::T_PLUS:
    case TokenKind::T_MINUS:
        return left_assoc(100);
    case TokenKind::T_LSHIFT:
    case TokenKind::T_RSHIFT:
        return left_assoc(90);
    case TokenKind::T_BITAND:
        return left_assoc(80);
    case TokenKind::T_BITXOR:
        return left_assoc(70);
    case TokenKind::T_BITOR:
        return left_assoc(60);
    case TokenKind::T_LT:
    case TokenKind::T_GT:
    case TokenKind::T_LE:
    case TokenKind::T_GE:
    case TokenKind::T_EQ:
    case TokenKind::T_NE:
        return left_assoc(50);
    case TokenKind::T_LOGAND:
        return left_assoc(40);
    case TokenKind::T_LOGOR:
        return left_assoc(30);
    default:
        return std::make_pair(0, 0);
    }
}

struct Node {
    Node *left;
    Node *right;
    int64_t number;
    Token tok;
};

static Node *new_node(Node *left, Node *right, Token tok, int64_t v) {
    Node *node = new Node;

    node->left = left;
    node->right = right;
    node->tok = tok;
    node->number = v;
    return node;
}

static void node_free(Node *root) {
    if (root == nullptr)
        return;

    node_free(root->left);
    node_free(root->right);
    delete root;
}

static Node *new_prefix(Node *left, Token op) {
    return new_node(left, nullptr, op, 0);
}

static Node *new_infix(Node *left, Node *right, Token op) {
    return new_node(left, right, op, 0);
}

static Node *new_literal(Token val, int64_t v) {
    return new_node(nullptr, nullptr, val, v);
}

static Node *new_postfix(Node *right, Token op) {
    return new_node(nullptr, right, op, 0);
}

static void node_print(Node *root, int indent) {
    if (root == nullptr) {
        return;
    }

    node_print(root->left, indent + 4);
    node_print(root->right, indent + 4);
    int len = (int)(root->tok.end - root->tok.start);
    printf("%*s %.*s\n", indent, " ", len, root->tok.start);
}

static int64_t node_eval(Node *root) {
    int64_t lhs = INT64_MAX, rhs = INT64_MAX;
    
    if (root == nullptr) {
        return lhs;
    }

    lhs = node_eval(root->left);
    rhs = node_eval(root->right);

    Token tok = root->tok;
    switch (tok.kind) {
    case TokenKind::T_NUMBER:
        return root->number;
    case TokenKind::T_PLUS:
        if (rhs == INT64_MAX) {
            return 0 + lhs;
        }
        return lhs + rhs;
    case TokenKind::T_MINUS:
        if (rhs == INT64_MAX) {
            return 0 - lhs;
        }
        return lhs - rhs;
    case TokenKind::T_MUL:
        return lhs * rhs;
    case TokenKind::T_DIV:
        if (rhs == 0) {
            printf("Integer division by zero\n");
            exit(1);
        }
        return lhs / rhs;
    case TokenKind::T_MOD:
        if (rhs == 0) {
            printf("Integer division by zero\n");
            exit(1);
        }
        return lhs % rhs;
    case TokenKind::T_BITAND:
        return lhs & rhs;
    case TokenKind::T_BITXOR:
        return lhs ^ rhs;
    case TokenKind::T_BITOR:
        return lhs | rhs;
    case TokenKind::T_BITNOT:
        return ~lhs;
    case TokenKind::T_LT:
        return lhs < rhs;
    case TokenKind::T_GT:
        return lhs > rhs;
    case TokenKind::T_EQ:
        return lhs == rhs;
    case TokenKind::T_NE:
        return lhs != rhs;
    case TokenKind::T_LE:
        return lhs <= rhs;
    case TokenKind::T_GE:
        return lhs >= rhs;
    case TokenKind::T_BANG:
        if (lhs == INT64_MAX && rhs != INT64_MAX) {
            if (rhs < 0) {
                return 0;
            } else if (rhs > 20) {
                printf("Factorial number too big\n");
                exit(1);
            } else {
                int64_t v = 1;
                for (int64_t i = 1; i <= rhs; i++) {
                    v *= i;
                }
                return v;
            }
        }
        return !lhs;
    case TokenKind::T_LOGAND:
        return lhs && rhs;
    case TokenKind::T_LOGOR:
        return lhs || rhs;
    case TokenKind::T_LSHIFT:
        if (lhs < 0 || rhs < 0) {
            printf("Bitwise shifts only work on non-negative integer\n");
            exit(1);
        } else if (rhs > 63) {
            return 0;
        }
        return lhs << rhs;
    case TokenKind::T_RSHIFT:
        if (lhs < 0 || rhs < 0) {
            printf("Bitwise shifts only work on non-negative integer\n");
            exit(1);
        } else if (lhs > 63) {
            return 0;
        }
        return lhs >> rhs;
    default:
        exit(0);
        break;
    }
}

static Node *expr_bp(Lexer& lex, int min_bp) {
    Node *lhs = nullptr, *rhs = nullptr;
    Token tok = lex.token();
    int64_t v = 0;

    // Parse primary or in Pratt term it is Null Denotation (nud).
    // It is consist of:
    // - Literal number
    // - Parentheses
    // - Prefix operators (+, -, ~, !)
    switch (tok.kind) {
    case TokenKind::T_NUMBER:
        std::from_chars(tok.start, tok.end, v, 10);
        lhs = new_literal(tok, v);
        lex.next(); // Skip last valid digit
        break;
    case TokenKind::T_LPAREN:
        lex.next(); // Skip '('
        lhs = expr_bp(lex, 0);
        if (lex.token_kind() == TokenKind::T_EOF) {
            fprintf(stderr, "Unexpected EOF\n");
            exit(1);
        } else if (lex.token_kind() != TokenKind::T_RPAREN) {
            fprintf(stderr, "Missing ')' in subexpression\n");
            exit(1);
        }
        lex.next(); // Skip ')'
        break;
    case TokenKind::T_PLUS:
    case TokenKind::T_MINUS:
    case TokenKind::T_BITNOT:
    case TokenKind::T_BANG:
        lex.next(); // Skip the operator
        rhs = expr_bp(lex, prefix_bp(tok.kind));
        if (rhs == nullptr) {
            printf("Expected expression after prefix operator\n");
            exit(1);
        }
        lhs = new_prefix(rhs, tok);
        break;
    case TokenKind::T_UNKNOWN:
        fprintf(stderr, "Unknown token: %.*s\n", (int)(tok.end - tok.start),
                                                 tok.start);
        exit(1);
        break;
    default:
        break;
    }

    tok = lex.token();
    std::pair<int, int> bp = lookup_bp(tok.kind);

    // Parse Left Denotation.
    // It is used to control binary expression including precedence which is
    // controlled by left binding power and associativity which is controlled
    // by right binding power.
    // For now, it only handles infix operators, but mixfix like ternary are
    // handled here as well.
    while (min_bp < bp.first) {
        lex.next(); // Skip current token
        if (tok.kind == TokenKind::T_BANG) {
            lhs = new_postfix(lhs, tok);
        } else {
            rhs = expr_bp(lex, bp.second);
            if (rhs == nullptr) {
                printf("Expected expression for RHS\n");
                exit(1);
            }
            lhs = new_infix(lhs, rhs, tok);
        }
        tok = lex.token();
        bp = lookup_bp(tok.kind);
    }

    return lhs;
}

static Node *expr(const char *in) {
    Lexer lex{in};
    lex.next();
    Node *result = expr_bp(lex, 0);
    if (lex.token_kind() != TokenKind::T_EOF) {
        printf("Expected EOF\n");
        exit(1);
    }
    return result;
}

int main(int argc, char **argv) {
#if 1
    const char *in = argc < 2 ? "1 + 2 * 3" : argv[1];
    Node *result = expr(in);
    int rc = 0;
    if (result != nullptr) {
        //node_print(result, 2);
        int64_t v = node_eval(result);
        printf("%ld\n", v);
        node_free(result);
    } else {
        printf("Expected expression\n");
        rc = 1;
    }
    return rc;
#else
    (void)argc, (void)argv;
#define SPAN_PTR(s) s, s+sizeof(s)-1
    Token t_op = {SPAN_PTR("+"), TokenKind::T_PLUS};
    Token t_lhs = {SPAN_PTR("10"), TokenKind::T_NUMBER};
    Token t_rhs = {SPAN_PTR("20"), TokenKind::T_NUMBER};
    Node *ast = new_infix(
                    new_literal(t_lhs, 10),
                    new_literal(t_rhs, 20),
                    t_op);
    node_print(ast, 2);
    printf("%ld\n", node_eval(ast));
    node_free(ast);
#endif
}
