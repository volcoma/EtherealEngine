#pragma once

#include "event.hpp"
#include "traits/operator_existence.hpp"


namespace hpp
{

template <typename T>
struct observed_property
{
    using on_change_event = event<void(const T&, const T&)>;

    observed_property() = default;

    /// Copy constructor.
    /// Does not copies the rhs's on_change events.
    observed_property(const observed_property& rhs)
        : value_(rhs.value_)
    {
    }

    observed_property(const T& rhs)
        : value_(rhs)
    {
    }

    /// Assignment operator.
    /// Does not copies the rhs's on_change events.
    observed_property& operator=(const observed_property& rhs)
    {
        if(this == &rhs)
        {
            return *this;
        }

        const auto old = value_;
        value_ = rhs.value_;
        on_change_.emit(old, value_);
        return *this;
    }

    observed_property& operator=(const T& rhs)
    {
        if(&value_ == &rhs)
        {
            return *this;
        }

        const auto old = value_;
        value_ = rhs;
        on_change_.emit(old, value_);
        return *this;
    }

    /// Events holder.
    /// Attach or detach events to it.
    /// Will be called on each object assignment, regardless the assigned value,
    /// with the old and new values as arguments.
    on_change_event& on_change()
    {
        return on_change_;
    }

    /// Underling object getter as cast.
    operator const T&() const
    {
        return value_;
    }

    /// Underling object getter.
    const T& get() const
    {
        return value_;
    }

    void clear()
    {
        value_ = {};
        on_change_ = {};
    }
private:
    T value_{};
    on_change_event on_change_{};

    static_assert ((std::is_trivially_copyable<T>::value || std::is_assignable<T, T>::value) && !std::is_reference<T>::value && !std::is_const<T>::value,
                   "Could be extended. Take in mind the moving operations."
                   "Consider the workflow for modifing a single field of a complex types."
                   "It's not convenient to make a copy of the object, modify it (e.g. via API calls) and then assigning the modified object.");
};

///
/// Operators over the underling type.
///
template<typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value || operator_existence::addition<T>::value>>
observed_property<T> operator+(const observed_property<T>& lhs, const T& rhs)
{
    return {lhs.get() + rhs};
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value || operator_existence::substraction<T>::value>>
observed_property<T> operator-(const observed_property<T>& lhs, const T& rhs)
{
    return {lhs.get() - rhs};
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value || operator_existence::multiplication<T>::value>>
observed_property<T> operator*(const observed_property<T>& lhs, const T& rhs)
{
    return {lhs.get() * rhs};
}

template<typename T, typename = std::enable_if_t<std::is_arithmetic<T>::value || operator_existence::division<T>::value>>
observed_property<T> operator/(const observed_property<T>& lhs, const T& rhs)
{
    return {lhs.get() / rhs};
}

template<typename T, typename = std::enable_if_t<operator_existence::addition<observed_property<T>, T>::value>>
observed_property<T>& operator+=(observed_property<T>& lhs, const T& rhs)
{
    lhs = lhs + rhs;
    return lhs;
}

template<typename T, typename = std::enable_if_t<operator_existence::substraction<observed_property<T>, T>::value>>
observed_property<T>& operator-=(observed_property<T>& lhs, const T& rhs)
{
    lhs = lhs - rhs;
    return lhs;
}

template<typename T, typename = std::enable_if_t<operator_existence::multiplication<observed_property<T>, T>::value>>
observed_property<T>& operator*=(observed_property<T>& lhs, const T& rhs)
{
    lhs = lhs * rhs;
    return lhs;
}

template<typename T, typename = std::enable_if_t<operator_existence::division<observed_property<T>, T>::value>>
observed_property<T>& operator/=(observed_property<T>& lhs, const T& rhs)
{
    lhs = lhs / rhs;
    return lhs;
}

template<typename T, typename = std::enable_if_t<(std::is_arithmetic<T>::value && !std::is_floating_point<T>::value) || operator_existence::equal<T>::value>>
bool operator==(const observed_property<T>& lhs, const T& rhs)
{
    return lhs.get() == rhs;
}

template<typename T, typename = std::enable_if_t<(std::is_arithmetic<T>::value && !std::is_floating_point<T>::value) || operator_existence::not_equal<T>::value>>
bool operator!=(const observed_property<T>& lhs, const T& rhs)
{
    return lhs.get() != rhs;
}

template<typename T, typename = std::enable_if_t<(std::is_arithmetic<T>::value && !std::is_floating_point<T>::value) || operator_existence::less<T>::value>>
bool operator<(const observed_property<T>& lhs, const T& rhs)
{
    return lhs.get() < rhs;
}

template<typename T, typename = std::enable_if_t<(std::is_arithmetic<T>::value && !std::is_floating_point<T>::value) || operator_existence::greater<T>::value>>
bool operator>(const observed_property<T>& lhs, const T& rhs)
{
    return lhs.get() > rhs;
}

template<typename T, typename = std::enable_if_t<(std::is_arithmetic<T>::value && !std::is_floating_point<T>::value) || operator_existence::less_or_equal<T>::value>>
bool operator<=(const observed_property<T>& lhs, const T& rhs)
{
    return lhs.get() <= rhs;
}

template<typename T, typename = std::enable_if_t<(std::is_arithmetic<T>::value && !std::is_floating_point<T>::value) || operator_existence::greater_or_equal<T>::value>>
bool operator>=(const observed_property<T>& lhs, const T& rhs)
{
    return lhs.get() >= rhs;
}

}
