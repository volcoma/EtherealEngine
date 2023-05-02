#pragma once

#if defined(__cpp_char8_t)
#include <utility>

template<std::size_t N>
struct char8_t_string_literal
{
    static constexpr inline std::size_t size = N;

    template<std::size_t... I>
    constexpr char8_t_string_literal(const char8_t (&r)[N], std::index_sequence<I...>)
        : s{r[I]...}
    {}

    constexpr char8_t_string_literal(const char8_t (&r)[N])
        : char8_t_string_literal(r, std::make_index_sequence<N>())
    {}

    char8_t s[N];
};

template<char8_t_string_literal L, std::size_t... I>
constexpr inline const char as_char_buffer[sizeof...(I)] = { static_cast<char>(L.s[I])... };

template<char8_t_string_literal L, std::size_t... I>
constexpr auto& make_as_char_buffer(std::index_sequence<I...>)
{
    return as_char_buffer<L, I...>;
}

constexpr char operator ""_as_char(char8_t c)
{
    return char(c);
}

template<char8_t_string_literal L>
constexpr auto& operator""_as_char()
{
    return make_as_char_buffer<L>(std::make_index_sequence<decltype(L)::size>());
}

// we can use this macro if we want to make shorter vesrion of string
#define U8(x) u8##x##_as_char

#else

#include <cstddef>

constexpr char operator""_as_char(char ch)
{
    return ch;
}

constexpr const char* operator""_as_char(const char* arr, std::size_t /*size*/)
{
    return arr;
}

// we can use this macro if we want to make shorter vesrion of string
#define U8(x) u8##x

#endif
