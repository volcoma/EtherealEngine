#ifndef DELEGATE_HPP
#define DELEGATE_HPP
#pragma once

#include <assert.h>
#include <cstring>
#include <memory>

template <typename T>
class delegate;
template <class R, class... A>
class delegate<R(A...)>
{

	template <class C>
	using member_pair = std::pair<C* const, R (C::*const)(A...)>;

	template <class C>
	using const_member_pair = std::pair<C const* const, R (C::*const)(A...) const>;

	template <typename>
	struct is_member_pair : std::false_type
	{
	};

	template <class C>
	struct is_member_pair<std::pair<C* const, R (C::*const)(A...)>> : std::true_type
	{
	};

	template <typename>
	struct is_const_member_pair : std::false_type
	{
	};

	template <class C>
	struct is_const_member_pair<std::pair<C const* const, R (C::*const)(A...) const>> : std::true_type
	{
	};

	template <typename F>
	struct is_location_invariant : std::is_trivially_copyable<F>::type
	{
	};

	class undefined_class;

	struct pairtype
	{
		undefined_class* object;
		void (undefined_class::*member_pointer)();
	};

	union nocopy_types {
		void* object;
		const void* const_object;
		void (*function_pointer)();
		void (undefined_class::*member_pointer)();
		pairtype pair;
	};

	union any_data {
		void* access()
		{
			return &pod_data[0];
		}
		const void* access() const
		{
			return &pod_data[0];
		}

		template <typename F>
		F& access()
		{
			return *static_cast<F*>(access());
		}

		template <typename F>
		const F& access() const
		{
			return *static_cast<const F*>(access());
		}

		nocopy_types unused;
		char pod_data[sizeof(nocopy_types)];
	};

	enum manager_operation
	{
		get_functor_ptr,
		clone_functor,
		destroy_functor,
		compare_functor,
	};

	// Simple type wrapper that helps avoid annoying const problems
	// when casting between void pointers and pointers-to-pointers.
	template <typename F>
	struct simple_type_wrapper
	{
		simple_type_wrapper(F value)
			: _value(value)
		{
		}

		F _value;
	};

	template <typename F>
	struct is_location_invariant<simple_type_wrapper<F>> : is_location_invariant<F>
	{
	};

	constexpr static const std::size_t max_size = sizeof(nocopy_types);
	constexpr static const std::size_t max_align = alignof(nocopy_types);

	template <typename F>
	class base_manager
	{
	protected:
		constexpr static const bool stored_locally =
			(is_location_invariant<F>::value && sizeof(F) <= max_size && alignof(F) <= max_align &&
			 (max_align % alignof(F) == 0));

		using local_storage = std::integral_constant<bool, stored_locally>;
		using is_copiable = typename std::is_copy_constructible<F>::type;
		using is_movable = typename std::is_move_constructible<F>::type;
		using is_any_member_pair =
			std::integral_constant<bool, is_const_member_pair<F>::value || is_member_pair<F>::value>;

		using is_function_pointer =
			std::integral_constant<bool, std::is_pointer<F>::value &&
											 std::is_function<typename std::remove_pointer<F>::type>::value>;

	public:
		static void* get_pointer(const any_data& source)
		{
			return const_cast<F*>(get_pointer_typed(source));
		}

		static void clone(any_data& dest, const any_data& source)
		{
			clone_impl(dest, source, is_copiable());
		}

		static bool compare(const any_data& data1, const any_data& data2)
		{
			return compare_impl(data1, data2, is_any_member_pair());
		}

		static void destroy(any_data& victim)
		{
			destroy_impl(victim, local_storage());
		}

		static void init(any_data& _F, F&& f)
		{
			init_impl(_F, std::forward<F>(f), local_storage());
		}

		template <typename _Signature>
		static bool not_empty_function(const delegate<_Signature>& f)
		{
			return static_cast<bool>(f);
		}

		template <typename Func>
		static bool not_empty_function(Func* const& fp)
		{
			return fp;
		}

		template <typename _Class, typename Func>
		static bool not_empty_function(Func _Class::*const& mp)
		{
			return mp;
		}

