#pragma once
#include "invoke.hpp"
#include <stdexcept>
#include <tuple>

namespace hpp
{
namespace detail
{

template <typename Tuple, typename F, std::size_t... Indices>
void for_each_impl(Tuple&& tuple, F&& f, std::index_sequence<Indices...>)
{
	ignore(tuple, f);
	using swallow = int[];
	(void)swallow{1, (f(std::get<Indices>(std::forward<Tuple>(tuple))), void(), int{})...};
}


template <size_t I>
struct visit_impl
{
    template <typename T, typename F>
    static void visit(T&& tup, size_t idx, F&& fun)
    {
        if(idx == I - 1)
        {
            fun(std::get<I - 1>(tup));
        }
        else
        {
            visit_impl<I - 1>::visit(tup, idx, std::forward<F>(fun));
        }
    }
};

template <>
struct visit_impl<0>
{
    template <typename T, typename F>
    static void visit(T&& /*unused*/, size_t /*unused*/, F&& /*unused*/)
    {
        throw std::runtime_error("bad field index");
    }
};

} // namespace detail

template <typename Tuple, typename F>
void for_each(Tuple&& tuple, F&& f)
{
	detail::for_each_impl(std::forward<Tuple>(tuple), std::forward<F>(f),
						  std::make_index_sequence<std::tuple_size<std::remove_reference_t<Tuple>>::value>{});
}


template<class T> struct tag_t { constexpr tag_t() {}; using type=T; };
template<class T> constexpr tag_t<T> tag{};
template<class Tag> using type_t = typename Tag::type;

template<class...Ts, class F>
void for_each_type(F&& f)
{
    using discard=int[];
    (void)discard{ 0,(void(f( tag<Ts> ) ),0)...};
}

template <typename Tuple, typename F>
void visit_at(Tuple&& tup, size_t idx, F&& f)
{
    detail::visit_impl<std::tuple_size<std::decay_t<Tuple>>::value>::visit(tup, idx, std::forward<F>(f));
}

} // namespace hpp
