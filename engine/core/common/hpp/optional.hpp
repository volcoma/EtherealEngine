#ifndef STX_OPTIONAL_HPP_INCLUDED
#define STX_OPTIONAL_HPP_INCLUDED

#ifndef STX_NAMESPACE_NAME
#define STX_NAMESPACE_NAME hpp
#endif

// libc++ on Apple has a broken std::experimental::optional version
#if !defined(STX_NO_STD_OPTIONAL) && defined(__APPLE__)
// This header is empty on C++ but defines _LIBCPP_VERSION for us
#include <ciso646>
#if defined(_LIBCPP_VERSION) && (_LIBCPP_VERSION <= 4000)
#define STX_NO_STD_OPTIONAL
#endif // _LIBCPP_VERSION
#endif // __APPLE__

// libstdc++ and libc++'s std::experimental::optional only work in C++14 mode
#if !defined(STX_NO_STD_OPTIONAL) && defined(__GNUC__) && (__cplusplus < 201402)
#define STX_NO_STD_OPTIONAL
#endif

#if defined(__has_include) && !defined(STX_NO_STD_OPTIONAL)
#if __has_include(<optional>) && (__cplusplus > 201402)
#include <optional>
namespace STX_NAMESPACE_NAME
{
using std::bad_optional_access;
#ifndef STX_HAVE_IN_PLACE_T
using std::in_place;
using std::in_place_t;
#define STX_HAVE_IN_PLACE_T 1
#endif
using std::make_optional;
using std::nullopt;
using std::nullopt_t;
using std::optional;
}
#define STX_HAVE_STD_OPTIONAL 1
//#elif __has_include(<experimental/optional>)
//#include <experimental/optional>
//namespace STX_NAMESPACE_NAME
//{
//using std::experimental::bad_optional_access;
//#ifndef STX_HAVE_IN_PLACE_T
//using std::experimental::in_place;
//using std::experimental::in_place_t;

//#define STX_HAVE_IN_PLACE_T 1
//#endif
//using std::experimental::make_optional;
//using std::experimental::nullopt;
//using std::experimental::nullopt_t;
//using std::experimental::optional;
//}
//#define STX_HAVE_STD_OPTIONAL 1
#endif // __hasinclude(optional)
#endif // defined(__hasinclude)

#ifndef STX_HAVE_STD_OPTIONAL

#define TL_OPTIONAL_VERSION_MAJOR 1
#define TL_OPTIONAL_VERSION_MINOR 0
#define TL_OPTIONAL_VERSION_PATCH 0

#include <exception>
#include <functional>
#include <new>
#include <type_traits>
#include <utility>
#include "variant.hpp"

#if (defined(_MSC_VER) && _MSC_VER == 1900)
#define TL_OPTIONAL_MSVC2015
#endif

#if (defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ <= 9 &&              \
     !defined(__clang__))
#define TL_OPTIONAL_GCC49
#endif

#if (defined(__GNUC__) && __GNUC__ == 5 && __GNUC_MINOR__ <= 4 &&              \
     !defined(__clang__))
#define TL_OPTIONAL_GCC54
#endif

#if (defined(__GNUC__) && __GNUC__ == 5 && __GNUC_MINOR__ <= 5 &&              \
     !defined(__clang__))
#define TL_OPTIONAL_GCC55
#endif

#if (defined(__GNUC__) && __GNUC__ == 4 && __GNUC_MINOR__ <= 9 &&              \
     !defined(__clang__))
// GCC < 5 doesn't support overloading on const&& for member functions
#define TL_OPTIONAL_NO_CONSTRR

// GCC < 5 doesn't support some standard C++11 type traits
#define TL_OPTIONAL_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(T)                                     \
  std::has_trivial_copy_constructor<T>::value
#define TL_OPTIONAL_IS_TRIVIALLY_COPY_ASSIGNABLE(T) std::has_trivial_copy_assign<T>::value

// This one will be different for GCC 5.7 if it's ever supported
#define TL_OPTIONAL_IS_TRIVIALLY_DESTRUCTIBLE(T) std::is_trivially_destructible<T>::value

// GCC 5 < v < 8 has a bug in is_trivially_copy_constructible which breaks std::vector
// for non-copyable types
#elif (defined(__GNUC__) && __GNUC__ < 8 &&                                                \
     !defined(__clang__))
#ifndef TL_GCC_LESS_8_TRIVIALLY_COPY_CONSTRUCTIBLE_MUTEX
#define TL_GCC_LESS_8_TRIVIALLY_COPY_CONSTRUCTIBLE_MUTEX
namespace hpp {
namespace hpp_detail {
template<class T>
struct is_trivially_copy_constructible : std::is_trivially_copy_constructible<T>{};
#ifdef _GLIBCXX_VECTOR
template<class T, class A>
struct is_trivially_copy_constructible<std::vector<T,A>>
    : std::is_trivially_copy_constructible<T>{};
#endif
  }
}
#endif

#define TL_OPTIONAL_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(T)                                     \
    hpp::hpp_detail::is_trivially_copy_constructible<T>::value
#define TL_OPTIONAL_IS_TRIVIALLY_COPY_ASSIGNABLE(T)                                        \
  std::is_trivially_copy_assignable<T>::value
#define TL_OPTIONAL_IS_TRIVIALLY_DESTRUCTIBLE(T) std::is_trivially_destructible<T>::value
#else
#define TL_OPTIONAL_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(T)                                     \
  std::is_trivially_copy_constructible<T>::value
#define TL_OPTIONAL_IS_TRIVIALLY_COPY_ASSIGNABLE(T)                                        \
  std::is_trivially_copy_assignable<T>::value
#define TL_OPTIONAL_IS_TRIVIALLY_DESTRUCTIBLE(T) std::is_trivially_destructible<T>::value
#endif

#if __cplusplus > 201103L
#define TL_OPTIONAL_CXX14
#endif

// constexpr implies const in C++11, not C++14
#if (__cplusplus == 201103L || defined(TL_OPTIONAL_MSVC2015) ||                \
     defined(TL_OPTIONAL_GCC49))
#define TL_OPTIONAL_11_CONSTEXPR
#else
#define TL_OPTIONAL_11_CONSTEXPR constexpr
#endif