		template <typename Func>
		static bool not_empty_function(const Func& /*unused*/)
		{
			return true;
		}

	private:
		// Retrieve a pointer to the function object
		static const F* get_pointer_typed(const any_data& source)
		{
			const F* ptr = stored_locally ? std::addressof(source.template access<F>())
										  /* have stored a pointer */
										  : source.template access<F*>();
			return ptr;
		}

		// Clone a function object that is not location-invariant or
		// that cannot fit into an any_data structure.
		static void clone_impl(any_data& /*unused*/, const any_data& /*unused*/, std::false_type /*unused*/)
		{
			assert(false && "TRYING TO COPY A NON-COPYABLE FUNCTOR/FUNCTION");
		}

		static void clone_impl(any_data& dest, const any_data& source, std::true_type /*unused*/)
		{
			clone_impl_local(dest, source, local_storage());
		}

		// Clone a location-invariant function object that fits within
		// an any_data structure.
		static void clone_impl_local(any_data& dest, const any_data& source, std::true_type /*unused*/)
		{
			new(dest.access()) F(source.template access<F>());
		}

		// Clone a function object that is not location-invariant or
		// that cannot fit into an any_data structure.
		static void clone_impl_local(any_data& dest, const any_data& source, std::false_type /*unused*/)
		{
			dest.template access<F*>() = new F(*source.template access<F*>());
		}

		// Compare a function object that is a member pair.
		static bool compare_impl_pair(const any_data& data1, const any_data& data2)
		{
			const F* pair1 = get_pointer_typed(data1);
			const auto& obj_ptr1 = pair1->first;
			const auto& func_ptr1 = pair1->second;

			const F* pair2 = get_pointer_typed(data2);
			const auto& obj_ptr2 = pair2->first;
			const auto& func_ptr2 = pair2->second;

			return obj_ptr1 == obj_ptr2 && func_ptr1 == func_ptr2;
		}

		static bool compare_impl_function(const any_data& data1, const any_data& data2, std::true_type /*unused*/)
		{
			const F* ptr1 = get_pointer_typed(data1);
			const F* ptr2 = get_pointer_typed(data2);

			return *ptr1 == *ptr2;
		}

		static bool compare_impl_function(const any_data& data1, const any_data& data2, std::false_type /*unused*/)
		{
			const F* ptr1 = get_pointer_typed(data1);
			const F* ptr2 = get_pointer_typed(data2);

			return ptr1 == ptr2;
		}

		static bool compare_impl(const any_data& data1, const any_data& data2, std::true_type /*unused*/)
		{
			return compare_impl_pair(data1, data2);
		}

		static bool compare_impl(const any_data& data1, const any_data& data2, std::false_type /*unused*/)
		{
			return compare_impl_function(data1, data2, is_function_pointer());
		}

		// Destroying a location-invariant object may still require
		// destruction.
		static void destroy_impl(any_data& victim, std::true_type /*unused*/)
		{
			victim.template access<F>().~F();
		}

		// Destroying an object located on the heap.
		static void destroy_impl(any_data& victim, std::false_type /*unused*/)
		{
			delete victim.template access<F*>();
		}

		static void init_impl(any_data& _F, F&& f, std::true_type /*unused*/)
		{
			new(_F.access()) F(std::forward<F>(f));
		}

		static void init_impl(any_data& _F, F&& f, std::false_type /*unused*/)
		{
			_F.template access<F*>() = new F(std::forward<F>(f));
		}
	};

	using invoker_type = R (*)(void* const, A...);
	using get_pointer_type = void* (*)(const any_data&);
	using clone_type = void (*)(any_data&, const any_data&);
	using compare_type = bool (*)(const any_data&, const any_data&);
	using destroy_type = void (*)(any_data&);

	struct manager
	{
		constexpr manager(get_pointer_type getter, clone_type cloner, compare_type comparer,
						  destroy_type destructor)
			: get_pointer_type_(getter)
			, clone_type_(cloner)
			, compare_type_(comparer)
			, destroy_type_(destructor)
		{
		}

