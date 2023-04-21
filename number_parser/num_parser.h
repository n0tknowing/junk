#ifndef NUM_PARSER_H
#define NUM_PARSER_H

#include <cstdint>
#include <string>
#include <utility>

enum class result_status : int8_t {
    OK =  0,
    LZ = -1, /* leading zero in base 10 */
    BL = -2, /* invalid binary literal */
    OL = -3, /* invalid octal literal */
    XL = -4, /* invalid hex literal */
    IL = -5, /* invalid literal */
    MI = -6, /* no digit after minus sign */
    ND = -7, /* invalid digit */
    FR = -8, /* no digit after fractional part */
    EX = -9, /* no digit after exponent part */
    OO = -10, /* conversion result is out of range */
};

enum class result_type : int8_t {
    T_INV = 0,
    T_F64 = 1,
    T_I64 = 2,
    T_U64 = 3,
};

struct result_num {
    result_type type;
    union {
        double as_f64;
        int64_t as_i64;
        uint64_t as_u64;
    };
};


const char *parse_number_err(result_status status);
std::pair<result_status, result_num> parse_number(char *in);
std::pair<result_status, result_num> parse_number(std::string& in);


#endif /* NUM_PARSER_H */