namespace hpp {
//#ifndef TL_MONOSTATE_INPLACE_MUTEX
//#define TL_MONOSTATE_INPLACE_MUTEX
///// Used to represent an optional with no data; essentially a bool
//class monostate {};

/////  A tag type to tell optional to construct its value in-place
//struct in_place_t {
//    explicit in_place_t() = default;
//};
///// A tag to tell optional to construct its value in-place
//static constexpr in_place_t in_place{};
//#endif

template <class T> class optional;

namespace hpp_detail {
#ifndef TL_TRAITS_MUTEX
#define TL_TRAITS_MUTEX
// C++14-style aliases for brevity
template <class T> using remove_const_t = typename std::remove_const<T>::type;
template <class T>
using remove_reference_t = typename std::remove_reference<T>::type;
template <class T> using decay_t = typename std::decay<T>::type;
template <bool E, class T = void>
using enable_if_t = typename std::enable_if<E, T>::type;
template <bool B, class T, class F>
using conditional_t = typename std::conditional<B, T, F>::type;

// std::conjunction from C++17
template <class...> struct conjunction : std::true_type {};
template <class B> struct conjunction<B> : B {};
template <class B, class... Bs>
struct conjunction<B, Bs...>
    : std::conditional<bool(B::value), conjunction<Bs...>, B>::type {};

#if defined(_LIBCPP_VERSION) && __cplusplus == 201103L
#define TL_TRAITS_LIBCXX_MEM_FN_WORKAROUND
#endif

// In C++11 mode, there's an issue in libc++'s std::mem_fn
// which results in a hard-error when using it in a noexcept expression
// in some cases. This is a check to workaround the common failing case.
#ifdef TL_TRAITS_LIBCXX_MEM_FN_WORKAROUND
template <class T> struct is_pointer_to_non_const_member_func : std::false_type{};
template <class T, class Ret, class... Args>
struct is_pointer_to_non_const_member_func<Ret (T::*) (Args...)> : std::true_type{};
template <class T, class Ret, class... Args>
struct is_pointer_to_non_const_member_func<Ret (T::*) (Args...)&> : std::true_type{};
template <class T, class Ret, class... Args>
struct is_pointer_to_non_const_member_func<Ret (T::*) (Args...)&&> : std::true_type{};
template <class T, class Ret, class... Args>
struct is_pointer_to_non_const_member_func<Ret (T::*) (Args...) volatile> : std::true_type{};
template <class T, class Ret, class... Args>
struct is_pointer_to_non_const_member_func<Ret (T::*) (Args...) volatile&> : std::true_type{};
template <class T, class Ret, class... Args>
struct is_pointer_to_non_const_member_func<Ret (T::*) (Args...) volatile&&> : std::true_type{};

template <class T> struct is_const_or_const_ref : std::false_type{};
template <class T> struct is_const_or_const_ref<T const&> : std::true_type{};
template <class T> struct is_const_or_const_ref<T const> : std::true_type{};
#endif

// std::invoke from C++17
// https://stackoverflow.com/questions/38288042/c11-14-invoke-workaround
template <typename Fn, typename... Args,
#ifdef TL_TRAITS_LIBCXX_MEM_FN_WORKAROUND
          typename = enable_if_t<!(is_pointer_to_non_const_member_func<Fn>::value
                                 && is_const_or_const_ref<Args...>::value)>,
#endif
          typename = enable_if_t<std::is_member_pointer<decay_t<Fn>>::value>,
          int = 0>
constexpr auto invoke(Fn &&f, Args &&... args) noexcept(
    noexcept(std::mem_fn(f)(std::forward<Args>(args)...)))
    -> decltype(std::mem_fn(f)(std::forward<Args>(args)...)) {
    return std::mem_fn(f)(std::forward<Args>(args)...);
}

template <typename Fn, typename... Args,
          typename = enable_if_t<!std::is_member_pointer<decay_t<Fn>>::value>>
constexpr auto invoke(Fn &&f, Args &&... args) noexcept(
    noexcept(std::forward<Fn>(f)(std::forward<Args>(args)...)))
    -> decltype(std::forward<Fn>(f)(std::forward<Args>(args)...)) {
    return std::forward<Fn>(f)(std::forward<Args>(args)...);
}

// std::invoke_result from C++17
template <class F, class, class... Us> struct invoke_result_impl;

template <class F, class... Us>
struct invoke_result_impl<
    F, decltype(hpp_detail::invoke(std::declval<F>(), std::declval<Us>()...), void()),
    Us...> {
    using type = decltype(hpp_detail::invoke(std::declval<F>(), std::declval<Us>()...));
};

template <class F, class... Us>
using invoke_result = invoke_result_impl<F, void, Us...>;

template <class F, class... Us>
using invoke_result_t = typename invoke_result<F, Us...>::type;

#if defined(_MSC_VER) && _MSC_VER <= 1900
// TODO make a version which works with MSVC 2015
template <class T, class U = T> struct is_swappable : std::true_type {};

template <class T, class U = T> struct is_nothrow_swappable : std::true_type {};
#else
// https://stackoverflow.com/questions/26744589/what-is-a-proper-way-to-implement-is-swappable-to-test-for-the-swappable-concept
namespace swap_adl_tests {
// if swap ADL finds this then it would call std::swap otherwise (same
// signature)
struct tag {};

template <class T> tag swap(T &, T &);
template <class T, std::size_t N> tag swap(T (&a)[N], T (&b)[N]);

// helper functions to test if an unqualified swap is possible, and if it
// becomes std::swap
template <class, class> std::false_type can_swap(...) noexcept(false);
template <class T, class U,
          class = decltype(swap(std::declval<T &>(), std::declval<U &>()))>
std::true_type can_swap(int) noexcept(noexcept(swap(std::declval<T &>(),
                                                    std::declval<U &>())));

template <class, class> std::false_type uses_std(...);
template <class T, class U>
std::is_same<decltype(swap(std::declval<T &>(), std::declval<U &>())), tag>
uses_std(int);

template <class T>
struct is_std_swap_noexcept
    : std::integral_constant<bool,
                             std::is_nothrow_move_constructible<T>::value &&
                                 std::is_nothrow_move_assignable<T>::value> {};

template <class T, std::size_t N>
struct is_std_swap_noexcept<T[N]> : is_std_swap_noexcept<T> {};

template <class T, class U>
struct is_adl_swap_noexcept
    : std::integral_constant<bool, noexcept(can_swap<T, U>(0))> {};
} // namespace swap_adl_tests

template <class T, class U = T>
struct is_swappable
    : std::integral_constant<
          bool,
          decltype(hpp_detail::swap_adl_tests::can_swap<T, U>(0))::value &&
              (!decltype(hpp_detail::swap_adl_tests::uses_std<T, U>(0))::value ||
               (std::is_move_assignable<T>::value &&
                std::is_move_constructible<T>::value))> {};

template <class T, std::size_t N>
struct is_swappable<T[N], T[N]>
    : std::integral_constant<
          bool,
          decltype(hpp_detail::swap_adl_tests::can_swap<T[N], T[N]>(0))::value &&
              (!decltype(
                   hpp_detail::swap_adl_tests::uses_std<T[N], T[N]>(0))::value ||
               is_swappable<T, T>::value)> {};

template <class T, class U = T>
struct is_nothrow_swappable
    : std::integral_constant<
          bool,
          is_swappable<T, U>::value &&
              ((decltype(hpp_detail::swap_adl_tests::uses_std<T, U>(0))::value
                    &&hpp_detail::swap_adl_tests::is_std_swap_noexcept<T>::value) ||
               (!decltype(hpp_detail::swap_adl_tests::uses_std<T, U>(0))::value &&
                    hpp_detail::swap_adl_tests::is_adl_swap_noexcept<T,
                                                                 U>::value))> {
};
#endif
#endif

// std::void_t from C++17
template <class...> struct voider { using type = void; };
template <class... Ts> using void_t = typename voider<Ts...>::type;

// Trait for checking if a type is a hpp::optional
template <class T> struct is_optional_impl : std::false_type {};
template <class T> struct is_optional_impl<optional<T>> : std::true_type {};
template <class T> using is_optional = is_optional_impl<decay_t<T>>;

// Change void to hpp::monostate
template <class U>
using fixup_void = conditional_t<std::is_void<U>::value, monostate, U>;

template <class F, class U, class = invoke_result_t<F, U>>
using get_map_return = optional<fixup_void<invoke_result_t<F, U>>>;

// Check if invoking F for some Us returns void
template <class F, class = void, class... U> struct returns_void_impl;
template <class F, class... U>
struct returns_void_impl<F, void_t<invoke_result_t<F, U...>>, U...>
    : std::is_void<invoke_result_t<F, U...>> {};
template <class F, class... U>
using returns_void = returns_void_impl<F, void, U...>;

template <class T, class... U>
using enable_if_ret_void = enable_if_t<returns_void<T &&, U...>::value>;

template <class T, class... U>
using disable_if_ret_void = enable_if_t<!returns_void<T &&, U...>::value>;

template <class T, class U>
using enable_forward_value =
    hpp_detail::enable_if_t<std::is_constructible<T, U &&>::value &&
                        !std::is_same<hpp_detail::decay_t<U>, in_place_t>::value &&
                        !std::is_same<optional<T>, hpp_detail::decay_t<U>>::value>;

template <class T, class U, class Other>
using enable_from_other = hpp_detail::enable_if_t<
    std::is_constructible<T, Other>::value &&
    !std::is_constructible<T, optional<U> &>::value &&
    !std::is_constructible<T, optional<U> &&>::value &&
    !std::is_constructible<T, const optional<U> &>::value &&
    !std::is_constructible<T, const optional<U> &&>::value &&
    !std::is_convertible<optional<U> &, T>::value &&
    !std::is_convertible<optional<U> &&, T>::value &&
    !std::is_convertible<const optional<U> &, T>::value &&
    !std::is_convertible<const optional<U> &&, T>::value>;

template <class T, class U>
using enable_assign_forward = hpp_detail::enable_if_t<
    !std::is_same<optional<T>, hpp_detail::decay_t<U>>::value &&
    !hpp_detail::conjunction<std::is_scalar<T>,
                         std::is_same<T, hpp_detail::decay_t<U>>>::value &&
    std::is_constructible<T, U>::value && std::is_assignable<T &, U>::value>;

template <class T, class U, class Other>
using enable_assign_from_other = hpp_detail::enable_if_t<
    std::is_constructible<T, Other>::value &&
    std::is_assignable<T &, Other>::value &&
    !std::is_constructible<T, optional<U> &>::value &&
    !std::is_constructible<T, optional<U> &&>::value &&
    !std::is_constructible<T, const optional<U> &>::value &&
    !std::is_constructible<T, const optional<U> &&>::value &&
    !std::is_convertible<optional<U> &, T>::value &&
    !std::is_convertible<optional<U> &&, T>::value &&
    !std::is_convertible<const optional<U> &, T>::value &&
    !std::is_convertible<const optional<U> &&, T>::value &&
    !std::is_assignable<T &, optional<U> &>::value &&
    !std::is_assignable<T &, optional<U> &&>::value &&
    !std::is_assignable<T &, const optional<U> &>::value &&
    !std::is_assignable<T &, const optional<U> &&>::value>;

// The storage base manages the actual storage, and correctly propagates
// trivial destruction from T. This case is for when T is not trivially
// destructible.
template <class T, bool = ::std::is_trivially_destructible<T>::value>
struct optional_storage_base {
    TL_OPTIONAL_11_CONSTEXPR optional_storage_base() noexcept
        : m_dummy(), m_has_value(false) {}

    template <class... U>
    TL_OPTIONAL_11_CONSTEXPR optional_storage_base(in_place_t, U &&... u)
        : m_value(std::forward<U>(u)...), m_has_value(true) {}

    ~optional_storage_base() {
        if (m_has_value) {
            m_value.~T();
            m_has_value = false;
        }
    }

    struct dummy {};
    union {
        dummy m_dummy;
        T m_value;
    };

