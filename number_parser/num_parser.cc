#include <climits>
#include <cstdlib>
#include <charconv>
#include <strings.h>
#include <system_error>

#include "num_parser.h"

#define is_digit(c)   (dtype_lut[(uint8_t)(c)] & (D_))
#define is_bdigit(c)  (dtype_lut[(uint8_t)(c)] & (B_))
#define is_odigit(c)  (dtype_lut[(uint8_t)(c)] & (O_))
#define is_xdigit(c)  (dtype_lut[(uint8_t)(c)] & (D_|X_))

#define D_   0x01
#define B_   0x02
#define O_   0x04
#define X_   0x08

#define DBO_ (D_|B_|O_)
#define DO_  (D_|O_)

static const uint8_t dtype_lut[256] = {
    0,     0,    0,    0,    0,    0,    0,    0,
    0,     0,    0,    0,    0,    0,    0,    0,
    0,     0,    0,    0,    0,    0,    0,    0,
    0,     0,    0,    0,    0,    0,    0,    0,
    0,     0,    0,    0,    0,    0,    0,    0,
    0,     0,    0,    0,    0,    0,    0,    0,
    DBO_,  DBO_, DO_,  DO_,  DO_,  DO_,  DO_,  DO_,
    D_,    D_,   0,    0,    0,    0,    0,    0,
    0,     X_,   X_,   X_,   X_,   X_,   X_,   0,
    0,     0,    0,    0,    0,    0,    0,    0,
    0,     0,    0,    0,    0,    0,    0,    0,
    0,     0,    0,    0,    0,    0,    0,    0,
    0,     X_,   X_,   X_,   X_,   X_,   X_,   0,
    0,     0,    0,    0,    0,    0,    0,    0,
    0,     0,    0,    0,    0,    0,    0,    0,
    0,     0,    0,    0,    0,    0,    0,    0,
};

const char *parse_number_err(result_status status)
{
    switch (status) {
    case result_status::OK:
        return "";
    case result_status::LZ:
        return "leading zero in base-10";
    case result_status::BL:
        return "invalid binary literal";
    case result_status::OL:
        return "invalid octal literal";
    case result_status::XL:
        return "invalid hex literal";
    case result_status::IL:
        return "invalid literal";
    case result_status::MI:
        return "no digit after minus sign";
    case result_status::ND:
        return "invalid digit";
    case result_status::FR:
        return "no digit after fractional part";
    case result_status::EX:
        return "no digit after exponent part";
    case result_status::OO:
        return "conversion result is out of range";
    }
    return "unknown error";
}

std::pair<result_status, result_num> parse_number(char *in)
{
    char *p = in;
    bool neg = false;
    bool is_f64 = false;
    int base = 10;
    char literal_char = ' ';
    result_num res = { .type = result_type::T_INV, .as_u64 = 0 };

    if (*p == '-') {
        neg = true;
        p++;
    }

    if (*p == '0') {
        p++;
        if (is_digit(*p)) {
            return std::make_pair(result_status::LZ, res);
        } else if (*p == 'b' || *p == 'B') {
            literal_char = *p;
            *p++ = '0'; /* leading zeros is ok here */
            if (!is_bdigit(*p))
                return std::make_pair(result_status::BL, res);
            do {
                p++;
            } while (is_bdigit(*p));
            base = 2;
            goto conv;
        } else if (*p == 'o' || *p == 'O') {
            literal_char = *p;
            *p++ = '0'; /* leading zeros is ok here */
            if (!is_odigit(*p))
                return std::make_pair(result_status::OL, res);
            do {
                p++;
            } while (is_odigit(*p));
            base = 8;
            goto conv;
        } else if (*p == 'x' || *p == 'X') {
            literal_char = *p;
            *p++ = '0'; /* leading zeros is ok here */
            if (!is_xdigit(*p))
                return std::make_pair(result_status::XL, res);
            do {
                p++;
            } while (is_xdigit(*p));
            base = 16;
            goto conv;
        } else {
            return std::make_pair(result_status::IL, res);
        }
    } else if (is_digit(*p)) {
        do {
            p++;
        } while (is_digit(*p));
    } else if (*p == '.') {
        goto parse_fraction;
    } else {
        if ((*p == 'n' || *p == 'N') && strncasecmp(p, "nan", 3) == 0) {
            p += 3;
            is_f64 = true;
            goto conv;
        } else if (*p == 'i' || *p == 'I') {
            if (strncasecmp(p, "infinite", 8) == 0) {
                p += 8;
                is_f64 = true;
                goto conv;
            } else if (strncasecmp(p, "inf", 3) == 0) {
                p += 3;
                is_f64 = true;
                goto conv;
            }
        }
        if (neg) return std::make_pair(result_status::MI, res);
        else     return std::make_pair(result_status::ND, res);
    }

    if (*p == '.') {
parse_fraction:
        do {
            p++;
        } while (is_digit(*p));
        is_f64 = true;
    }

    if (*p == 'e' || *p == 'E') {
        p++;
        if (*p == '+' || *p == '-') p++;
        if (!is_digit(*p))
            return std::make_pair(result_status::EX, res);
        do {
            p++;
        } while (is_digit(*p));
        is_f64 = true;
    }

conv:
    const char *end = (const char *)(in + (size_t)(p - in));
    std::from_chars_result conv_res;
    result_type type;

    if (is_f64) {
        conv_res = std::from_chars(in, end, res.as_f64);
        type = result_type::T_F64;
    } else if (neg) {
        conv_res = std::from_chars(in, end, res.as_i64, base);
        type = result_type::T_I64;
    } else {
        conv_res = std::from_chars(in, end, res.as_u64, base);
        type = result_type::T_U64;
    }

   if (conv_res.ec == std::errc::result_out_of_range)
       return std::make_pair(result_status::OO, res);
   else if (conv_res.ec == std::errc::invalid_argument)
       return std::make_pair(result_status::ND, res);
   else
       res.type = type;

    if (base != 10)
        in[neg ? 2 : 1] = literal_char;

    return std::make_pair(result_status::OK, res);
}

std::pair<result_status, result_num> parse_number(std::string& in)
{
    return parse_number((char *)in.data());
}