		const get_pointer_type get_pointer_type_;
		const clone_type clone_type_;
		const compare_type compare_type_;
		const destroy_type destroy_type_;
	};
	using manager_type = const manager*;

	any_data functor_;
	manager_type manager_ = nullptr;
	invoker_type invoker_ = nullptr;

public:
	delegate() = default;

	~delegate()
	{
		if(manager_) {
			manager_->destroy_type_(functor_);
}
	}

	delegate(delegate const& rhs)
	{
		if(static_cast<bool>(rhs))
		{
			invoker_ = rhs.invoker_;
			manager_ = rhs.manager_;
			manager_->clone_type_(functor_, rhs.functor_);
		}
	}

	delegate(delegate&& d)
	{
		d.swap(*this);
	}

	delegate(std::nullptr_t const) noexcept
		: delegate()
	{
	}

	template <class C>
	delegate(C* const object_ptr, R (C::*const method_ptr)(A...))
	{
		*this = from(object_ptr, method_ptr);
	}

	template <class C>
	delegate(C* const object_ptr, R (C::*const method_ptr)(A...) const)
	{
		*this = from(object_ptr, method_ptr);
	}

	template <class C>
	delegate(C& object, R (C::*const method_ptr)(A...))
	{
		*this = from(object, method_ptr);
	}

	template <class C>
	delegate(C const& object, R (C::*const method_ptr)(A...) const)
	{
		*this = from(object, method_ptr);
	}

	template <typename T, typename = typename std::enable_if<
							  !std::is_same<delegate, typename std::decay<T>::type>::value>::type>
	delegate(T&& f)
	{
		using handler = base_manager<typename std::decay<T>::type>;
		using functor_type = typename std::decay<T>::type;

		if(handler::not_empty_function(std::forward<T>(f)))
		{
			handler::init(functor_, std::move(f));

			constexpr static const manager man{&handler::get_pointer, &handler::clone, &handler::compare,
											   &handler::destroy};

			invoker_ = &functor_stub<functor_type>;
			manager_ = &man;
		}
	}

	delegate& operator=(delegate const& d)
	{
		delegate(d).swap(*this);
		return *this;
	}

	delegate& operator=(delegate&& d)
	{
		delegate(std::move(d)).swap(*this);
		return *this;
	}

	delegate& operator=(std::nullptr_t)
	{
		if(manager_)
		{
			manager_->destroy_type_(functor_);
			manager_ = nullptr;
			invoker_ = nullptr;
		}
		return *this;
	}

	template <class C>
	delegate& operator=(R (C::*const rhs)(A...))
	{
		const void* object_ptr = get_object_ptr();
		return *this = from(static_cast<C*>(object_ptr), rhs);
	}

	template <class C>
	delegate& operator=(R (C::*const rhs)(A...) const)
	{
		const void* object_ptr = get_object_ptr();
		return *this = from(static_cast<C const*>(object_ptr), rhs);
	}

	template <typename T, typename = typename std::enable_if<
							  !std::is_same<delegate, typename std::decay<T>::type>::value>::type>
	delegate& operator=(T&& f)
	{
		delegate(std::forward<T>(f)).swap(*this);

		return *this;
	}

	template <R (*const function_ptr)(A...)>
	static delegate from(void) noexcept
	{
		return {nullptr, function_stub<function_ptr>};
	}

	template <class C, R (C::*const method_ptr)(A...)>
	static delegate from(C* const object_ptr) noexcept
	{
		return {object_ptr, method_stub<C, method_ptr>};
	}

	template <class C, R (C::*const method_ptr)(A...) const>
	static delegate from(C const* const object_ptr) noexcept
	{
		return {const_cast<C*>(object_ptr), const_method_stub<C, method_ptr>};
	}

	template <class C, R (C::*const method_ptr)(A...)>
	static delegate from(C& object) noexcept
	{
		return {&object, method_stub<C, method_ptr>};
	}

