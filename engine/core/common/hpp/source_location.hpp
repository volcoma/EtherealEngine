#pragma once
#ifndef HPP_SOURCE_LOCATION_HPP
#define HPP_SOURCE_LOCATION_HPP

#include <cstdint>

namespace hpp
{
struct source_location
{
public:
#if !defined(__apple_build_version__) && defined(__clang__) && (__clang_major__ >= 9) && !defined(NDEBUG)
    static constexpr source_location current(const char* file_name = __builtin_FILE(),
                                             const char* function_name = __builtin_FUNCTION(),
                                             const uint_least32_t line_number = __builtin_LINE(),
                                             const uint_least32_t column_offset = __builtin_COLUMN()) noexcept
#elif defined(__GNUC__) && (__GNUC__ > 4 || (__GNUC__ == 4 && __GNUC_MINOR__ >= 8)) && !defined(NDEBUG)
    static constexpr source_location current(const char* file_name = __builtin_FILE(),
                                             const char* function_name = __builtin_FUNCTION(),
                                             const uint_least32_t line_number = __builtin_LINE(),
                                             const uint_least32_t column_offset = 0) noexcept
#else
    static constexpr source_location current(const char* file_name = "unsupported",
                                             const char* function_name = "unsupported",
                                             const uint_least32_t line_number = 0,
                                             const uint_least32_t column_offset = 0) noexcept
#endif
    {
        return source_location(file_name, function_name, line_number, column_offset);
    }

    constexpr source_location() noexcept = default;
    source_location(const source_location&) = default;
    source_location(source_location&&) noexcept = default;
    source_location& operator=(const source_location&) = default;
    source_location& operator=(source_location&&) noexcept = default;
    constexpr const char* file_name() const noexcept
    {
        return file_name_;
    }

    constexpr const char* function_name() const noexcept
    {
        return function_name_;
    }

    constexpr uint_least32_t line() const noexcept
    {
        return line_number_;
    }

    constexpr std::uint_least32_t column() const noexcept
    {
        return column_offset_;
    }

private:
    constexpr source_location(const char* fileName, const char* functionName, const uint_least32_t lineNumber,
                              const uint_least32_t columnOffset) noexcept
        : file_name_(fileName)
        , function_name_(functionName)
        , line_number_(lineNumber)
        , column_offset_(columnOffset)
    {
    }

    const char* file_name_ {};
    const char* function_name_ {};
    std::uint_least32_t line_number_ {};
    std::uint_least32_t column_offset_ {};
};
} // end of namespace hpp

#endif //HPP_SOURCE_LOCATION_HPP
