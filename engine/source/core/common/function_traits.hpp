#ifndef FUNCTION_TRAITS_HPP_INCLUDED
#define FUNCTION_TRAITS_HPP_INCLUDED


#include <functional>
#include <tuple>

namespace core
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
		typedef R result_type;
		typedef result_type return_type;
		typedef result_type function_type(Args...);
		enum
		{
			arity = sizeof...(Args)
		};

		typedef std::tuple<Args...> tuple_type;
		template <size_t i>
		struct arg
		{
			typedef typename std::tuple_element<i, tuple_type>::type type;
		};

		typedef std::tuple<special_decay_t<Args>...> tuple_type_decayed;
		template <size_t i>
		struct arg_decayed
		{
			typedef typename std::tuple_element<i, tuple_type_decayed>::type type;
		};
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

	template <typename Functor>
	using fn_result_of = typename function_traits<Functor>::result_type;
}

#endif // FUNCTION_TRAITS_HPP_INCLUDED