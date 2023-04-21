// TODO:
//
// - New struct or union to handle heap-allocated value

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define NKJSON_NULL     0
#define NKJSON_BOOL     1
#define NKJSON_NUMBER   2
#define NKJSON_STRING   3
#define NKJSON_ARRAY    4
#define NKJSON_OBJECT   5
#define NKJSON_POINTER  6

#define QNAN_MASK       (UINT64_C(0x7ff8000000000000))
#define TYPE_MASK       (UINT64_C(0xffff000000000000))
#define SIGN_MASK       (UINT64_C(0x8000000000000000))

// JSON value representation that used here:
//
// null   = 000
// true   = 001
// false  = 010
// string = 100
//
// Number itself is already in double-precision floating format, so no need
// to create another bit mask, this leaves us 4 empty masks which can be used
// later.
//
// Array and Object are heap-allocated, so their encoding scheme are different
// than primitive types.
//
// String is special, if the supplied string less than 6 bytes then we can just put
// the value directly. For this to work we need string mask bit ended with zero,
// so for the last byte it's already '\0'-terminated as our string is C-style one.
// Of course this has limitation, only works on C-style string, but I think
// this should work on UTF-8 character which also part of ISO 8859-1 as it
// just 1 byte anyway.
//
// For example on little-endian machine, string "Hello" will encoded as:
//
//  +-- Sign bit (Not set on short string)
//  |
//  |    Quiet NaN          '\0'       'o'      'l'       'l'       'e'       'H'
//  v  vvvvvvvvvvvv       vvvvvvvv  vvvvvvvv  vvvvvvvv  vvvvvvvv  vvvvvvvv  vvvvvvvv
// [0][111111111111][100][00000000][01101111][01101100][01101100][01100101][01001000]
//                   ^^^ NKJSON_STRING
//
// Otherwise, String is stored on heap-allocated buffer.

#define NULL_MASK     QNAN_MASK
#define HEAP_MASK    (SIGN_MASK | QNAN_MASK)
#define TRUE_MASK    (UINT64_C(0x7ff9000000000000))
#define FALSE_MASK   (UINT64_C(0x7ffa000000000000))
#define STRING_MASK  (UINT64_C(0x7ffc000000000000))

typedef uint64_t nkjson_value;

// The reason why JSON Object implemented as linked list is because
// we very often to iterate the value inside JSON object, if we implemented
// it as Hash table then it would become serious issue regarding performance.
// Alternative is hash table with iterator implemented as linked list, this
// is probably good but the drawback is it uses more memory.
struct nkjson_object {
    struct nkjson_object *next;
    nkjson_value value;
}

struct nkjson_array {
    nkjson_value *values;
    size_t len;
    size_t cap;
};

static inline int is_on_heap(nkjson_value value)
{
    return (value & HEAP_MASK) == HEAP_MASK;
}

static inline void *value_on_heap(nkjson_value value)
{
    return (void *)((uint64_t)((uintptr_t)value & ~HEAP_MASK));
}

int nkjson_typeof(nkjson_value value)
{
    if ((value & QNAN_MASK) != QNAN_MASK)
        return NKJSON_NUMBER;
    else if (is_on_heap(value))
        return NKJSON_POINTER;

    switch (value & TYPE_MASK) {
    case NULL_MASK:
        return NKJSON_NULL;
    case TRUE_MASK:
    case FALSE_MASK:
        return NKJSON_BOOL;
    case STRING_MASK: // only happens if <= 5 string
        return NKJSON_STRING;
    default:
        break;
    }

    return NKJSON_NULL;
}

nkjson_value create_number(double value)
{
    nkjson_value retval;
    memcpy(&retval, &value, sizeof(retval));
    return retval;
}

nkjson_value create_null(void)
{
    return NULL_MASK;
}

nkjson_value create_bool(bool value)
{
    return value ? TRUE_MASK : FALSE_MASK;
}

nkjson_value create_string(const char *s, size_t len)
{
    nkjson_value retval = QNAN_MASK | STRING_MASK;
    char *str;

    if (s != NULL && len > 0) {
        if (len <= 5) {
            str = (char *)&retval;
            switch (len) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__           
            case 5: str[4] = s[4];
                // FALLTHROUGH
            case 4: str[3] = s[3];
                // FALLTHROUGH
            case 3: str[2] = s[2];
                // FALLTHROUGH
            case 2: str[1] = s[1];
                // FALLTHROUGH
            case 1: str[0] = s[0];
                // FALLTHROUGH
#elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
            case 5: str[6] = s[4];
                // FALLTHROUGH
            case 4: str[5] = s[3];
                // FALLTHROUGH
            case 3: str[4] = s[2];
                // FALLTHROUGH
            case 2: str[3] = s[1];
                // FALLTHROUGH
            case 1: str[2] = s[0];
                // FALLTHROUGH
#else
            #error Unknown endian, please buy a better computer
#endif
            default: // silence compiler warning
                break;
            }
        } else {
            str = calloc(len + 1, sizeof(*str));
            if (str != NULL) {
                memcpy(str, s, len);
                retval = HEAP_MASK | (uint64_t)((uintptr_t)str);
            } // else the result is "" and stored on the stack.
        }
    }

    return retval;
}

void free_string(nkjson_value value)
{
    if (is_on_heap(value)) {
        char *ptr = value_on_heap(value);
        free(ptr);
    }
}

const char *value_string(nkjson_value *value)
{
    const char *retval;

    if (is_on_heap(*value)) {
        retval = value_on_heap(*value);
    } else {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
        retval = (const char *)value;
#elif __BYTE_ORDER == __ORDER_BIG_ENDIAN__
        retval = ((const char *)value) + 2;
#else
        #error Unknown endian, please buy a better computer
#endif
    }

    return retval;
}

double value_number(nkjson_value value)
{
    double retval;
    memcpy(&retval, &value, sizeof(retval));
    return retval;
}

bool value_bool(nkjson_value value)
{
    return value == TRUE_MASK;
}
