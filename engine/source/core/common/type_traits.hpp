#pragma once

#include <type_traits>

namespace nonstd
{

	// static max generator
	template<size_t S, size_t ... Args> struct static_max;

	template<size_t S> struct static_max<S>
	{
		static const size_t value = S;
	};

	template<size_t S1, size_t S2, size_t ... Args> struct static_max<S1, S2, Args...>
	{
		static const size_t value = S1 > S2 ? static_max<S1, Args...>::value : static_max<S2, Args...>::value;
	};

	// static all true
	template<bool...> struct boolean_value_pack;
	template<bool... Values> using AllTrue = std::is_same<
		boolean_value_pack<Values..., true>,
		boolean_value_pack<true, Values...>>;

	// incremental id of type
	struct type_info_polymorphic
	{
		using index_t = size_t;

		template<typename Base, typename Derived> 
		static index_t id()
		{
			static_assert(std::is_base_of<Base, Derived>::value, "D should be derived from B.");
			static index_t sid = counter<Base>::value++;
			return sid;
		}

	protected:
		template<typename Base>
		struct counter
		{
			static index_t value;
		};
	};

	template<typename B>
	type_info_polymorphic::index_t type_info_polymorphic::counter<B>::value = 0;

	struct type_info
	{
		using index_t = size_t;

		template<typename T> 
		static index_t id()
		{
			static index_t sid = get_counter()++;
			return sid;
		}
	protected:

		static index_t& get_counter()
		{
			static index_t value = 0;
			return value;
		};
	};

}