#include <cstring>
#include <stdexcept>
#include "string_pool.h"
#define XXH_INLINE_ALL
#include "xxhash.h"

uint64_t string_pool::_hash(const char *p, uint32_t size) const
{
    return XXH3_64bits(p, size);
}

string_pool::string_pool()
{
    string_detail empty{};

    m_buffer.reserve(1u << 16); // 64 KiB
    m_array.reserve(512); // 511 strings, string_ref 0 is for empty string
    m_pool.reserve(512); // string_ref 0 is never stored here
    m_temp.reserve(512);

    m_array.push_back(empty);
    m_buffer.push_back('\0');
    m_offset = 1; // since offset 0 is for empty string, we start from 1
}

string_ref string_pool::add(const char *p, uint32_t size)
{
    string_ref ref{};

    if (p == nullptr || *p == '\0' || size == 0)
        return ref;

    uint64_t hash = _hash(p, size);
    if (auto ref2 = m_pool.find(hash); ref2 != m_pool.end())
        return ref2->second;

    ref = m_array.size();
    string_detail det{m_offset, size};

    m_buffer.insert(m_offset, p, static_cast<size_t>(size));
    m_buffer.push_back('\0');

    m_offset = m_buffer.size();

    m_pool.insert({hash, ref});
    m_array.push_back(det);

    return ref;
}

string_ref string_pool::add(const char *p)
{
    return (p == nullptr || *p == '\0') ? 0 : add(p, strlen(p));
}

string_ref string_pool::add(const std::string &s)
{
    return add(s.data(), static_cast<size_t>(s.size()));
}

string_ref string_pool::concat(string_ref ref1, string_ref ref2)
{
    size_t size1 = size_of(ref1);
    size_t size2 = size_of(ref2);
    size_t size = size1 + size2;

    if (size > m_temp.capacity())
        m_temp.reserve(size);

    m_temp.append(data_of(ref1), size1);
    m_temp.append(data_of(ref2), size);

    string_ref ref = add(m_temp);
    m_temp.clear();

    return ref;
}

size_t string_pool::size_of(string_ref ref) const
{
    if (ref > m_array.size())
        throw std::out_of_range("Dangling string_ref");

    return m_array[ref].length;
}

const char *string_pool::data_of(string_ref ref) const
{
    if (ref > m_array.size())
        throw std::out_of_range("Dangling string_ref");

    return m_buffer.data() + m_array[ref].offset;
}

uint64_t string_pool::hash_of(string_ref ref) const
{
    return _hash(data_of(ref), size_of(ref));
}
