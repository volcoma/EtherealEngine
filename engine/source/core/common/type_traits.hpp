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
			static type_id_t id(&typeid(T));
#else
			static type_index_t id(&type_id_impl<T>);
#endif
			return id;
		}
	}

	template<typename T>
	const type_index_t& type_id()
	{
		//this is required to copy the behavior of typeid(T)
		return detail::type_id_impl<typename std::remove_cv<T>::type>();
	}

}

#endif