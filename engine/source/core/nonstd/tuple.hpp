#pragma once

#include <tuple>
#include <type_traits>

namespace nonstd
{
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