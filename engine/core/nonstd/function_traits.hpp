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

	template<typename T, size_t Index>
	using param_types_decayed_t = typename param_types_decayed<T, Index>::type;
	
	template<typename T, size_t Index>
	using param_types_t = typename param_types<T, Index>::type;
	
}



#endif // FUNCTION_TRAITS_HPP_INCLUDED