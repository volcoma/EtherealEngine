#ifndef STRONG_ALIAS_HPP
#define STRONG_ALIAS_HPP

#include <utility>

namespace hpp
{

template <typename BaseType, typename TypeName>
struct strong_alias
{
    explicit constexpr strong_alias(BaseType const& value)
        : value(value)
    {
    }
    explicit constexpr strong_alias(BaseType&& value)
        : value(std::move(value))
    {
    }
    operator BaseType&() noexcept
    {
        return value;
    }
    constexpr operator const BaseType&() const noexcept
    {
        return value;
    }
    BaseType value;
};
} // namespace

#define STRONG_ALIAS(BASE_TYPE, TYPE_NAME)                                                                   \
    struct TYPE_NAME : hpp::strong_alias<BASE_TYPE, TYPE_NAME>                                            \
    {                                                                                                        \
        using strong_alias::strong_alias;                                                                    \
    };

#endif // STRONG_ALIAS_HPP
