#pragma once

#include <cstdarg>
#include <cstring>
#include <cstdio>
#include <cerrno>

#include "memory/stub.h"

// strerror_s (C11 Annex K, also provided by minwin_linux.h)
inline int strerror_s(char* buf, usize size, int errnum) {
    const char* msg = std::strerror(errnum);
    std::strncpy(buf, msg, size);
    buf[size - 1] = '\0';
    return 0;
}

template <usize N>
inline int strerror_s(char (&buf)[N], int errnum) {
    return strerror_s(buf, N, errnum);
}

#define ARTS_TRUNCATE _TRUNCATE

#if defined(_MSC_VER)

#    define arts_sprintf sprintf_s
#    define arts_snprintf _snprintf_s

#    define arts_vsprintf vsprintf_s
#    define arts_vsnprintf _vsnprintf_s

#    define arts_vsscanf vsscanf_s
#    define arts_sscanf sscanf_s

#    define arts_strcpy strcpy_s
#    define arts_strncpy strncpy_s

#    define arts_strcat strcat_s
#    define arts_strncat strncat_s

#    define arts_strtok strtok_s

#    define arts_sprintf sprintf_s
#    define arts_stricmp _stricmp
#    define arts_strnicmp _strnicmp
#    define arts_strupr strupr_s
#    define arts_itoa _itoa_s
#    define arts_ltoa _ltoa_s
#    define arts_ultoa _ultoa_s
#else

// _TRUNCATE is used with MSVC secure string functions; define it as no-limit for Linux
#ifndef _TRUNCATE
#    define _TRUNCATE ((size_t)-1)
#endif

// Portable implementations for Linux
inline int arts_sprintf(char* buffer, const char* format, ...) {
    va_list va;
    va_start(va, format);
    int result = std::vsprintf(buffer, format, va);
    va_end(va);
    return result;
}

// 3-arg overload matching sprintf_s(buffer, size, format, ...)
inline int arts_sprintf(char* buffer, size_t, const char* format, ...) {
    va_list va;
    va_start(va, format);
    int result = std::vsprintf(buffer, format, va);
    va_end(va);
    return result;
}

template <size_t N>
inline int arts_sprintf(char (&buffer)[N], const char* format, ...) {
    va_list va;
    va_start(va, format);
    int result = std::vsnprintf(buffer, N, format, va);
    va_end(va);
    return result;
}

template <size_t N>
inline int arts_snprintf(char (&buffer)[N], size_t count, const char* format, ...) {
    va_list va;
    va_start(va, format);
    int result = std::vsnprintf(buffer, count, format, va);
    va_end(va);
    return result;
}

inline int arts_snprintf(char* buffer, size_t count, const char* format, ...) {
    va_list va;
    va_start(va, format);
    int result = std::vsnprintf(buffer, count, format, va);
    va_end(va);
    return result;
}

inline int arts_vsprintf(char* buffer, const char* format, va_list va) {
    return std::vsprintf(buffer, format, va);
}

template <size_t N>
inline int arts_vsprintf(char (&buffer)[N], const char* format, va_list va) {
    return std::vsnprintf(buffer, N, format, va);
}

template <size_t N>
inline int arts_vsnprintf(char (&buffer)[N], size_t count, const char* format, va_list va) {
    return std::vsnprintf(buffer, count, format, va);
}

inline int arts_vsscanf(const char* buffer, const char* format, va_list va) {
    return std::vsscanf(buffer, format, va);
}

inline int arts_sscanf(const char* buffer, const char* format, ...) {
    va_list va;
    va_start(va, format);
    int result = std::vsscanf(buffer, format, va);
    va_end(va);
    return result;
}

inline char* arts_strcpy(char* dest, const char* src) {
    return std::strcpy(dest, src);
}

inline char* arts_strcpy(char* dest, size_t, const char* src) {
    return std::strcpy(dest, src);
}

inline char* arts_strncpy(char* dest, const char* src, size_t count) {
    if (count == (size_t)-1 || count >= SIZE_MAX / 2) {
        size_t len = strlen(src);
        memcpy(dest, src, len + 1);
        return dest;
    }
    return std::strncpy(dest, src, count);
}

inline char* arts_strcat(char* dest, const char* src) {
    return std::strcat(dest, src);
}

inline char* arts_strncat(char* dest, const char* src, size_t count) {
    if (count == (size_t)-1 || count >= SIZE_MAX / 2) {
        return std::strcat(dest, src);
    }
    return std::strncat(dest, src, count);
}

inline int arts_stricmp(const char* a, const char* b) {
    return strcasecmp(a, b);
}

inline int arts_strnicmp(const char* a, const char* b, size_t n) {
    return strncasecmp(a, b, n);
}

inline void arts_strupr(char* str, size_t len) {
    for (size_t i = 0; i < len && str[i]; ++i) {
        if (str[i] >= 'a' && str[i] <= 'z')
            str[i] = str[i] - 'a' + 'A';
    }
}

// Template overload for stack arrays (like MSVC's _strupr_s template)
template <size_t N>
inline void arts_strupr(char (&str)[N]) {
    arts_strupr(str, N);
}

inline char* arts_strtok(char* str, const char* delimiters, char** context) {
    return strtok_r(str, delimiters, context);
}

#endif

// ?arts_strdup@@YAPADPBD@Z
ARTS_EXPORT char* arts_strdup(const char* str);

