#ifndef _NONSTD_TYPE_TRAITS_
#define _NONSTD_TYPE_TRAITS_

#include <type_traits>
#include <cstddef>

namespace nonstd
{

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


	/// Class template integer_sequence
	template<typename _Tp, _Tp... _Idx>
	struct integer_sequence
	{
		typedef _Tp value_type;
		static constexpr size_t size() { return sizeof...(_Idx); }
	};


	namespace detail
	{
		// Stores a tuple of indices.  Used by tuple and pair, and by bind() to
		// extract the elements in a tuple.
		template<size_t... _Indexes> struct _Index_tuple { };

		// Concatenates two _Index_tuples.
		template<typename _Itup1, typename _Itup2> struct _Itup_cat;

		template<size_t... _Ind1, size_t... _Ind2>
		struct _Itup_cat<_Index_tuple<_Ind1...>, _Index_tuple<_Ind2...>>
		{
			using __type = _Index_tuple<_Ind1..., (_Ind2 + sizeof...(_Ind1))...>;
		};

		// Builds an _Index_tuple<0, 1, 2, ..., _Num-1>.
		template<size_t _Num>
		struct _Build_index_tuple
			: _Itup_cat<typename _Build_index_tuple<_Num / 2>::__type,
			typename _Build_index_tuple<_Num - _Num / 2>::__type>
		{ };

		template<>
		struct _Build_index_tuple<1>
		{
			typedef _Index_tuple<0> __type;
		};

		template<>
		struct _Build_index_tuple<0>
		{
			typedef _Index_tuple<> __type;
		};

		template<typename _Tp, _Tp _Num,
			typename _ISeq = typename _Build_index_tuple<_Num>::__type>
			struct _Make_integer_sequence;

		template<typename _Tp, _Tp _Num, size_t... _Idx>
		struct _Make_integer_sequence<_Tp, _Num, _Index_tuple<_Idx...>>
		{
			static_assert(_Num >= 0,
				"Cannot make integer sequence of negative length");

			typedef integer_sequence<_Tp, static_cast<_Tp>(_Idx)...> __type;
		};
	}


	/// Alias template make_integer_sequence
	template<typename _Tp, _Tp _Num>
	using make_integer_sequence = typename detail::_Make_integer_sequence<_Tp, _Num>::__type;

	/// Alias template index_sequence
	template<size_t... _Idx>
	using index_sequence = integer_sequence<size_t, _Idx...>;

	/// Alias template make_index_sequence
	template<size_t _Num>
	using make_index_sequence = make_integer_sequence<size_t, _Num>;

	/// Alias template index_sequence_for
	template<typename... _Types>
	using index_sequence_for = make_index_sequence<sizeof...(_Types)>;

	static bool check_all_true() { return true; }

	template<typename... BoolArgs>
	static bool check_all_true(bool arg1, BoolArgs... args) { return arg1 & check_all_true(args...); }

	template <bool...> struct bool_pack;
	template <bool... v>
	using all_true = std::is_same<bool_pack<true, v...>, bool_pack<v..., true>>;

}

///////////////////////////////////////////////////////
#define __try_using_rtti__ 1
///////////////////////////////////////////////////////

#ifdef _MSC_VER
#ifndef __cpp_rtti
#define __cpp_rtti _CPPRTTI 
#endif // !__cpp_rtti
#endif

#define __cpp_rtti_enabled__ __try_using_rtti__ && __cpp_rtti
#if __cpp_rtti_enabled__
#include <typeindex>
#endif

namespace rtti
{
	class type_index_t;
	namespace detail
	{
		template<typename T>
		const type_index_t& type_id_impl();
	}

	class type_index_t
	{
#if __cpp_rtti_enabled__
		using construct_t = const std::type_info;
#else
		using construct_t = const type_index_t&();
#endif
		construct_t* _info = nullptr;
		type_index_t(construct_t* info) noexcept : _info{ info } {}

		template<typename T>
		friend const type_index_t& detail::type_id_impl();
	public:
		bool operator==(const type_index_t& o) const noexcept
		{
			return hash_code() == o.hash_code();
		}
		bool operator!=(const type_index_t& o) const noexcept
		{
			return hash_code() != o.hash_code();
		}
		bool operator<(const type_index_t& o) const noexcept
		{
			return hash_code() < o.hash_code();
		}
		bool operator>(const type_index_t& o) const noexcept
		{
			return hash_code() > o.hash_code();
		}

		std::size_t hash_code() const noexcept
		{
#if __cpp_rtti_enabled__
			return std::type_index(*_info).hash_code();
#else
			return reinterpret_cast<std::size_t>(_info);
#endif
		}
	};

	namespace detail
	{
		template<typename T>
		const type_index_t& type_id_impl()
		{
#if __cpp_rtti_enabled__
			static type_index_t id(&typeid(T));
#else
			static type_index_t id(&type_id_impl<T>);
#endif
			return id;
		}
	}

	template<typename T>
	const type_index_t& type_id()
	{
		//this is required to copy the behavior of typeid operator
		return detail::type_id_impl<typename std::remove_cv<T>::type>();
	}

}

#endif