#ifndef STRING_POOL_H
#define STRING_POOL_H

#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

using string_ref = uint32_t;

struct string_detail {
    uint32_t offset;
    uint32_t length;
};

class string_pool {
public:
    string_pool();
    ~string_pool() = default;

    string_ref add(const char *p);
    string_ref add(const char *p, uint32_t size);
    string_ref add(const std::string &s);

    string_ref concat(string_ref r1, string_ref r2);

    size_t size_of(string_ref ref) const;
    uint64_t hash_of(string_ref ref) const;
    const char *data_of(string_ref ref) const;

private:
    uint32_t m_offset; // offset for insertion into m_buffer
    std::string m_temp; // temporary for concatenation
    std::string m_buffer;
    std::vector<string_detail> m_array;
    std::unordered_map<uint64_t, string_ref> m_pool;

    uint64_t _hash(const char *p, uint32_t size) const;
};

#endif