class ConstString
{
public:
    constexpr ConstString() noexcept = default;

    constexpr ConstString(std::nullptr_t) noexcept
        : data_(nullptr)
    {}

    explicit ConstString(const char* value)
        : data_(arts_strdup(value))
    {}

    explicit ConstString(usize capacity)
        : data_(static_cast<char*>(arts_malloc(capacity)))
    {}

    ConstString(const ConstString& other)
        : ConstString(other.data_)
    {}

    ConstString(ConstString&& other) noexcept
    {
        data_ = other.data_;
        other.data_ = nullptr;
    }

    ~ConstString() noexcept
    {
        if (data_)
            arts_free(data_);
    }

    ConstString& operator=(const char* value)
    {
        assign(value);
        return *this;
    }

    ConstString& operator=(const ConstString& value)
    {
        assign(value.data_);
        return *this;
    }

    ConstString& operator=(ConstString&& value) noexcept
    {
        if (data_)
            arts_free(data_);

        data_ = value.data_;
        value.data_ = nullptr;
        return *this;
    }

    void assign(const char* value)
    {
        if (data_)
            arts_free(data_);
        data_ = value ? arts_strdup(value) : nullptr;
    }

    void reset()
    {
        if (data_)
        {
            arts_free(data_);
            data_ = nullptr;
        }
    }

    char* get() noexcept { return data_; }
    const char* get() const noexcept { return data_; }

    char& operator[](usize index) noexcept { return data_[index]; }
    const char& operator[](usize index) const noexcept { return data_[index]; }

    explicit operator bool() const noexcept { return data_ != nullptr; }

private:
    char* data_ {};
};

static_assert(sizeof(ConstString) == sizeof(char*));

ConstString arts_getenv(const char* name);

class arts_format_t {};
class arts_vformat_t {};

template <usize N>
class CStringBuffer
{
public:
    static_assert(N != 0, "Cannot have an empty string buffer");

    CStringBuffer() { buffer_[0] = '\0'; }
    CStringBuffer(const char* value) { assign(value); }

    CStringBuffer(arts_vformat_t, ARTS_FORMAT_STRING const char* format, std::va_list va)
    {
        arts_vsprintf(buffer_, format, va);
    }

    CStringBuffer(arts_format_t, ARTS_FORMAT_STRING const char* format, ...)
    {
        std::va_list va;
        va_start(va, format);
        arts_vsprintf(buffer_, format, va);
        va_end(va);
    }

    void clear() { buffer_[0] = '\0'; }
    void assign(const char* value) { arts_strcpy(buffer_, value); }
    void assign(const char* value, usize len) { arts_strncpy(buffer_, value, len); }
    void append(const char* value) { arts_strcat(buffer_, value); }
    void append(const char* value, usize len) { arts_strncat(buffer_, value, len); }

    char* get() { return buffer_; }
    const char* get() const { return buffer_; }

    operator char*() { return buffer_; }
    operator const char*() const { return buffer_; }

    static constexpr usize capacity() { return N; }

private:
    char buffer_[N];
};

class CStringBuilder
{
public:
    constexpr CStringBuilder(char* buffer, usize capacity)
        : buffer_(buffer)
        , capacity_(capacity)
    {}

    ~CStringBuilder()
    {
        buffer_[(written_ < capacity_) ? written_ : 0] = '\0';
    }

    void operator+=(const char* str) { append(str, std::strlen(str)); }

    void operator+=(char c)
    {
        if (written_ < capacity_)
            buffer_[written_++] = c;
    }

    void append(const char* str, usize len)
    {
        if (written_ + len >= capacity_)
        {
            written_ = capacity_;
            return;
        }
        std::memcpy(buffer_ + written_, str, len);
        written_ += len;
    }

private:
    char* buffer_ {};
    usize capacity_ {};
    usize written_ {};
};

inline bool IsSpace(i32 value)
{
    return (value == '\t' || value == '\n' || value == '\r' || value == ' ');
}

inline bool IsDigit(i32 value)
{
    return (value >= '0' && value <= '9');
}

inline bool IsLetter(i32 value)
{
    return (value >= 'A' && value <= 'Z') || (value >= 'a' && value <= 'z');
}

inline bool operator==(const ConstString& lhs, const ConstString& rhs) { return std::strcmp(lhs.get(), rhs.get()) == 0; }
inline bool operator==(const ConstString& lhs, const char* rhs) { return std::strcmp(lhs.get(), rhs) == 0; }
inline bool operator==(const char* lhs, const ConstString& rhs) { return std::strcmp(lhs, rhs.get()) == 0; }
inline bool operator!=(const ConstString& lhs, const ConstString& rhs) { return std::strcmp(lhs.get(), rhs.get()) != 0; }
inline bool operator!=(const ConstString& lhs, const char* rhs) { return std::strcmp(lhs.get(), rhs) != 0; }
inline bool operator!=(const char* lhs, const ConstString& rhs) { return std::strcmp(lhs, rhs.get()) != 0; }

template <usize N, typename... Args>
ARTS_FORCEINLINE CStringBuffer<N> arts_formatf(const char* format, const Args&... args)
{
    return CStringBuffer<N> {arts_format_t {}, format, args...};
}

template <usize N>
ARTS_FORCEINLINE CStringBuffer<N> arts_vformatf(const char* format, std::va_list va)
{
    return CStringBuffer<N> {arts_vformat_t {}, format, va};
}
