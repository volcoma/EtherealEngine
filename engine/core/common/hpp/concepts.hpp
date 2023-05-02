#pragma once
#include "type_traits.hpp"
#include "utility/if_constexpr.hpp"

#include <cstring>
#include <iostream>
#include <string>
#include <vector>

namespace hpp
{

template <class C>
inline constexpr auto size(const C& c) -> decltype(c.size())
{
	return c.size();
}
template <class T, std::size_t N>
inline constexpr std::size_t size(const T (&)[N]) noexcept
{
	return N;
}

inline std::size_t size(const char* c) noexcept
{
	return std::strlen(c);
}

template <class C>
inline constexpr auto data(const C& c) -> decltype(c.data())
{
    return c.data();
}

template <class C>
inline constexpr auto data(C& c) -> decltype(c.data())
{
    return c.data();
}

template <typename T>
inline constexpr auto data(std::basic_string<T>& c)->T*
{
    if (c.empty())
    {
        return nullptr;
    }

    return &c[0];
}

template <class T, std::size_t N>
inline constexpr const T* data(const T (&)[N]) noexcept
{
    return N;
}

template <class T, std::size_t N>
inline constexpr T* data(T (&)[N]) noexcept
{
    return N;
}

inline const char* data(const char* c) noexcept
{
    return c;
}

inline char* data(char* c) noexcept
{
    return c;
}
} // namespace hpp

namespace traits
{

template <typename T>
using has_begin = decltype(std::begin(std::declval<const T>()));
template <typename T>
using has_end = decltype(std::end(std::declval<const T>()));
template <typename T>
using has_data = decltype(hpp::data(std::declval<const T>()));
template <typename T>
using has_size = decltype(hpp::size(std::declval<const T>()));
template <typename T>
using has_clear = decltype(std::declval<T>().clear());
template <typename T>
using has_resize = decltype(std::declval<T>().resize(std::declval<size_t>()));
template <typename T>
using has_key_type = typename T::key_type;
template <typename T>
using has_mapped_type = typename T::mapped_type;

template <typename T>
struct has_contiguous_storage : hpp::conjunction<hpp::is_detected<has_data, T>, hpp::is_detected<has_size, T>>
{
};

template <typename T>
struct has_contiguous_storage<std::vector<bool, T>> : std::false_type
{
};

template <typename T>
using is_range = hpp::conjunction<hpp::is_detected<has_begin, T>, hpp::is_detected<has_end, T>>;

template <typename T>
using is_container = hpp::conjunction<is_range<T>, hpp::is_detected<has_size, T>>;

template <typename T>
using is_contiguous = has_contiguous_storage<T>;

template <typename T>
using is_associative =
    hpp::conjunction<hpp::is_detected<has_key_type, T>, hpp::is_detected<has_mapped_type, T>>;

template <typename T>
using is_clearable = hpp::conjunction<is_container<T>, hpp::is_detected<has_clear, T>>;

template <typename T>
using is_resizeable = hpp::conjunction<is_container<T>, hpp::is_detected<has_resize, T>>;

template <typename T>
using is_fundamental = hpp::disjunction<std::is_fundamental<T>, std::is_enum<T>>;

template <typename T>
using is_integral = hpp::disjunction<std::is_integral<T>, std::is_enum<T>>;

template <class Iterator>
struct iterator_value
{
    using type = typename std::iterator_traits<Iterator>::value_type;
};
template <class T>
struct range_underlying_value : iterator_value<decltype(std::cbegin(std::declval<T>()))>
{
};

template <>
struct range_underlying_value<const char*> : iterator_value<const char*>
{
};

template <typename T>
struct container_helper
{
    static void resize_impl(T& val, size_t sz)
    {
        val.resize(sz);
    }
    static void clear_impl(T& val)
    {
        val.clear();
    }

    static void resize(T& val, size_t sz)
    {
        if_constexpr(traits::is_resizeable<T>::value)
        {
            container_helper<T>::resize_impl(val, sz);
        }
        end_if_constexpr;
    }

    static void clear(T& val)
    {
        if_constexpr(traits::is_clearable<T>::value)
        {
            container_helper<T>::clear_impl(val);
        }
        end_if_constexpr;
    }
};

} // namespace traits
