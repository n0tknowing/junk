#include <string>
#include <cstdio>

class const_str {
public:
    constexpr const_str() noexcept
        : m_str(nullptr), m_len(0) {}

    constexpr const_str(const char *str) noexcept
        : m_str(str), m_len(__builtin_strlen(str)) {}

    constexpr const_str(const const_str& other) noexcept = default;

    constexpr const_str(const char *str, size_t count) noexcept
        : m_str(str), m_len(count) {}

    constexpr const_str(const std::string& s) noexcept
        : m_str(s.c_str()), m_len(s.size()) {}

    ~const_str() = default;
    const_str(std::nullptr_t) = delete;
    const_str(std::nullptr_t, size_t) = delete;

    constexpr const_str& operator=(const const_str& other) noexcept = default;
    const_str& operator=(std::nullptr_t) = delete;

    constexpr size_t length() const noexcept { return m_len; }
    constexpr size_t size() const noexcept { return m_len; }
    constexpr const char *data() const noexcept { return m_str ? m_str : ""; }
    constexpr bool empty() const noexcept { return m_len == 0; }

    constexpr bool operator==(const_str& other) const noexcept {
        if (this == &other)
            return true;
        else if (m_len != other.m_len)
            return false;
        return __builtin_strcmp(m_str, other.m_str) == 0;
    }

    constexpr bool operator!=(const_str& other) const noexcept {
        return !(operator==(other));
    }

private:
    const char *m_str;
    size_t m_len;
};

constexpr const_str operator""_cs(const char *str, size_t count) noexcept {
    return const_str(str, count);
}

int main() {
    const_str msv{}, nsv{"from_nsv"};
    printf("msv = %s, %zu\n", msv.data(), msv.size());
    printf("nsv = %s, %zu\n\n", nsv.data(), nsv.size());
    msv = nsv;
    printf("msv = %s, %zu\n", msv.data(), msv.size());
    printf("nsv = %s, %zu\n", nsv.data(), nsv.size());
    const_str osv = "Hello\0World"_cs;
    printf("%s %zu\n", osv.data(), osv.size());
}
