#pragma once

#include <tuple>

namespace core
{
	/// to support tuple unpacking
	template <typename T, T... ints> struct integer_sequence { };

	template <typename T, T N, typename = void>
	struct MakeIntegerSequenceImpl
	{
		template <typename> struct Resolver;

		template <T... Prev> struct Resolver<integer_sequence<T, Prev...>>
		{
			using type = integer_sequence<T, Prev..., N - 1>;
		};

		using type = typename Resolver<typename MakeIntegerSequenceImpl<T, N - 1>::type>::type;
	};

	template <typename T, T N>
	struct MakeIntegerSequenceImpl<T, N, typename std::enable_if<N == 0>::type>
	{
		using type = integer_sequence<T>;
	};

	template <typename T, T N>
	using make_integer_sequence = typename MakeIntegerSequenceImpl<T, N>::type;

	// returns the index of the first occurrence of a given type
	template<typename T, typename Tuple> struct TupleIndex;

	template<typename T> struct TupleIndex<T, std::tuple<>>
	{
		static const size_t value = size_t(-1);
	};

	template<typename T, typename ... Args> struct TupleIndex<T, std::tuple<T, Args...>>
	{
		static const size_t value = 0;
	};

	template<typename T, typename U, typename ... Args> struct TupleIndex<T, std::tuple<U, Args...>>
	{
		static const size_t value = 1 + TupleIndex<T, std::tuple<Args...>>::value;
	};

	// returns true if tuple has specfied type
	template<typename T, typename Tuple> struct TupleHas;

	template<typename T>
	struct TupleHas<T, std::tuple<>> : std::false_type
	{
		static const bool value = false;
	};

	template<typename T, typename U, typename ... Args>
	struct TupleHas<T, std::tuple<U, Args...>> : TupleHas<T, std::tuple<Args...>>
	{};

	template<typename T, typename ... Args>
	struct TupleHas<T, std::tuple<T, Args...>> : std::true_type
	{
		static const bool value = true;
	};
}