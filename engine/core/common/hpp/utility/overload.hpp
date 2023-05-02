#pragma once

namespace hpp
{
template <class... Fs>
struct overloaded;

template <class F0, class... Frest>
struct overloaded<F0, Frest...> : F0, overloaded<Frest...>
{
    overloaded(F0 f0, Frest... rest) : F0(f0), overloaded<Frest...>(rest...) {}

    using F0::operator();
    using overloaded<Frest...>::operator();
};

template <class F0>
struct overloaded<F0> : F0
{
    overloaded(F0 f0) : F0(f0) {}

    using F0::operator();
};

template <class... Fs>
auto overload(Fs... fs) -> overloaded<Fs...>
{
    return overloaded<Fs...>(fs...);
}
}
