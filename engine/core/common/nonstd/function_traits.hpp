#ifndef FUNCTION_TRAITS_HPP_INCLUDED
#define FUNCTION_TRAITS_HPP_INCLUDED


#include <functional>
#include <tuple>
#include "sequence.hpp"
#include "type_traits.hpp"

namespace nonstd
{
	namespace detail
	{
		/*
		* invoke implemented as per the C++17 standard specification.
		*/
		template <class B, class T, class D, class ... Args>
		constexpr inline auto invoke_(T B::*f, D && d, Args && ... args)
			noexcept (noexcept (
			(std::forward <D>(d).*f) (std::forward <Args>(args)...)
				))
			-> typename std::enable_if <
			std::is_function <T>::value &&
			std::is_base_of <B, typename std::decay <D>::type>::value,
			decltype ((std::forward <D>(d).*f) (std::forward <Args>(args)...))
			>::type
		{
			return (std::forward <D>(d).*f) (std::forward <Args>(args)...);
		}

		template <class B, class T, class R, class ... Args>
		constexpr inline auto invoke_(T B::*f, R && r, Args && ... args)
			noexcept (noexcept (
			(r.get().*f) (std::forward <Args>(args)...)
				))
			-> typename std::enable_if <
			std::is_function <T>::value &&
			is_reference_wrapper <typename std::decay <R>::type>::value,
			decltype ((r.get().*f) (std::forward <Args>(args)...))
			>::type
		{
			return (r.get().*f) (std::forward <Args>(args)...);
		}

		template <class B, class T, class P, class ... Args>
		constexpr inline auto invoke_(T B::*f, P && p, Args && ... args)
			noexcept (noexcept (
			((*std::forward <P>(p)).*f) (std::forward <Args>(args)...)
				))
			-> typename std::enable_if <
			std::is_function <T>::value &&
			!is_reference_wrapper <typename std::decay <P>::type>::value &&
			!std::is_base_of <B, typename std::decay <P>::type>::value,
			decltype (((*std::forward <P>(p)).*f) (
				std::forward <Args>(args)...
				))
			>::type
		{
			return ((*std::forward <P>(p)).*f) (std::forward <Args>(args)...);
		}

		template <class B, class T, class D>
		constexpr inline auto invoke_(T B::*m, D && d)
			noexcept (noexcept (std::forward <D>(d).*m))
			-> typename std::enable_if <
			!std::is_function <T>::value &&
			std::is_base_of <B, typename std::decay <D>::type>::value,
			decltype (std::forward <D>(d).*m)
			>::type
		{
			return std::forward <D>(d).*m;
		}

		template <class B, class T, class R>
		constexpr inline auto invoke_(T B::*m, R && r)
			noexcept (noexcept (r.get().*m))
			-> typename std::enable_if <
			!std::is_function <T>::value &&
			is_reference_wrapper <typename std::decay <R>::type>::value,
			decltype (r.get().*m)
			>::type
		{
			return r.get().*m;
		}

		template <class B, class T, class P>
		constexpr inline auto invoke_(T B::*m, P && p)
			noexcept (noexcept ((*std::forward <P>(p)).*m))
			-> typename std::enable_if <
			!std::is_function <T>::value &&
			!is_reference_wrapper <typename std::decay <P>::type>::value &&
			!std::is_base_of <B, typename std::decay <P>::type>::value,
			decltype ((*std::forward <P>(p)).*m)
			>::type
		{
			return (*std::forward <P>(p)).*m;
		}

		template <class Callable, class ... Args>
		constexpr inline auto invoke_(Callable && c, Args && ... args)
			noexcept (noexcept (
				std::forward <Callable>(c) (std::forward <Args>(args)...)
				))
			-> decltype (
				std::forward <Callable>(c) (std::forward <Args>(args)...)
				)
		{
			return std::forward <Callable>(c) (std::forward <Args>(args)...);
		}

		/*
		* apply implemented as per the C++17 standard specification.
		*/
		template <class F, class T, std::size_t ... I>
		constexpr inline auto apply_(F && f, T && t, index_sequence <I...>)
			noexcept (noexcept (
				invoke_(
					std::forward <F>(f), std::get <I>(std::forward <T>(t))...
				)
				))
			-> decltype (
				invoke_(
					std::forward <F>(f), std::get <I>(std::forward <T>(t))...
				)
				)
		{
			return invoke_(
				std::forward <F>(f), std::get <I>(std::forward <T>(t))...
			);
		}
	}   // namespace detail

