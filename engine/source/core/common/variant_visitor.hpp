#ifndef _UTIL_VARIANT_VISITOR_HPP
#define _UTIL_VARIANT_VISITOR_HPP

namespace nonstd {

template <typename... Fns>
struct visitor;

template <typename Fn>
struct visitor<Fn> : Fn
{
    using type = Fn;
    using Fn::operator();

    visitor(Fn fn) : Fn(fn) {}
};

template <typename Fn, typename... Fns>
struct visitor<Fn, Fns...> : Fn, visitor<Fns...>
{
    using type = visitor;
    using Fn::operator();
    using visitor<Fns...>::operator();

    visitor(Fn fn, Fns... fns) : Fn(fn), visitor<Fns...>(fns...) {}
};

template <typename... Fns>
visitor<Fns...> make_visitor(Fns... fns)
{
    return visitor<Fns...>(fns...);
}

} // namespace nonstd

#endif // _UTIL_VARIANT_VISITOR_HPP
