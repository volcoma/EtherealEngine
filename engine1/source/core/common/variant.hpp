#pragma once

#include "type_traits.hpp"
#include "tuple.hpp"

#include <type_traits>
#include <stdexcept>

namespace core
{

	// a generic, type-safe and discriminated union.
	template<typename ... Types> struct VariantResolver;

	template<typename ... Types> struct Variant
	{
		static_assert(sizeof...(Types) > 0, "template arguments type list of variant can not be empty.");

		constexpr static const size_t size = StaticMax<sizeof(Types)...>::value;
		constexpr static const size_t align = StaticMax<alignof(Types)...>::value;
		constexpr static const size_t type_size = sizeof...(Types);

		using resolver_t = VariantResolver<Types...>;
		using tuple_t = typename std::tuple<Types...>;
		using aligned_storage_t = typename std::aligned_storage<size, align>::type;

		Variant() : _index_t(invalid) {}

		// initialize variant with copy constructor
		Variant(const Variant& rhs)
		{
			_index_t = rhs._index_t;
			resolver_t::copy(_index_t, &rhs._data, &_data);
		}

		Variant(Variant&& rhs)
		{
			_index_t = rhs._index_t;
			resolver_t::move(_index_t, &rhs._data, &_data);
		}

		// copy-assignments
		Variant& operator = (const Variant& rhs)
		{
			resolver_t::destroy(_index_t, &_data);
			_index_t = rhs._index_t;
			resolver_t::copy(_index_t, &rhs._data, &_data);
			return *this;
		}

		// move-assignments
		Variant& operator = (Variant&& rhs)
		{
			resolver_t::destroy(_index_t, &_data);
			_index_t = rhs._index_t;
			resolver_t::move(_index_t, &rhs._data, &_data);
			return *this;
		}

		template<typename T, typename Enable = typename std::enable_if<TupleHas<T, tuple_t>::value>::type>
		bool is() const
		{
			return _index_t == TupleIndex<T, tuple_t>::value;
		}

		// returns the zero-based index into the set of bounded types of the contained type
		size_t which() const
		{
			return _index_t;
		}

		bool empty() const
		{
			return _index_t < type_size;
		}

		template<typename T, typename ... Args>
		typename std::enable_if<TupleHas<T, tuple_t>::value, T>::type& set(Args&& ... args)
		{
			resolver_t::destroy(_index_t, &_data);
			_index_t = TupleIndex<T, tuple_t>::value;
			new (&_data) T(std::forward<Args>(args)...);
			return *reinterpret_cast<T*>(&_data);
		}

		template<typename T>
		typename std::enable_if<TupleHas<T, tuple_t>::value, T>::type& get()
		{
			if (_index_t == TupleIndex<T, tuple_t>::value)
				return *reinterpret_cast<T*>(&_data);
			else
				throw std::runtime_error("failed to get T in variant.");
		}

		template<typename T>
		typename std::enable_if<TupleHas<T, tuple_t>::value, T>::type const& get() const
		{
			if (_index_t == TupleIndex<T, tuple_t>::value)
				return *reinterpret_cast<const T*>(&_data);
			else
				throw std::runtime_error("failed to get T in variant.");
		}

	protected:
		constexpr static const size_t invalid = size_t(-1);

		size_t _index_t;
		aligned_storage_t _data;
	};

	template<typename ... Types> struct VariantResolverReverse;

	template<typename T, typename ... Types> struct VariantResolverReverse<T, Types...>
	{
		constexpr static size_t position = sizeof...(Types);

		static void destroy(size_t index, void* data)
		{
			if (index == position)
				reinterpret_cast<T*>(data)->~T();
			else
				VariantResolverReverse<Types...>::destroy(index, data);
		}

		static void move(size_t index, void* from, void* to)
		{
			if (index == position)
				new (to) T(std::move(*reinterpret_cast<T*>(from)));
			else
				VariantResolverReverse<Types...>::move(index, from, to);
		}

		static void copy(size_t index, const void* from, void* to)
		{
			if (index == position)
				new (to) T(*reinterpret_cast<const T*>(from));
			else
				VariantResolverReverse<Types...>::copy(index, from, to);
		}
	};

	template<> struct VariantResolverReverse<>
	{
		static void destroy(size_t id, void* data) {}
		static void move(size_t id, void* from, void* to) {}
		static void copy(size_t id, const void* from, void* to) {}
	};

	template<typename ... Types> struct VariantResolver
	{
		constexpr static size_t size = sizeof...(Types);

		static void destroy(size_t index, void* data)
		{
			VariantResolverReverse<Types...>::destroy(size - index - 1, data);
		}

		static void move(size_t index, void* from, void* to)
		{
			VariantResolverReverse<Types...>::move(size - index - 1, from, to);
		}

		static void copy(size_t index, const void* from, void* to)
		{
			VariantResolverReverse<Types...>::copy(size - index - 1, from, to);
		}
	};

}