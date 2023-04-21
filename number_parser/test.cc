#include <stdio.h>
#include <string>

#include <fmt/core.h>
#include <fmt/format.h>
#include "num_parser.h"

static void errvf_exit(fmt::string_view format, fmt::format_args args)
{
    fmt::vprint(stderr, format, args);
    fmt::print(stderr, "\n");
    exit(1);
}

template<typename S, typename... Args>
static void errf_exit(const S& format, Args&&... args)
{
    errvf_exit(format, fmt::make_format_args(args...));
}

#define unlikely(x)   (__builtin_expect(!!(x), 0))

static size_t get_input(char *in, size_t n, FILE *fp)
{
    int ch;
    unsigned char *p;

    if (unlikely(in == nullptr) || unlikely(fp == nullptr)) {
        return 0;
    } else if (unlikely(n == 0)) {
        return 0;
    } else if (unlikely(n == 1)) {
        *in = '\0';
        return 0;
    }

    p = (unsigned char *)in;
    n -= 1; /* for '\0' */

    /**
     * note that character encoding other than ASCII is accepted here.
     * so make sure your buffer large enough to handles it otherwise it will
     * splitted and you will get garbage result.
     */
    while (n > 1 && (ch = getc_unlocked(fp)) != EOF && ch != '\0') {
        *p++ = (unsigned char)ch; /* integer promotions are annoying */
        if (ch == '\n')
            break;
        n--;
    }

    if (n == 1) /* truncated or empty input or maybe done */
        *p++ = '\n';

    *p = '\0';
    return (size_t)(p - (unsigned char *)in); /* always > 0 */
}

int main(int argc, char **argv)
{
#ifndef TEST_ARGV
    FILE *fp = (argc > 1) ? fopen(argv[1], "r") : stdin;
    if (fp == nullptr)
        errf_exit("failed to open {}", argv[1]);

    size_t nread;
    char buffer[8192];
    while ((nread = get_input(buffer, sizeof(buffer), fp)) != 0) {
        buffer[nread - 1] = 0;
        auto r = parse_number(buffer);
        if (r.first == result_status::OK) {
            fmt::print("{}: ", buffer);
            switch (r.second.type) {
            case result_type::T_F64:
                fmt::print("{}\n", r.second.as_f64);
                break;
            case result_type::T_I64:
                fmt::print("{}\n", r.second.as_i64);
                break;
            case result_type::T_U64:
                fmt::print("{}\n", r.second.as_u64);
                break;
            default: /* to avoid warning */
                break;
            }
            continue;
        }
        errf_exit("failed to parse {}: {}", buffer, parse_number_err(r.first));
    }

    fclose(fp);
#else
    (void)get_input;
    std::string neg = (argc > 1) ? argv[1] : "------------20";
    auto r = parse_number(neg);
    if (r.first != result_status::OK)
        errf_exit("failed to parse {}: {}", neg, parse_number_err(r.first));
#endif
}