	template <class C, R (C::*const method_ptr)(A...) const>
	static delegate from(C const& object) noexcept
	{
		return {const_cast<C*>(&object), const_method_stub<C, method_ptr>};
	}

	template <typename T>
	static delegate from(T&& f)
	{
		return std::forward<T>(f);
	}

	static delegate from(R (*const function_ptr)(A...))
	{
		return function_ptr;
	}

	template <class C>
	static delegate from(C* const object_ptr, R (C::*const method_ptr)(A...))
	{
		return member_pair<C>(object_ptr, method_ptr);
	}

	template <class C>
	static delegate from(C const* const object_ptr, R (C::*const method_ptr)(A...) const)
	{
		return const_member_pair<C>(object_ptr, method_ptr);
	}

	template <class C>
	static delegate from(C& object, R (C::*const method_ptr)(A...))
	{
		return member_pair<C>(&object, method_ptr);
	}

	template <class C>
	static delegate from(C const& object, R (C::*const method_ptr)(A...) const)
	{
		return const_member_pair<C>(&object, method_ptr);
	}

	void swap(delegate& other) noexcept
	{
		std::swap(functor_, other.functor_);
		std::swap(manager_, other.manager_);
		std::swap(invoker_, other.invoker_);
	}

	bool operator==(delegate const& rhs) const noexcept
	{
		if(manager_ && manager_->compare_type_(functor_, rhs.functor_))
			return true;

		return false;
	}

	bool operator!=(delegate const& rhs) const noexcept
	{
		return !operator==(rhs);
	}

	bool operator<(delegate const& rhs) const noexcept
	{
		const void* object_ptr = get_object_ptr();
		const void* rhs_object_ptr = rhs.get_object_ptr();

		return (object_ptr < rhs_object_ptr) || ((object_ptr == rhs_object_ptr) && (invoker_ < rhs.invoker_));
	}

	bool operator==(std::nullptr_t const) const noexcept
	{
		return !invoker_;
	}

	bool operator!=(std::nullptr_t const) const noexcept
	{
		return invoker_;
	}

	explicit operator bool() const noexcept
	{
		return !!invoker_;
	}

	R operator()(A... args) const
	{
		void* object_ptr = get_object_ptr();
		assert(object_ptr);
		return invoker_(object_ptr, std::forward<A>(args)...);
	}

private:
	void* get_object_ptr() const
	{
		if(manager_)
		{
			return manager_->get_pointer_type_(functor_);
		}
		return nullptr;
	}

	friend struct std::hash<delegate>;

	template <R (*function_ptr)(A...)>
	static R function_stub(void* const, A... args)
	{
		return function_ptr(std::forward<A>(args)...);
	}

	template <class C, R (C::*method_ptr)(A...)>
	static R method_stub(void* const object_ptr, A... args)
	{
		return (static_cast<C*>(object_ptr)->*method_ptr)(std::forward<A>(args)...);
	}

	template <class C, R (C::*method_ptr)(A...) const>
	static R const_method_stub(void* const object_ptr, A... args)
	{
		return (static_cast<C const*>(object_ptr)->*method_ptr)(std::forward<A>(args)...);
	}

	template <typename T>
	static typename std::enable_if<!(is_member_pair<T>::value || is_const_member_pair<T>::value), R>::type
	functor_stub(void* const object_ptr, A... args)
	{
		return (*static_cast<T*>(object_ptr))(std::forward<A>(args)...);
	}

	template <typename T>
	static typename std::enable_if<is_member_pair<T>::value || is_const_member_pair<T>::value, R>::type
	functor_stub(void* const object_ptr, A... args)
	{
		return (static_cast<T*>(object_ptr)->first->*static_cast<T*>(object_ptr)->second)(
			std::forward<A>(args)...);
	}
};

namespace std
{
template <typename R, typename... A>
struct hash<delegate<R(A...)>>
{
	size_t operator()(::delegate<R(A...)> const& d) const noexcept
	{
		auto const seed(hash<void*>()(d.object_ptr_));
		return hash<decltype(d.invoker_)>()(d.invoker_) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
	}
};
} // namespace std

#endif // DELEGATE_HPP
