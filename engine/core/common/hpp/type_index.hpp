#ifndef HPP_TYPE_INDEX
#define HPP_TYPE_INDEX

#include <cstddef>
#include <future>
#include <type_traits>

namespace rtti
{
// incremental id of type
struct type_index_sequential_t
{
    using index_t = size_t;

    template<typename Base, typename Derived>
    static index_t id()
    {
        static_assert(std::is_base_of<Base, Derived>::value, "D should be derived from B.");
        static index_t sid = get_counter<Base>()++;
        return sid;
    }

protected:
    template<typename Base>
    static index_t& get_counter()
    {
        static index_t value = 0;
        return value;
    }
};
} // namespace rtti

///////////////////////////////////////////////////////
#define __try_using_rtti__ 1
///////////////////////////////////////////////////////

#ifdef _MSC_VER
#ifndef __cpp_rtti
#define __cpp_rtti _CPPRTTI
#endif // !__cpp_rtti
#endif

#define __cpp_rtti_enabled__ __try_using_rtti__&& __cpp_rtti
#if __cpp_rtti_enabled__
#include <typeindex>
#endif

namespace rtti
{
class type_index_t;
namespace detail
{
template<typename T>
const type_index_t& type_id_impl();
}

class type_index_t
{
#if __cpp_rtti_enabled__
    using construct_t = const std::type_info;
#else
    using construct_t = const type_index_t&();
#endif
    construct_t* _info = nullptr;
    explicit type_index_t(construct_t* info) noexcept : _info{info}
    {
    }

    template<typename T>
    friend const type_index_t& detail::type_id_impl();

public:
    bool operator==(const type_index_t& o) const noexcept
    {
        return hash_code() == o.hash_code();
    }
    bool operator!=(const type_index_t& o) const noexcept
    {
        return hash_code() != o.hash_code();
    }
    bool operator<(const type_index_t& o) const noexcept
    {
        return hash_code() < o.hash_code();
    }
    bool operator>(const type_index_t& o) const noexcept
    {
        return hash_code() > o.hash_code();
    }

    std::size_t hash_code() const noexcept
    {
#if __cpp_rtti_enabled__
        return std::type_index(*_info).hash_code();
#else
        return reinterpret_cast<std::size_t>(_info);
#endif
    }
};

namespace detail
{
template<typename T>
const type_index_t& type_id_impl()
{
#if __cpp_rtti_enabled__
    static type_index_t id(&typeid(T));
#else
    static type_index_t id(&type_id_impl<T>);
#endif
    return id;
}
} // namespace detail

template<typename T>
const type_index_t& type_id()
{
    // this is required to copy the behavior of typeid operator
    return detail::type_id_impl<typename std::remove_cv<T>::type>();
}
} // namespace rtti

#endif
