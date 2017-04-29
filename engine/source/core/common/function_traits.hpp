#ifndef FUNCTION_TRAITS_HPP_INCLUDED
#define FUNCTION_TRAITS_HPP_INCLUDED


#include <functional>
#include <tuple>

namespace nonstd
{
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
	
	/////////////////////////////////////////////////////////////////////////////////////
	namespace detail {

		template <class T>
		struct is_reference_wrapper : std::false_type {};
		template <class U>
		struct is_reference_wrapper<std::reference_wrapper<U>> : std::true_type {};
		template <class T>
		constexpr bool is_reference_wrapper_v = is_reference_wrapper<T>::value;

		template<class Base, class T, class Derived, class... Args>
		auto INVOKE(T Base::*pmf, Derived&& ref, Args&&...args)
			noexcept(noexcept((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...)))
			-> std::enable_if_t<std::is_function_v<T> &&
							std::is_base_of_v<Base, std::decay_t<Derived>>,
			decltype((std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...))>
		{
			return (std::forward<Derived>(ref).*pmf)(std::forward<Args>(args)...);
		}

		template<class Base, class T, class RefWrap, class... Args>
		auto INVOKE(T Base::*pmf, RefWrap&& ref, Args&&...args)
			noexcept(noexcept((ref.get().*pmf)(std::forward<Args>(args)...)))
			-> std::enable_if_t<std::is_function_v<T> &&
							is_reference_wrapper_v<std::decay_t<RefWrap>>,
			decltype((ref.get().*pmf)(std::forward<Args>(args)...))>
		{
			return (ref.get().*pmf)(std::forward<Args>(args)...);
		}

		template<class Base, class T, class Pointer, class... Args>
		auto INVOKE(T Base::*pmf, Pointer&& ptr, Args&&...args)
			noexcept(noexcept(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...)))
			-> std::enable_if_t<std::is_function_v<T> &&
							!is_reference_wrapper_v<std::decay_t<Pointer>> &&
							!std::is_base_of_v<Base, std::decay_t<Pointer>>,
			decltype(((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...))>
		{
			return ((*std::forward<Pointer>(ptr)).*pmf)(std::forward<Args>(args)...);
		}

		template<class Base, class T, class Derived>
		auto INVOKE(T Base::*pmf, Derived&& ref)
			noexcept(noexcept(std::forward<Derived>(ref).*pmf))
			-> std::enable_if_t<!std::is_function_v<T> &&
							std::is_base_of_v<Base, std::decay_t<Derived>>,
			decltype(std::forward<Derived>(ref).*pmf)>
		{
			return std::forward<Derived>(ref).*pmf;
		}

		template<class Base, class T, class RefWrap>
		auto INVOKE(T Base::*pmf, RefWrap&& ref)
			noexcept(noexcept(ref.get().*pmf))
			-> std::enable_if_t<!std::is_function_v<T> &&
							is_reference_wrapper_v<std::decay_t<RefWrap>>,
			decltype(ref.get().*pmf)>
		{
			return ref.get().*pmf;
		}

		template<class Base, class T, class Pointer>
		auto INVOKE(T Base::*pmd, Pointer&& ptr)
			noexcept(noexcept((*std::forward<Pointer>(ptr)).*pmd))
			-> std::enable_if_t<!std::is_function_v<T> &&
							!is_reference_wrapper_v<std::decay_t<Pointer>> &&
							!std::is_base_of_v<Base, std::decay_t<Pointer>>,
			decltype((*std::forward<Pointer>(ptr)).*pmd)>
		{
			return ((*std::forward<Pointer>(ptr)).*pmd);
		}

		template<class F, class... Args>
		auto INVOKE(F&& f, Args&&...args)
			noexcept(noexcept(std::forward<F>(f)(std::forward<Args>(args)...)))
			-> std::enable_if_t<!std::is_member_pointer_v<std::decay_t<F>>,
			decltype(std::forward<F>(f)(std::forward<Args>(args)...))>
		{
			return std::forward<F>(f)(std::forward<Args>(args)...);
		}
	}

	template< class F, class... ArgTypes >
	auto invoke(F&& f, ArgTypes&&... args)
		noexcept(noexcept(detail::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...)))
		->decltype(detail::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...))
	{
		return detail::INVOKE(std::forward<F>(f), std::forward<ArgTypes>(args)...);
	}
}



#endif // FUNCTION_TRAITS_HPP_INCLUDED