	template <class F, class ... Args>
	constexpr inline auto invoke(F && f, Args && ... args)
		noexcept (noexcept (
			detail::invoke_(std::forward <F>(f),
				std::forward <Args>(args)...)
			))
		-> decltype (
			detail::invoke_(std::forward <F>(f),
				std::forward <Args>(args)...)
			)
	{
		return detail::invoke_(
			std::forward <F>(f), std::forward <Args>(args)...
		);
	}

	template <class F, class T>
	constexpr inline auto apply(F && f, T && t)
		noexcept (noexcept (
			detail::apply_(
				std::forward <F>(f), std::forward <T>(t),
				make_index_sequence <std::tuple_size <
				typename std::decay <T>::type
				>::value> {}
	)
			))
		-> decltype (
			detail::apply_(
				std::forward <F>(f), std::forward <T>(t),
				make_index_sequence <std::tuple_size <
				typename std::decay <T>::type
				>::value> {}
	)
			)
	{
		return detail::apply_(
			std::forward <F>(f), std::forward <T>(t),
			make_index_sequence <std::tuple_size <
			typename std::decay <T>::type
			>::value> {}
		);
	}



	template <class T>
	struct unwrap_refwrapper
	{
		using type = T;
	};

	template <class T>
	struct unwrap_refwrapper<std::reference_wrapper<T>>
	{
		using type = T&;
	};

	template <class T>
	using special_decay_t = typename unwrap_refwrapper<typename std::decay<T>::type>::type;


	template <typename Functor>
	struct function_traits : public function_traits<decltype(&Functor::operator())>
	{
	};

	template <typename R, typename... Args>
	struct function_traits<R(Args...)>
	{
		using result_type = R;
		using return_type =  result_type;
		typedef result_type function_type(Args...);
		static const std::size_t arity = sizeof...(Args);

		using arg_types = std::tuple<Args...>;
		using arg_types_decayed = std::tuple<special_decay_t<Args>...>;
	};

	template <typename R, typename... Args>
	struct function_traits<R(*)(Args...)> : public function_traits<R(Args...)>
	{
	};


	template <typename C, typename R, typename... Args>
	struct function_traits<R(C::*)(Args...)> : public function_traits<R(Args...)>
	{
		typedef C &owner_type;
	};

	template <typename C, typename R, typename... Args>
	struct function_traits<R(C::*)(Args...) const> : public function_traits<R(Args...)>
	{
		typedef const C &owner_type;
	};

	template <typename C, typename R, typename... Args>
	struct function_traits<R(C::*)(Args...) volatile> : public function_traits<R(Args...)>
	{
		typedef volatile C &owner_type;
	};

	template <typename C, typename R, typename... Args>
	struct function_traits<R(C::*)(Args...) const volatile> : public function_traits<R(Args...)>
	{
		typedef const volatile C &owner_type;
	};

	template <typename Functor>
	struct function_traits<std::function<Functor>> : public function_traits<Functor>
	{
	};

	template <typename T>
	struct function_traits<T &> : public function_traits<T>
	{
	};

	template <typename T>
	struct function_traits<const T &> : public function_traits<T>
	{
	};

	template <typename T>
	struct function_traits<volatile T &> : public function_traits<T>
	{
	};

	template <typename T>
	struct function_traits<const volatile T &> : public function_traits<T>
	{
	};

	template <typename T>
	struct function_traits<T &&> : public function_traits<T>
	{
	};

	template <typename T>
	struct function_traits<const T &&> : public function_traits<T>
	{
	};

	template <typename T>
	struct function_traits<volatile T &&> : public function_traits<T>
	{
	};

	template <typename T>
	struct function_traits<const volatile T &&> : public function_traits<T>
	{
	};

	/*
	* This seems to be slightly better than the standard library version as of now.
	* */

	template <typename F>
	using fn_result_of = typename function_traits<F>::result_type;

	/////////////////////////////////////////////////////////////////////////////////////
	// use it like e.g:
	// param_types<F, 0>::type

	template<typename F, size_t Index>
	struct param_types
	{
		using type = typename std::tuple_element<Index, typename function_traits<F>::arg_types>::type;
	};

	template<typename F, size_t Index>
	struct param_types_decayed
	{
		using type = typename std::tuple_element<Index, typename function_traits<F>::arg_types_decayed>::type;
	};

	template<typename T, size_t Index>
	using param_types_decayed_t = typename param_types_decayed<T, Index>::type;
	
	template<typename T, size_t Index>
	using param_types_t = typename param_types<T, Index>::type;
	
}



#endif // FUNCTION_TRAITS_HPP_INCLUDED