    bool m_has_value;
};

// This case is for when T is trivially destructible.
template <class T> struct optional_storage_base<T, true> {
    TL_OPTIONAL_11_CONSTEXPR optional_storage_base() noexcept
        : m_dummy(), m_has_value(false) {}

    template <class... U>
    TL_OPTIONAL_11_CONSTEXPR optional_storage_base(in_place_t, U &&... u)
        : m_value(std::forward<U>(u)...), m_has_value(true) {}

    // No destructor, so this class is trivially destructible

    struct dummy {};
    union {
        dummy m_dummy;
        T m_value;
    };

    bool m_has_value = false;
};

// This base class provides some handy member functions which can be used in
// further derived classes
template <class T> struct optional_operations_base : optional_storage_base<T> {
    using optional_storage_base<T>::optional_storage_base;

    void hard_reset() noexcept {
        get().~T();
        this->m_has_value = false;
    }

    template <class... Args> void construct(Args &&... args) noexcept {
        new (std::addressof(this->m_value)) T(std::forward<Args>(args)...);
        this->m_has_value = true;
    }

    template <class Opt> void assign(Opt &&rhs) {
        if (this->has_value()) {
            if (rhs.has_value()) {
                this->m_value = std::forward<Opt>(rhs).get();
            } else {
                this->m_value.~T();
                this->m_has_value = false;
            }
        }

        else if (rhs.has_value()) {
            construct(std::forward<Opt>(rhs).get());
        }
    }

    bool has_value() const { return this->m_has_value; }

    TL_OPTIONAL_11_CONSTEXPR T &get() & { return this->m_value; }
    TL_OPTIONAL_11_CONSTEXPR const T &get() const & { return this->m_value; }
    TL_OPTIONAL_11_CONSTEXPR T &&get() && { return std::move(this->m_value); }
#ifndef TL_OPTIONAL_NO_CONSTRR
    constexpr const T &&get() const && { return std::move(this->m_value); }
#endif
};

// This class manages conditionally having a trivial copy constructor
// This specialization is for when T is trivially copy constructible
template <class T, bool = TL_OPTIONAL_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(T)>
struct optional_copy_base : optional_operations_base<T> {
    using optional_operations_base<T>::optional_operations_base;
};

// This specialization is for when T is not trivially copy constructible
template <class T>
struct optional_copy_base<T, false> : optional_operations_base<T> {
    using optional_operations_base<T>::optional_operations_base;

    optional_copy_base() = default;
    optional_copy_base(const optional_copy_base &rhs)
        : optional_operations_base<T>() {
        if (rhs.has_value()) {
            this->construct(rhs.get());
        } else {
            this->m_has_value = false;
        }
    }

    optional_copy_base(optional_copy_base &&rhs) = default;
    optional_copy_base &operator=(const optional_copy_base &rhs) = default;
    optional_copy_base &operator=(optional_copy_base &&rhs) = default;
};

// This class manages conditionally having a trivial move constructor
// Unfortunately there's no way to achieve this in GCC < 5 AFAIK, since it
// doesn't implement an analogue to std::is_trivially_move_constructible. We
// have to make do with a non-trivial move constructor even if T is trivially
// move constructible
#ifndef TL_OPTIONAL_GCC49
template <class T, bool = std::is_trivially_move_constructible<T>::value>
struct optional_move_base : optional_copy_base<T> {
    using optional_copy_base<T>::optional_copy_base;
};
#else
template <class T, bool = false> struct optional_move_base;
#endif
template <class T> struct optional_move_base<T, false> : optional_copy_base<T> {
    using optional_copy_base<T>::optional_copy_base;

    optional_move_base() = default;
    optional_move_base(const optional_move_base &rhs) = default;

    optional_move_base(optional_move_base &&rhs) noexcept(
        std::is_nothrow_move_constructible<T>::value) {
        if (rhs.has_value()) {
            this->construct(std::move(rhs.get()));
        } else {
            this->m_has_value = false;
        }
    }
    optional_move_base &operator=(const optional_move_base &rhs) = default;
    optional_move_base &operator=(optional_move_base &&rhs) = default;
};

// This class manages conditionally having a trivial copy assignment operator
template <class T, bool = TL_OPTIONAL_IS_TRIVIALLY_COPY_ASSIGNABLE(T) &&
                          TL_OPTIONAL_IS_TRIVIALLY_COPY_CONSTRUCTIBLE(T) &&
                          TL_OPTIONAL_IS_TRIVIALLY_DESTRUCTIBLE(T)>
struct optional_copy_assign_base : optional_move_base<T> {
    using optional_move_base<T>::optional_move_base;
};

template <class T>
struct optional_copy_assign_base<T, false> : optional_move_base<T> {
    using optional_move_base<T>::optional_move_base;

    optional_copy_assign_base() = default;
    optional_copy_assign_base(const optional_copy_assign_base &rhs) = default;

    optional_copy_assign_base(optional_copy_assign_base &&rhs) = default;
    optional_copy_assign_base &operator=(const optional_copy_assign_base &rhs) {
        this->assign(rhs);
        return *this;
    }
    optional_copy_assign_base &
    operator=(optional_copy_assign_base &&rhs) = default;
};

// This class manages conditionally having a trivial move assignment operator
// Unfortunately there's no way to achieve this in GCC < 5 AFAIK, since it
// doesn't implement an analogue to std::is_trivially_move_assignable. We have
// to make do with a non-trivial move assignment operator even if T is trivially
// move assignable
#ifndef TL_OPTIONAL_GCC49
template <class T, bool = std::is_trivially_destructible<T>::value
                       &&std::is_trivially_move_constructible<T>::value
                           &&std::is_trivially_move_assignable<T>::value>
struct optional_move_assign_base : optional_copy_assign_base<T> {
    using optional_copy_assign_base<T>::optional_copy_assign_base;
};
#else
template <class T, bool = false> struct optional_move_assign_base;
#endif

template <class T>
struct optional_move_assign_base<T, false> : optional_copy_assign_base<T> {
    using optional_copy_assign_base<T>::optional_copy_assign_base;

    optional_move_assign_base() = default;
    optional_move_assign_base(const optional_move_assign_base &rhs) = default;

    optional_move_assign_base(optional_move_assign_base &&rhs) = default;

    optional_move_assign_base &
    operator=(const optional_move_assign_base &rhs) = default;

