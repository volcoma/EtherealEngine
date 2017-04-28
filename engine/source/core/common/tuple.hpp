#pragma once

#include <tuple>
#include <type_traits>

namespace nonstd
{
	/// to support tuple unpacking
	template <typename T, T... ints> struct integer_sequence { };

	template <typename T, T N, typename = void>
	struct make_integer_sequence_impl
	{
		template <typename> struct Resolver;

		template <T... Prev> struct Resolver<integer_sequence<T, Prev...>>
		{
			using type = integer_sequence<T, Prev..., N - 1>;
		};

		using type = typename Resolver<typename make_integer_sequence_impl<T, N - 1>::type>::type;
	};

	template <typename T, T N>
	struct make_integer_sequence_impl<T, N, typename std::enable_if<N == 0>::type>
	{
		using type = integer_sequence<T>;
	};

	template <typename T, T N>
	using make_integer_sequence = typename make_integer_sequence_impl<T, N>::type;

	// returns the index of the first occurrence of a given type
	template<typename T, typename Tuple> struct tuple_index;

	template<typename T> struct tuple_index<T, std::tuple<>>
	{
		static const size_t value = size_t(-1);
	};

	template<typename T, typename ... Args> struct tuple_index<T, std::tuple<T, Args...>>
	{
		static const size_t value = 0;
	};

	template<typename T, typename U, typename ... Args> struct tuple_index<T, std::tuple<U, Args...>>
	{
		static const size_t value = 1 + tuple_index<T, std::tuple<Args...>>::value;
	};

	// returns true if tuple has specfied type
	template<typename T, typename Tuple> struct tuple_has;

	template<typename T>
	struct tuple_has<T, std::tuple<>> : std::false_type
	{
		static const bool value = false;
	};

	template<typename T, typename U, typename ... Args>
	struct tuple_has<T, std::tuple<U, Args...>> : tuple_has<T, std::tuple<Args...>>
	{};

	template<typename T, typename ... Args>
	struct tuple_has<T, std::tuple<T, Args...>> : std::true_type
	{
		static const bool value = true;
	};

}