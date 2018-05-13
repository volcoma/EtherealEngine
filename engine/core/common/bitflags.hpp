#pragma once

namespace bits
{

template <typename T>
inline static auto get_bit(T val, unsigned int bit) noexcept
{
    return ((val & (1 << bit)) >> bit);
}

template <typename T>
inline static auto get_bit_range(T val, unsigned int offset, unsigned int length) noexcept
{
    return ((val & (((1 << length) - 1) << offset)) >> offset);
}

template <typename T>
inline static void set_bit(T &val, unsigned int bit) noexcept
{
    val |= (1 << bit);
}

template <typename T>
inline static void clear_bit(T &val, unsigned int bit) noexcept
{
    val &= (val & ~(1 << bit));
}

template <typename T>
inline static void set_bit_flag(T &val, T flag) noexcept
{
    val |= flag;
}

template <typename T>
inline static void clear_bit_flag(T &val, T flag) noexcept
{
    val &= (val & (~flag));
}

template <typename T>
inline static auto check_bit_flag(T value, T flag) noexcept
{
    return (value & flag) == flag;
}

inline static auto get_low_nibble(unsigned char value) noexcept
{
    return static_cast<unsigned char>(value & 0x0F);
}

inline static auto get_high_nibble(unsigned char value) noexcept
{
    return static_cast<unsigned char>((value & 0xF0) >> 4);
}

} // namespace bits