    optional_move_assign_base &
    operator=(optional_move_assign_base &&rhs) noexcept(
        std::is_nothrow_move_constructible<T>::value
            &&std::is_nothrow_move_assignable<T>::value) {
        this->assign(std::move(rhs));
        return *this;
    }
};

// optional_delete_ctor_base will conditionally delete copy and move
// constructors depending on whether T is copy/move constructible
template <class T, bool EnableCopy = std::is_copy_constructible<T>::value,
          bool EnableMove = std::is_move_constructible<T>::value>
struct optional_delete_ctor_base {
    optional_delete_ctor_base() = default;
    optional_delete_ctor_base(const optional_delete_ctor_base &) = default;
    optional_delete_ctor_base(optional_delete_ctor_base &&) noexcept = default;
    optional_delete_ctor_base &
    operator=(const optional_delete_ctor_base &) = default;
    optional_delete_ctor_base &
    operator=(optional_delete_ctor_base &&) noexcept = default;
};

template <class T> struct optional_delete_ctor_base<T, true, false> {
    optional_delete_ctor_base() = default;
    optional_delete_ctor_base(const optional_delete_ctor_base &) = default;
    optional_delete_ctor_base(optional_delete_ctor_base &&) noexcept = delete;
    optional_delete_ctor_base &
    operator=(const optional_delete_ctor_base &) = default;
    optional_delete_ctor_base &
    operator=(optional_delete_ctor_base &&) noexcept = default;
};

template <class T> struct optional_delete_ctor_base<T, false, true> {
    optional_delete_ctor_base() = default;
    optional_delete_ctor_base(const optional_delete_ctor_base &) = delete;
    optional_delete_ctor_base(optional_delete_ctor_base &&) noexcept = default;
    optional_delete_ctor_base &
    operator=(const optional_delete_ctor_base &) = default;
    optional_delete_ctor_base &
    operator=(optional_delete_ctor_base &&) noexcept = default;
};

template <class T> struct optional_delete_ctor_base<T, false, false> {
    optional_delete_ctor_base() = default;
    optional_delete_ctor_base(const optional_delete_ctor_base &) = delete;
    optional_delete_ctor_base(optional_delete_ctor_base &&) noexcept = delete;
    optional_delete_ctor_base &
    operator=(const optional_delete_ctor_base &) = default;
    optional_delete_ctor_base &
    operator=(optional_delete_ctor_base &&) noexcept = default;
};

// optional_delete_assign_base will conditionally delete copy and move
// constructors depending on whether T is copy/move constructible + assignable
template <class T,
          bool EnableCopy = (std::is_copy_constructible<T>::value &&
                             std::is_copy_assignable<T>::value),
          bool EnableMove = (std::is_move_constructible<T>::value &&
                             std::is_move_assignable<T>::value)>
struct optional_delete_assign_base {
    optional_delete_assign_base() = default;
    optional_delete_assign_base(const optional_delete_assign_base &) = default;
    optional_delete_assign_base(optional_delete_assign_base &&) noexcept =
        default;
    optional_delete_assign_base &
    operator=(const optional_delete_assign_base &) = default;
    optional_delete_assign_base &
    operator=(optional_delete_assign_base &&) noexcept = default;
};

template <class T> struct optional_delete_assign_base<T, true, false> {
    optional_delete_assign_base() = default;
    optional_delete_assign_base(const optional_delete_assign_base &) = default;
    optional_delete_assign_base(optional_delete_assign_base &&) noexcept =
        default;
    optional_delete_assign_base &
    operator=(const optional_delete_assign_base &) = default;
    optional_delete_assign_base &
    operator=(optional_delete_assign_base &&) noexcept = delete;
};

template <class T> struct optional_delete_assign_base<T, false, true> {
    optional_delete_assign_base() = default;
    optional_delete_assign_base(const optional_delete_assign_base &) = default;
    optional_delete_assign_base(optional_delete_assign_base &&) noexcept =
        default;
    optional_delete_assign_base &
    operator=(const optional_delete_assign_base &) = delete;
    optional_delete_assign_base &
    operator=(optional_delete_assign_base &&) noexcept = default;
};

template <class T> struct optional_delete_assign_base<T, false, false> {
    optional_delete_assign_base() = default;
    optional_delete_assign_base(const optional_delete_assign_base &) = default;
    optional_delete_assign_base(optional_delete_assign_base &&) noexcept =
        default;
    optional_delete_assign_base &
    operator=(const optional_delete_assign_base &) = delete;
    optional_delete_assign_base &
    operator=(optional_delete_assign_base &&) noexcept = delete;
};

} // namespace hpp_detail

/// A tag type to represent an empty optional
struct nullopt_t {
    struct do_not_use {};
    constexpr explicit nullopt_t(do_not_use, do_not_use) noexcept {}
};
/// Represents an empty optional
static constexpr nullopt_t nullopt{nullopt_t::do_not_use{},
                                   nullopt_t::do_not_use{}};

class bad_optional_access : public std::exception {
public:
    bad_optional_access() = default;
    const char *what() const noexcept { return "Optional has no value"; }
};

/// An optional object is an object that contains the storage for another
/// object and manages the lifetime of this contained object, if any. The
/// contained object may be initialized after the optional object has been
/// initialized, and may be destroyed before the optional object has been
/// destroyed. The initialization state of the contained object is tracked by
/// the optional object.
template <class T>
class optional : private hpp_detail::optional_move_assign_base<T>,
                 private hpp_detail::optional_delete_ctor_base<T>,
                 private hpp_detail::optional_delete_assign_base<T> {
    using base = hpp_detail::optional_move_assign_base<T>;

    static_assert(!std::is_same<T, in_place_t>::value,
                  "instantiation of optional with in_place_t is ill-formed");
    static_assert(!std::is_same<hpp_detail::decay_t<T>, nullopt_t>::value,
                  "instantiation of optional with nullopt_t is ill-formed");

public:
// The different versions for C++14 and 11 are needed because deduced return
// types are not SFINAE-safe. This provides better support for things like
// generic lambdas. C.f.
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0826r0.html
#if defined(TL_OPTIONAL_CXX14) && !defined(TL_OPTIONAL_GCC49) &&               \
    !defined(TL_OPTIONAL_GCC54) && !defined(TL_OPTIONAL_GCC55)
    /// Carries out some operation which returns an optional on the stored
    /// object if there is one.
    template <class F> TL_OPTIONAL_11_CONSTEXPR auto and_then(F &&f) & {
        using result = hpp_detail::invoke_result_t<F, T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

    template <class F> TL_OPTIONAL_11_CONSTEXPR auto and_then(F &&f) && {
        using result = hpp_detail::invoke_result_t<F, T &&>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : result(nullopt);
    }

    template <class F> constexpr auto and_then(F &&f) const & {
        using result = hpp_detail::invoke_result_t<F, const T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F> constexpr auto and_then(F &&f) const && {
        using result = hpp_detail::invoke_result_t<F, const T &&>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : result(nullopt);
    }
#endif
#else
    /// Carries out some operation which returns an optional on the stored
    /// object if there is one.
    template <class F>
    TL_OPTIONAL_11_CONSTEXPR hpp_detail::invoke_result_t<F, T &> and_then(F &&f) & {
        using result = hpp_detail::invoke_result_t<F, T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

    template <class F>
    TL_OPTIONAL_11_CONSTEXPR hpp_detail::invoke_result_t<F, T &&> and_then(F &&f) && {
        using result = hpp_detail::invoke_result_t<F, T &&>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : result(nullopt);
    }

    template <class F>
    constexpr hpp_detail::invoke_result_t<F, const T &> and_then(F &&f) const & {
        using result = hpp_detail::invoke_result_t<F, const T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F>
    constexpr hpp_detail::invoke_result_t<F, const T &&> and_then(F &&f) const && {
        using result = hpp_detail::invoke_result_t<F, const T &&>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : result(nullopt);
    }
#endif
#endif

#if defined(TL_OPTIONAL_CXX14) && !defined(TL_OPTIONAL_GCC49) &&               \
    !defined(TL_OPTIONAL_GCC54) && !defined(TL_OPTIONAL_GCC55)
    /// Carries out some operation on the stored object if there is one.
    template <class F> TL_OPTIONAL_11_CONSTEXPR auto map(F &&f) & {
        return optional_map_impl(*this, std::forward<F>(f));
    }

    template <class F> TL_OPTIONAL_11_CONSTEXPR auto map(F &&f) && {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F> constexpr auto map(F &&f) const & {
        return optional_map_impl(*this, std::forward<F>(f));
    }

    template <class F> constexpr auto map(F &&f) const && {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }
#else
    /// Carries out some operation on the stored object if there is one.
    template <class F>
    TL_OPTIONAL_11_CONSTEXPR decltype(optional_map_impl(std::declval<optional &>(),
                                                        std::declval<F &&>()))
    map(F &&f) & {
        return optional_map_impl(*this, std::forward<F>(f));
    }

    template <class F>
    TL_OPTIONAL_11_CONSTEXPR decltype(optional_map_impl(std::declval<optional &&>(),
                                                        std::declval<F &&>()))
    map(F &&f) && {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F>
    constexpr decltype(optional_map_impl(std::declval<const optional &>(),
                                         std::declval<F &&>()))
    map(F &&f) const & {
        return optional_map_impl(*this, std::forward<F>(f));
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F>
    constexpr decltype(optional_map_impl(std::declval<const optional &&>(),
                                         std::declval<F &&>()))
    map(F &&f) const && {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }
#endif
#endif

#if defined(TL_OPTIONAL_CXX14) && !defined(TL_OPTIONAL_GCC49) &&               \
    !defined(TL_OPTIONAL_GCC54) && !defined(TL_OPTIONAL_GCC55)
    /// Carries out some operation on the stored object if there is one.
    template <class F> TL_OPTIONAL_11_CONSTEXPR auto transform(F&& f) & {
        return optional_map_impl(*this, std::forward<F>(f));
    }

    template <class F> TL_OPTIONAL_11_CONSTEXPR auto transform(F&& f) && {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F> constexpr auto transform(F&& f) const & {
        return optional_map_impl(*this, std::forward<F>(f));
    }

    template <class F> constexpr auto transform(F&& f) const && {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }
#else
    /// Carries out some operation on the stored object if there is one.
    template <class F>
    TL_OPTIONAL_11_CONSTEXPR decltype(optional_map_impl(std::declval<optional&>(),
                                                        std::declval<F&&>()))
    transform(F&& f) & {
        return optional_map_impl(*this, std::forward<F>(f));
    }

    template <class F>
    TL_OPTIONAL_11_CONSTEXPR decltype(optional_map_impl(std::declval<optional&&>(),
                                                        std::declval<F&&>()))
    transform(F&& f) && {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F>
    constexpr decltype(optional_map_impl(std::declval<const optional&>(),
                                         std::declval<F&&>()))
    transform(F&& f) const & {
        return optional_map_impl(*this, std::forward<F>(f));
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F>
    constexpr decltype(optional_map_impl(std::declval<const optional&&>(),
                                         std::declval<F&&>()))
    transform(F&& f) const && {
        return optional_map_impl(std::move(*this), std::forward<F>(f));
    }
#endif
#endif

    /// Calls `f` if the optional is empty
    template <class F, hpp_detail::enable_if_ret_void<F> * = nullptr>
    optional<T> TL_OPTIONAL_11_CONSTEXPR or_else(F &&f) & {
        if (has_value())
            return *this;

        std::forward<F>(f)();
        return nullopt;
    }

    template <class F, hpp_detail::disable_if_ret_void<F> * = nullptr>
    optional<T> TL_OPTIONAL_11_CONSTEXPR or_else(F &&f) & {
        return has_value() ? *this : std::forward<F>(f)();
    }

    template <class F, hpp_detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F &&f) && {
        if (has_value())
            return std::move(*this);

        std::forward<F>(f)();
        return nullopt;
    }

    template <class F, hpp_detail::disable_if_ret_void<F> * = nullptr>
    optional<T> TL_OPTIONAL_11_CONSTEXPR or_else(F &&f) && {
        return has_value() ? std::move(*this) : std::forward<F>(f)();
    }

    template <class F, hpp_detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F &&f) const & {
        if (has_value())
            return *this;

        std::forward<F>(f)();
        return nullopt;
    }

    template <class F, hpp_detail::disable_if_ret_void<F> * = nullptr>
    optional<T> TL_OPTIONAL_11_CONSTEXPR or_else(F &&f) const & {
        return has_value() ? *this : std::forward<F>(f)();
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F, hpp_detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F &&f) const && {
        if (has_value())
            return std::move(*this);

        std::forward<F>(f)();
        return nullopt;
    }

    template <class F, hpp_detail::disable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F &&f) const && {
        return has_value() ? std::move(*this) : std::forward<F>(f)();
    }
#endif

    /// Maps the stored value with `f` if there is one, otherwise returns `u`.
    template <class F, class U> U map_or(F &&f, U &&u) & {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u);
    }

    template <class F, class U> U map_or(F &&f, U &&u) && {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u);
    }

    template <class F, class U> U map_or(F &&f, U &&u) const & {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u);
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F, class U> U map_or(F &&f, U &&u) const && {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u);
    }
#endif

    /// Maps the stored value with `f` if there is one, otherwise calls
    /// `u` and returns the result.
    template <class F, class U>
    hpp_detail::invoke_result_t<U> map_or_else(F &&f, U &&u) & {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u)();
    }

    template <class F, class U>
    hpp_detail::invoke_result_t<U> map_or_else(F &&f, U &&u) && {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u)();
    }

    template <class F, class U>
    hpp_detail::invoke_result_t<U> map_or_else(F &&f, U &&u) const & {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u)();
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F, class U>
    hpp_detail::invoke_result_t<U> map_or_else(F &&f, U &&u) const && {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u)();
    }
#endif

    /// Returns `u` if `*this` has a value, otherwise an empty optional.
    template <class U>
    constexpr optional<typename std::decay<U>::type> conjunction(U &&u) const {
        using result = optional<hpp_detail::decay_t<U>>;
        return has_value() ? result{u} : result{nullopt};
    }

    /// Returns `rhs` if `*this` is empty, otherwise the current value.
    TL_OPTIONAL_11_CONSTEXPR optional disjunction(const optional &rhs) & {
        return has_value() ? *this : rhs;
    }

    constexpr optional disjunction(const optional &rhs) const & {
        return has_value() ? *this : rhs;
    }

    TL_OPTIONAL_11_CONSTEXPR optional disjunction(const optional &rhs) && {
        return has_value() ? std::move(*this) : rhs;
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    constexpr optional disjunction(const optional &rhs) const && {
        return has_value() ? std::move(*this) : rhs;
    }
#endif

    TL_OPTIONAL_11_CONSTEXPR optional disjunction(optional &&rhs) & {
        return has_value() ? *this : std::move(rhs);
    }

    constexpr optional disjunction(optional &&rhs) const & {
        return has_value() ? *this : std::move(rhs);
    }

    TL_OPTIONAL_11_CONSTEXPR optional disjunction(optional &&rhs) && {
        return has_value() ? std::move(*this) : std::move(rhs);
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    constexpr optional disjunction(optional &&rhs) const && {
        return has_value() ? std::move(*this) : std::move(rhs);
    }
#endif

    /// Takes the value out of the optional, leaving it empty
    optional take() {
        optional ret = std::move(*this);
        reset();
        return ret;
    }

    using value_type = T;

    /// Constructs an optional that does not contain a value.
    constexpr optional() noexcept = default;

    constexpr optional(nullopt_t) noexcept {}

    /// Copy constructor
    ///
    /// If `rhs` contains a value, the stored value is direct-initialized with
    /// it. Otherwise, the constructed optional is empty.
    TL_OPTIONAL_11_CONSTEXPR optional(const optional &rhs) = default;

    /// Move constructor
    ///
    /// If `rhs` contains a value, the stored value is direct-initialized with
    /// it. Otherwise, the constructed optional is empty.
    TL_OPTIONAL_11_CONSTEXPR optional(optional &&rhs) = default;

    /// Constructs the stored value in-place using the given arguments.
    template <class... Args>
    constexpr explicit optional(
        hpp_detail::enable_if_t<std::is_constructible<T, Args...>::value, in_place_t>,
        Args &&... args)
        : base(in_place, std::forward<Args>(args)...) {}

    template <class U, class... Args>
    TL_OPTIONAL_11_CONSTEXPR explicit optional(
        hpp_detail::enable_if_t<std::is_constructible<T, std::initializer_list<U> &,
                                                  Args &&...>::value,
                            in_place_t>,
        std::initializer_list<U> il, Args &&... args) {
        this->construct(il, std::forward<Args>(args)...);
    }

    /// Constructs the stored value with `u`.
    template <
        class U = T,
        hpp_detail::enable_if_t<std::is_convertible<U &&, T>::value> * = nullptr,
        hpp_detail::enable_forward_value<T, U> * = nullptr>
    constexpr optional(U &&u) : base(in_place, std::forward<U>(u)) {}

    template <
        class U = T,
        hpp_detail::enable_if_t<!std::is_convertible<U &&, T>::value> * = nullptr,
        hpp_detail::enable_forward_value<T, U> * = nullptr>
    constexpr explicit optional(U &&u) : base(in_place, std::forward<U>(u)) {}

    /// Converting copy constructor.
    template <
        class U, hpp_detail::enable_from_other<T, U, const U &> * = nullptr,
        hpp_detail::enable_if_t<std::is_convertible<const U &, T>::value> * = nullptr>
    optional(const optional<U> &rhs) {
        if (rhs.has_value()) {
            this->construct(*rhs);
        }
    }

    template <class U, hpp_detail::enable_from_other<T, U, const U &> * = nullptr,
              hpp_detail::enable_if_t<!std::is_convertible<const U &, T>::value> * =
                  nullptr>
    explicit optional(const optional<U> &rhs) {
        if (rhs.has_value()) {
            this->construct(*rhs);
        }
    }

    /// Converting move constructor.
    template <
        class U, hpp_detail::enable_from_other<T, U, U &&> * = nullptr,
        hpp_detail::enable_if_t<std::is_convertible<U &&, T>::value> * = nullptr>
    optional(optional<U> &&rhs) {
        if (rhs.has_value()) {
            this->construct(std::move(*rhs));
        }
    }

    template <
        class U, hpp_detail::enable_from_other<T, U, U &&> * = nullptr,
        hpp_detail::enable_if_t<!std::is_convertible<U &&, T>::value> * = nullptr>
    explicit optional(optional<U> &&rhs) {
        if (rhs.has_value()) {
            this->construct(std::move(*rhs));
        }
    }

    /// Destroys the stored value if there is one.
    ~optional() = default;

    /// Assignment to empty.
    ///
    /// Destroys the current value if there is one.
    optional &operator=(nullopt_t) noexcept {
        if (has_value()) {
            this->m_value.~T();
            this->m_has_value = false;
        }

        return *this;
    }

    /// Copy assignment.
    ///
    /// Copies the value from `rhs` if there is one. Otherwise resets the stored
    /// value in `*this`.
    optional &operator=(const optional &rhs) = default;

    /// Move assignment.
    ///
    /// Moves the value from `rhs` if there is one. Otherwise resets the stored
    /// value in `*this`.
    optional &operator=(optional &&rhs) = default;

    /// Assigns the stored value from `u`, destroying the old value if there was
    /// one.
    template <class U = T, hpp_detail::enable_assign_forward<T, U> * = nullptr>
    optional &operator=(U &&u) {
        if (has_value()) {
            this->m_value = std::forward<U>(u);
        } else {
            this->construct(std::forward<U>(u));
        }

        return *this;
    }

    /// Converting copy assignment operator.
    ///
    /// Copies the value from `rhs` if there is one. Otherwise resets the stored
    /// value in `*this`.
    template <class U,
              hpp_detail::enable_assign_from_other<T, U, const U &> * = nullptr>
    optional &operator=(const optional<U> &rhs) {
        if (has_value()) {
            if (rhs.has_value()) {
                this->m_value = *rhs;
            } else {
                this->hard_reset();
            }
        }

        if (rhs.has_value()) {
            this->construct(*rhs);
        }

        return *this;
    }

    // TODO check exception guarantee
    /// Converting move assignment operator.
    ///
    /// Moves the value from `rhs` if there is one. Otherwise resets the stored
    /// value in `*this`.
    template <class U, hpp_detail::enable_assign_from_other<T, U, U> * = nullptr>
    optional &operator=(optional<U> &&rhs) {
        if (has_value()) {
            if (rhs.has_value()) {
                this->m_value = std::move(*rhs);
            } else {
                this->hard_reset();
            }
        }

        if (rhs.has_value()) {
            this->construct(std::move(*rhs));
        }

        return *this;
    }

    /// Constructs the value in-place, destroying the current one if there is
    /// one.
    template <class... Args> T &emplace(Args &&... args) {
        static_assert(std::is_constructible<T, Args &&...>::value,
                      "T must be constructible with Args");

        *this = nullopt;
        this->construct(std::forward<Args>(args)...);
        return value();
    }

    template <class U, class... Args>
    hpp_detail::enable_if_t<
        std::is_constructible<T, std::initializer_list<U> &, Args &&...>::value,
        T &>
    emplace(std::initializer_list<U> il, Args &&... args) {
        *this = nullopt;
        this->construct(il, std::forward<Args>(args)...);
        return value();
  }

    /// Swaps this optional with the other.
    ///
    /// If neither optionals have a value, nothing happens.
    /// If both have a value, the values are swapped.
    /// If one has a value, it is moved to the other and the movee is left
    /// valueless.
    void
    swap(optional &rhs) noexcept(std::is_nothrow_move_constructible<T>::value
                                     &&hpp_detail::is_nothrow_swappable<T>::value) {
        using std::swap;
        if (has_value()) {
            if (rhs.has_value()) {
                swap(**this, *rhs);
            } else {
                new (std::addressof(rhs.m_value)) T(std::move(this->m_value));
                this->m_value.T::~T();
            }
        } else if (rhs.has_value()) {
            new (std::addressof(this->m_value)) T(std::move(rhs.m_value));
            rhs.m_value.T::~T();
        }
        swap(this->m_has_value, rhs.m_has_value);
    }

    /// Returns a pointer to the stored value
    constexpr const T *operator->() const {
        return std::addressof(this->m_value);
    }

    TL_OPTIONAL_11_CONSTEXPR T *operator->() {
        return std::addressof(this->m_value);
    }

    /// Returns the stored value
    TL_OPTIONAL_11_CONSTEXPR T &operator*() & { return this->m_value; }

    constexpr const T &operator*() const & { return this->m_value; }

    TL_OPTIONAL_11_CONSTEXPR T &&operator*() && {
        return std::move(this->m_value);
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    constexpr const T &&operator*() const && { return std::move(this->m_value); }
#endif

    /// Returns whether or not the optional has a value
    constexpr bool has_value() const noexcept { return this->m_has_value; }

    constexpr explicit operator bool() const noexcept {
        return this->m_has_value;
    }

    /// Returns the contained value if there is one, otherwise throws bad_optional_access
    TL_OPTIONAL_11_CONSTEXPR T &value() & {
        if (has_value())
            return this->m_value;
        throw bad_optional_access();
    }
    TL_OPTIONAL_11_CONSTEXPR const T &value() const & {
        if (has_value())
            return this->m_value;
        throw bad_optional_access();
    }
    TL_OPTIONAL_11_CONSTEXPR T &&value() && {
        if (has_value())
            return std::move(this->m_value);
        throw bad_optional_access();
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    TL_OPTIONAL_11_CONSTEXPR const T &&value() const && {
        if (has_value())
            return std::move(this->m_value);
        throw bad_optional_access();
    }
#endif

    /// Returns the stored value if there is one, otherwise returns `u`
    template <class U> constexpr T value_or(U &&u) const & {
        static_assert(std::is_copy_constructible<T>::value &&
                          std::is_convertible<U &&, T>::value,
                      "T must be copy constructible and convertible from U");
        return has_value() ? **this : static_cast<T>(std::forward<U>(u));
    }

    template <class U> TL_OPTIONAL_11_CONSTEXPR T value_or(U &&u) && {
        static_assert(std::is_move_constructible<T>::value &&
                          std::is_convertible<U &&, T>::value,
                      "T must be move constructible and convertible from U");
        return has_value() ? **this : static_cast<T>(std::forward<U>(u));
    }

    /// Destroys the stored value if one exists, making the optional empty
    void reset() noexcept {
        if (has_value()) {
            this->m_value.~T();
            this->m_has_value = false;
        }
    }
}; // namespace hpp

/// Compares two optional objects
template <class T, class U>
inline constexpr bool operator==(const optional<T> &lhs,
                                 const optional<U> &rhs) {
    return lhs.has_value() == rhs.has_value() &&
           (!lhs.has_value() || *lhs == *rhs);
}
template <class T, class U>
inline constexpr bool operator!=(const optional<T> &lhs,
                                 const optional<U> &rhs) {
    return lhs.has_value() != rhs.has_value() ||
           (lhs.has_value() && *lhs != *rhs);
}
template <class T, class U>
inline constexpr bool operator<(const optional<T> &lhs,
                                const optional<U> &rhs) {
    return rhs.has_value() && (!lhs.has_value() || *lhs < *rhs);
}
template <class T, class U>
inline constexpr bool operator>(const optional<T> &lhs,
                                const optional<U> &rhs) {
    return lhs.has_value() && (!rhs.has_value() || *lhs > *rhs);
}
template <class T, class U>
inline constexpr bool operator<=(const optional<T> &lhs,
                                 const optional<U> &rhs) {
    return !lhs.has_value() || (rhs.has_value() && *lhs <= *rhs);
}
template <class T, class U>
inline constexpr bool operator>=(const optional<T> &lhs,
                                 const optional<U> &rhs) {
    return !rhs.has_value() || (lhs.has_value() && *lhs >= *rhs);
}

/// Compares an optional to a `nullopt`
template <class T>
inline constexpr bool operator==(const optional<T> &lhs, nullopt_t) noexcept {
    return !lhs.has_value();
}
template <class T>
inline constexpr bool operator==(nullopt_t, const optional<T> &rhs) noexcept {
    return !rhs.has_value();
}
template <class T>
inline constexpr bool operator!=(const optional<T> &lhs, nullopt_t) noexcept {
    return lhs.has_value();
}
template <class T>
inline constexpr bool operator!=(nullopt_t, const optional<T> &rhs) noexcept {
    return rhs.has_value();
}
template <class T>
inline constexpr bool operator<(const optional<T> &, nullopt_t) noexcept {
    return false;
}
template <class T>
inline constexpr bool operator<(nullopt_t, const optional<T> &rhs) noexcept {
    return rhs.has_value();
}
template <class T>
inline constexpr bool operator<=(const optional<T> &lhs, nullopt_t) noexcept {
    return !lhs.has_value();
}
template <class T>
inline constexpr bool operator<=(nullopt_t, const optional<T> &) noexcept {
    return true;
}
template <class T>
inline constexpr bool operator>(const optional<T> &lhs, nullopt_t) noexcept {
    return lhs.has_value();
}
template <class T>
inline constexpr bool operator>(nullopt_t, const optional<T> &) noexcept {
    return false;
}
template <class T>
inline constexpr bool operator>=(const optional<T> &, nullopt_t) noexcept {
    return true;
}
template <class T>
inline constexpr bool operator>=(nullopt_t, const optional<T> &rhs) noexcept {
    return !rhs.has_value();
}

/// Compares the optional with a value.
template <class T, class U>
inline constexpr bool operator==(const optional<T> &lhs, const U &rhs) {
    return lhs.has_value() ? *lhs == rhs : false;
}
template <class T, class U>
inline constexpr bool operator==(const U &lhs, const optional<T> &rhs) {
    return rhs.has_value() ? lhs == *rhs : false;
}
template <class T, class U>
inline constexpr bool operator!=(const optional<T> &lhs, const U &rhs) {
    return lhs.has_value() ? *lhs != rhs : true;
}
template <class T, class U>
inline constexpr bool operator!=(const U &lhs, const optional<T> &rhs) {
    return rhs.has_value() ? lhs != *rhs : true;
}
template <class T, class U>
inline constexpr bool operator<(const optional<T> &lhs, const U &rhs) {
    return lhs.has_value() ? *lhs < rhs : true;
}
template <class T, class U>
inline constexpr bool operator<(const U &lhs, const optional<T> &rhs) {
    return rhs.has_value() ? lhs < *rhs : false;
}
template <class T, class U>
inline constexpr bool operator<=(const optional<T> &lhs, const U &rhs) {
    return lhs.has_value() ? *lhs <= rhs : true;
}
template <class T, class U>
inline constexpr bool operator<=(const U &lhs, const optional<T> &rhs) {
    return rhs.has_value() ? lhs <= *rhs : false;
}
template <class T, class U>
inline constexpr bool operator>(const optional<T> &lhs, const U &rhs) {
    return lhs.has_value() ? *lhs > rhs : false;
}
template <class T, class U>
inline constexpr bool operator>(const U &lhs, const optional<T> &rhs) {
    return rhs.has_value() ? lhs > *rhs : true;
}
template <class T, class U>
inline constexpr bool operator>=(const optional<T> &lhs, const U &rhs) {
    return lhs.has_value() ? *lhs >= rhs : false;
}
template <class T, class U>
inline constexpr bool operator>=(const U &lhs, const optional<T> &rhs) {
    return rhs.has_value() ? lhs >= *rhs : true;
}

template <class T,
          hpp_detail::enable_if_t<std::is_move_constructible<T>::value> * = nullptr,
          hpp_detail::enable_if_t<hpp_detail::is_swappable<T>::value> * = nullptr>
void swap(optional<T> &lhs,
          optional<T> &rhs) noexcept(noexcept(lhs.swap(rhs))) {
    return lhs.swap(rhs);
}

namespace hpp_detail {
struct i_am_secret {};
} // namespace hpp_detail

template <class T = hpp_detail::i_am_secret, class U,
          class Ret =
              hpp_detail::conditional_t<std::is_same<T, hpp_detail::i_am_secret>::value,
                                    hpp_detail::decay_t<U>, T>>
inline constexpr optional<Ret> make_optional(U &&v) {
    return optional<Ret>(std::forward<U>(v));
}

template <class T, class... Args>
inline constexpr optional<T> make_optional(Args &&... args) {
    return optional<T>(in_place, std::forward<Args>(args)...);
}
template <class T, class U, class... Args>
inline constexpr optional<T> make_optional(std::initializer_list<U> il,
                                           Args &&... args) {
    return optional<T>(in_place, il, std::forward<Args>(args)...);
}

#if __cplusplus >= 201703L
template <class T> optional(T)->optional<T>;
#endif

/// \exclude
namespace hpp_detail {
#ifdef TL_OPTIONAL_CXX14
template <class Opt, class F,
          class Ret = decltype(hpp_detail::invoke(std::declval<F>(),
                                              *std::declval<Opt>())),
          hpp_detail::enable_if_t<!std::is_void<Ret>::value> * = nullptr>
constexpr auto optional_map_impl(Opt &&opt, F &&f) {
    return opt.has_value()
               ? hpp_detail::invoke(std::forward<F>(f), *std::forward<Opt>(opt))
               : optional<Ret>(nullopt);
}

template <class Opt, class F,
          class Ret = decltype(hpp_detail::invoke(std::declval<F>(),
                                              *std::declval<Opt>())),
          hpp_detail::enable_if_t<std::is_void<Ret>::value> * = nullptr>
auto optional_map_impl(Opt &&opt, F &&f) {
    if (opt.has_value()) {
        hpp_detail::invoke(std::forward<F>(f), *std::forward<Opt>(opt));
        return make_optional(monostate{});
    }

    return optional<monostate>(nullopt);
}
#else
template <class Opt, class F,
          class Ret = decltype(hpp_detail::invoke(std::declval<F>(),
                                              *std::declval<Opt>())),
          hpp_detail::enable_if_t<!std::is_void<Ret>::value> * = nullptr>

constexpr auto optional_map_impl(Opt &&opt, F &&f) -> optional<Ret> {
    return opt.has_value()
               ? hpp_detail::invoke(std::forward<F>(f), *std::forward<Opt>(opt))
               : optional<Ret>(nullopt);
}

template <class Opt, class F,
          class Ret = decltype(hpp_detail::invoke(std::declval<F>(),
                                              *std::declval<Opt>())),
          hpp_detail::enable_if_t<std::is_void<Ret>::value> * = nullptr>

auto optional_map_impl(Opt &&opt, F &&f) -> optional<monostate> {
    if (opt.has_value()) {
        hpp_detail::invoke(std::forward<F>(f), *std::forward<Opt>(opt));
        return monostate{};
    }

    return nullopt;
}
#endif
} // namespace hpp_detail

/// Specialization for when `T` is a reference. `optional<T&>` acts similarly
/// to a `T*`, but provides more operations and shows intent more clearly.
template <class T> class optional<T &> {
public:
// The different versions for C++14 and 11 are needed because deduced return
// types are not SFINAE-safe. This provides better support for things like
// generic lambdas. C.f.
// http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2017/p0826r0.html
#if defined(TL_OPTIONAL_CXX14) && !defined(TL_OPTIONAL_GCC49) &&               \
    !defined(TL_OPTIONAL_GCC54) && !defined(TL_OPTIONAL_GCC55)

    /// Carries out some operation which returns an optional on the stored
    /// object if there is one.
    template <class F> TL_OPTIONAL_11_CONSTEXPR auto and_then(F &&f) & {
        using result = hpp_detail::invoke_result_t<F, T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

    template <class F> TL_OPTIONAL_11_CONSTEXPR auto and_then(F &&f) && {
        using result = hpp_detail::invoke_result_t<F, T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

    template <class F> constexpr auto and_then(F &&f) const & {
        using result = hpp_detail::invoke_result_t<F, const T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F> constexpr auto and_then(F &&f) const && {
        using result = hpp_detail::invoke_result_t<F, const T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }
#endif
#else
    /// Carries out some operation which returns an optional on the stored
    /// object if there is one.
    template <class F>
    TL_OPTIONAL_11_CONSTEXPR hpp_detail::invoke_result_t<F, T &> and_then(F &&f) & {
        using result = hpp_detail::invoke_result_t<F, T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

    template <class F>
    TL_OPTIONAL_11_CONSTEXPR hpp_detail::invoke_result_t<F, T &> and_then(F &&f) && {
        using result = hpp_detail::invoke_result_t<F, T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

    template <class F>
    constexpr hpp_detail::invoke_result_t<F, const T &> and_then(F &&f) const & {
        using result = hpp_detail::invoke_result_t<F, const T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F>
    constexpr hpp_detail::invoke_result_t<F, const T &> and_then(F &&f) const && {
        using result = hpp_detail::invoke_result_t<F, const T &>;
        static_assert(hpp_detail::is_optional<result>::value,
                      "F must return an optional");

        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : result(nullopt);
    }
#endif
#endif

#if defined(TL_OPTIONAL_CXX14) && !defined(TL_OPTIONAL_GCC49) &&               \
    !defined(TL_OPTIONAL_GCC54) && !defined(TL_OPTIONAL_GCC55)
    /// Carries out some operation on the stored object if there is one.
    template <class F> TL_OPTIONAL_11_CONSTEXPR auto map(F &&f) & {
        return hpp_detail::optional_map_impl(*this, std::forward<F>(f));
    }

    template <class F> TL_OPTIONAL_11_CONSTEXPR auto map(F &&f) && {
        return hpp_detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F> constexpr auto map(F &&f) const & {
        return hpp_detail::optional_map_impl(*this, std::forward<F>(f));
    }

    template <class F> constexpr auto map(F &&f) const && {
        return hpp_detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }
#else
    /// Carries out some operation on the stored object if there is one.
    template <class F>
    TL_OPTIONAL_11_CONSTEXPR decltype(hpp_detail::optional_map_impl(std::declval<optional &>(),
                                                                std::declval<F &&>()))
    map(F &&f) & {
        return hpp_detail::optional_map_impl(*this, std::forward<F>(f));
    }

    template <class F>
    TL_OPTIONAL_11_CONSTEXPR decltype(hpp_detail::optional_map_impl(std::declval<optional &&>(),
                                                                std::declval<F &&>()))
    map(F &&f) && {
        return hpp_detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F>
    constexpr decltype(hpp_detail::optional_map_impl(std::declval<const optional &>(),
                                                 std::declval<F &&>()))
    map(F &&f) const & {
        return hpp_detail::optional_map_impl(*this, std::forward<F>(f));
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F>
    constexpr decltype(hpp_detail::optional_map_impl(std::declval<const optional &&>(),
                                                 std::declval<F &&>()))
    map(F &&f) const && {
        return hpp_detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }
#endif
#endif

#if defined(TL_OPTIONAL_CXX14) && !defined(TL_OPTIONAL_GCC49) &&               \
    !defined(TL_OPTIONAL_GCC54) && !defined(TL_OPTIONAL_GCC55)
    /// Carries out some operation on the stored object if there is one.
    template <class F> TL_OPTIONAL_11_CONSTEXPR auto transform(F&& f) & {
        return hpp_detail::optional_map_impl(*this, std::forward<F>(f));
    }

    template <class F> TL_OPTIONAL_11_CONSTEXPR auto transform(F&& f) && {
        return hpp_detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F> constexpr auto transform(F&& f) const & {
        return hpp_detail::optional_map_impl(*this, std::forward<F>(f));
    }

    template <class F> constexpr auto transform(F&& f) const && {
        return hpp_detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }
#else
    /// Carries out some operation on the stored object if there is one.
    template <class F>
    TL_OPTIONAL_11_CONSTEXPR decltype(hpp_detail::optional_map_impl(std::declval<optional&>(),
                                                                std::declval<F&&>()))
    transform(F&& f) & {
        return hpp_detail::optional_map_impl(*this, std::forward<F>(f));
    }

    /// \group map
    /// \synopsis template <class F> auto transform(F &&f) &&;
    template <class F>
    TL_OPTIONAL_11_CONSTEXPR decltype(hpp_detail::optional_map_impl(std::declval<optional&&>(),
                                                                std::declval<F&&>()))
    transform(F&& f) && {
        return hpp_detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }

    template <class F>
    constexpr decltype(hpp_detail::optional_map_impl(std::declval<const optional&>(),
                                                 std::declval<F&&>()))
    transform(F&& f) const & {
        return hpp_detail::optional_map_impl(*this, std::forward<F>(f));
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F>
    constexpr decltype(hpp_detail::optional_map_impl(std::declval<const optional&&>(),
                                                 std::declval<F&&>()))
    transform(F&& f) const && {
        return hpp_detail::optional_map_impl(std::move(*this), std::forward<F>(f));
    }
#endif
#endif

    /// Calls `f` if the optional is empty
    template <class F, hpp_detail::enable_if_ret_void<F> * = nullptr>
    optional<T> TL_OPTIONAL_11_CONSTEXPR or_else(F &&f) & {
        if (has_value())
            return *this;

        std::forward<F>(f)();
        return nullopt;
    }

    template <class F, hpp_detail::disable_if_ret_void<F> * = nullptr>
    optional<T> TL_OPTIONAL_11_CONSTEXPR or_else(F &&f) & {
        return has_value() ? *this : std::forward<F>(f)();
    }

    template <class F, hpp_detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F &&f) && {
        if (has_value())
            return std::move(*this);

        std::forward<F>(f)();
        return nullopt;
    }

    template <class F, hpp_detail::disable_if_ret_void<F> * = nullptr>
    optional<T> TL_OPTIONAL_11_CONSTEXPR or_else(F &&f) && {
        return has_value() ? std::move(*this) : std::forward<F>(f)();
    }

    template <class F, hpp_detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F &&f) const & {
        if (has_value())
            return *this;

        std::forward<F>(f)();
        return nullopt;
    }

    template <class F, hpp_detail::disable_if_ret_void<F> * = nullptr>
    optional<T> TL_OPTIONAL_11_CONSTEXPR or_else(F &&f) const & {
        return has_value() ? *this : std::forward<F>(f)();
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F, hpp_detail::enable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F &&f) const && {
        if (has_value())
            return std::move(*this);

        std::forward<F>(f)();
        return nullopt;
    }

    template <class F, hpp_detail::disable_if_ret_void<F> * = nullptr>
    optional<T> or_else(F &&f) const && {
        return has_value() ? std::move(*this) : std::forward<F>(f)();
    }
#endif

    /// Maps the stored value with `f` if there is one, otherwise returns `u`
    template <class F, class U> U map_or(F &&f, U &&u) & {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u);
    }

    template <class F, class U> U map_or(F &&f, U &&u) && {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u);
    }

    template <class F, class U> U map_or(F &&f, U &&u) const & {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u);
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F, class U> U map_or(F &&f, U &&u) const && {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u);
    }
#endif

    /// Maps the stored value with `f` if there is one, otherwise calls
    /// `u` and returns the result.
    template <class F, class U>
    hpp_detail::invoke_result_t<U> map_or_else(F &&f, U &&u) & {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u)();
    }

    template <class F, class U>
    hpp_detail::invoke_result_t<U> map_or_else(F &&f, U &&u) && {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u)();
    }

    template <class F, class U>
    hpp_detail::invoke_result_t<U> map_or_else(F &&f, U &&u) const & {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), **this)
                           : std::forward<U>(u)();
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    template <class F, class U>
    hpp_detail::invoke_result_t<U> map_or_else(F &&f, U &&u) const && {
        return has_value() ? hpp_detail::invoke(std::forward<F>(f), std::move(**this))
                           : std::forward<U>(u)();
    }
#endif

    /// Returns `u` if `*this` has a value, otherwise an empty optional.
    template <class U>
    constexpr optional<typename std::decay<U>::type> conjunction(U &&u) const {
        using result = optional<hpp_detail::decay_t<U>>;
        return has_value() ? result{u} : result{nullopt};
    }

    /// Returns `rhs` if `*this` is empty, otherwise the current value.
    TL_OPTIONAL_11_CONSTEXPR optional disjunction(const optional &rhs) & {
        return has_value() ? *this : rhs;
    }

    constexpr optional disjunction(const optional &rhs) const & {
        return has_value() ? *this : rhs;
    }

    TL_OPTIONAL_11_CONSTEXPR optional disjunction(const optional &rhs) && {
        return has_value() ? std::move(*this) : rhs;
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    constexpr optional disjunction(const optional &rhs) const && {
        return has_value() ? std::move(*this) : rhs;
    }
#endif

    TL_OPTIONAL_11_CONSTEXPR optional disjunction(optional &&rhs) & {
        return has_value() ? *this : std::move(rhs);
    }

    constexpr optional disjunction(optional &&rhs) const & {
        return has_value() ? *this : std::move(rhs);
    }

    TL_OPTIONAL_11_CONSTEXPR optional disjunction(optional &&rhs) && {
        return has_value() ? std::move(*this) : std::move(rhs);
    }

#ifndef TL_OPTIONAL_NO_CONSTRR
    constexpr optional disjunction(optional &&rhs) const && {
        return has_value() ? std::move(*this) : std::move(rhs);
    }
#endif

    /// Takes the value out of the optional, leaving it empty
    optional take() {
        optional ret = std::move(*this);
        reset();
        return ret;
    }

    using value_type = T &;

    /// Constructs an optional that does not contain a value.
    constexpr optional() noexcept : m_value(nullptr) {}

    constexpr optional(nullopt_t) noexcept : m_value(nullptr) {}

    /// Copy constructor
    ///
    /// If `rhs` contains a value, the stored value is direct-initialized with
    /// it. Otherwise, the constructed optional is empty.
    TL_OPTIONAL_11_CONSTEXPR optional(const optional &rhs) noexcept = default;

    /// Move constructor
    ///
    /// If `rhs` contains a value, the stored value is direct-initialized with
    /// it. Otherwise, the constructed optional is empty.
    TL_OPTIONAL_11_CONSTEXPR optional(optional &&rhs) = default;

    /// Constructs the stored value with `u`.
    template <class U = T,
              hpp_detail::enable_if_t<!hpp_detail::is_optional<hpp_detail::decay_t<U>>::value>
                  * = nullptr>
    constexpr optional(U &&u)  noexcept : m_value(std::addressof(u)) {
        static_assert(std::is_lvalue_reference<U>::value, "U must be an lvalue");
    }

    template <class U>
    constexpr explicit optional(const optional<U> &rhs) noexcept : optional(*rhs) {}

    /// No-op
    ~optional() = default;

    /// Assignment to empty.
    ///
    /// Destroys the current value if there is one.
    optional &operator=(nullopt_t) noexcept {
        m_value = nullptr;
        return *this;
    }

    /// Copy assignment.
    ///
    /// Rebinds this optional to the referee of `rhs` if there is one. Otherwise
    /// resets the stored value in `*this`.
    optional &operator=(const optional &rhs) = default;

    /// Rebinds this optional to `u`.
    template <class U = T,
              hpp_detail::enable_if_t<!hpp_detail::is_optional<hpp_detail::decay_t<U>>::value>
                  * = nullptr>
    optional &operator=(U &&u) {
        static_assert(std::is_lvalue_reference<U>::value, "U must be an lvalue");
        m_value = std::addressof(u);
        return *this;
    }

    /// Converting copy assignment operator.
    ///
    /// Rebinds this optional to the referee of `rhs` if there is one. Otherwise
    /// resets the stored value in `*this`.
    template <class U> optional &operator=(const optional<U> &rhs) noexcept {
        m_value = std::addressof(rhs.value());
        return *this;
    }

    /// Rebinds this optional to `u`.
    template <class U = T,
              hpp_detail::enable_if_t<!hpp_detail::is_optional<hpp_detail::decay_t<U>>::value>
                  * = nullptr>
    optional &emplace(U &&u) noexcept {
        return *this = std::forward<U>(u);
    }

    void swap(optional &rhs) noexcept { std::swap(m_value, rhs.m_value); }

    /// Returns a pointer to the stored value
    constexpr const T *operator->() const noexcept { return m_value; }

    TL_OPTIONAL_11_CONSTEXPR T *operator->() noexcept { return m_value; }

    /// Returns the stored value
    TL_OPTIONAL_11_CONSTEXPR T &operator*() noexcept { return *m_value; }

    constexpr const T &operator*() const noexcept { return *m_value; }

    constexpr bool has_value() const noexcept { return m_value != nullptr; }

    constexpr explicit operator bool() const noexcept {
        return m_value != nullptr;
    }

    /// Returns the contained value if there is one, otherwise throws bad_optional_access
    TL_OPTIONAL_11_CONSTEXPR T &value() {
        if (has_value())
            return *m_value;
        throw bad_optional_access();
    }
    TL_OPTIONAL_11_CONSTEXPR const T &value() const {
        if (has_value())
            return *m_value;
        throw bad_optional_access();
    }

    /// Returns the stored value if there is one, otherwise returns `u`
    template <class U> constexpr T value_or(U &&u) const & noexcept {
        static_assert(std::is_copy_constructible<T>::value &&
                          std::is_convertible<U &&, T>::value,
                      "T must be copy constructible and convertible from U");
        return has_value() ? **this : static_cast<T>(std::forward<U>(u));
    }

    /// \group value_or
    template <class U> TL_OPTIONAL_11_CONSTEXPR T value_or(U &&u) && noexcept {
        static_assert(std::is_move_constructible<T>::value &&
                          std::is_convertible<U &&, T>::value,
                      "T must be move constructible and convertible from U");
        return has_value() ? **this : static_cast<T>(std::forward<U>(u));
    }

    /// Destroys the stored value if one exists, making the optional empty
    void reset() noexcept { m_value = nullptr; }

private:
    T *m_value;
}; // namespace hpp



} // namespace hpp

namespace std {
// TODO SFINAE
template <class T> struct hash<hpp::optional<T>> {
    ::std::size_t operator()(const hpp::optional<T> &o) const {
        if (!o.has_value())
            return 0;

        return std::hash<hpp::hpp_detail::remove_const_t<T>>()(*o);
    }
};
} // namespace std

#endif // STX_NO_STD_OPTIONAL

#endif // STX_OPTIONAL_HPP_INCLUDED
