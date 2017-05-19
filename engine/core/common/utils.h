#pragma once
#include <cstdint>
#include <memory>

namespace utils
{
	template <class T>
	inline void hash_combine(std::size_t & seed, const T & v)
	{
		std::hash<T> hasher;
		seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}

    namespace detail
    {
        template<class T>
        struct _Unique_if
        {
            using _Single_object = std::unique_ptr<T>;
        };

        template<class T>
        struct _Unique_if<T[]>
        {
            using _Unknown_bound = std::unique_ptr<T[]>;
        };

        template<class T, size_t N>
        struct _Unique_if<T[N]>
        {
            using _Known_bound = void;
        };
    }


    template<class T, class... Args>
    typename detail::_Unique_if<T>::_Single_object
    make_unique(Args&&... args)
    {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }

    template<class T>
    typename detail::_Unique_if<T>::_Unknown_bound
    make_unique(size_t n)
    {
        using U = typename std::remove_extent<T>::type;
        return std::unique_ptr<T>(new U[n]());
    }

    template<class T, class... Args>
    typename detail::_Unique_if<T>::_Known_bound
    make_unique(Args&&...) = delete;
};

