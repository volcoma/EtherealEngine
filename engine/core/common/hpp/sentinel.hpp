#pragma once
#include "utility/for_each.hpp"
#include <utility>
#include <functional>
#include <vector>
#include <memory>

namespace hpp
{

struct sentinel
{
    using predicate_t = std::function<bool()>;

    sentinel() = default;

    template<typename T>
    sentinel(const std::shared_ptr<T>& ptr)
        : sentinel(std::weak_ptr<T>(ptr)) {}

    template<typename T>
    sentinel(const std::weak_ptr<T>& wptr)
    {
        predicate_ = [wptr]()
        {
            return wptr.expired();
        };
    }

    template<typename Predicate>
    sentinel(const Predicate& predicate)
        : predicate_(predicate) {}

    bool expired() const
    {
        if(predicate_)
        {
            return predicate_();
        }
        return true;
    }

    predicate_t predicate_;
};

namespace internal
{
    using predicate_list = std::vector<hpp::sentinel::predicate_t>;

    template<typename T> struct is_shared_ptr : std::false_type {};
    template<typename T> struct is_shared_ptr<std::shared_ptr<T>> : std::true_type {};

    template<typename T>
    typename std::enable_if<is_shared_ptr<T>::value, void>::type
    inline add_sentinel(predicate_list& out, const T& sent)
    {
        using PtrInternalType = typename std::decay_t<decltype((*std::declval<T>().get()))>;
        out.emplace_back([wsent = std::weak_ptr<PtrInternalType>(sent)]()
        {
            return wsent.expired();
        });
    }

    template<typename T>
    typename std::enable_if<!is_shared_ptr<T>::value, void>::type
    inline add_sentinel(predicate_list& out, const T& sent)
    {
        out.emplace_back([sent]()
        {
            return sent.expired();
        });
    }
}

template<typename... Args>
inline sentinel make_sentinel(const Args&... args)
{
    internal::predicate_list expiration_check_functions;

    auto tuple = std::tuple<Args...>(args...);
    hpp::for_each(tuple, [&expiration_check_functions](const auto& el)
    {
        internal::add_sentinel(expiration_check_functions, el);
    });

    sentinel sent;
    sent.predicate_ = [expiration_check_functions]()
    {
        for(auto& is_expired_fn : expiration_check_functions)
        {
            if( is_expired_fn() )
            {
                return true;
            }
        }
        return false;
    };
    return sent;
}

inline sentinel eternal_sentinel()
{
    static auto ptr = std::make_shared<int>();
    auto sentinel = hpp::sentinel(ptr);
    return sentinel;
}

} //end of namespace hpp
