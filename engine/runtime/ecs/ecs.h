#pragma once

#include "core/common/assert.hpp"
#include "core/common/nonstd/type_traits.hpp"
#include "core/reflection/registration.h"
#include "core/serialization/serialization.h"
#include "core/signals/event.hpp"

#include <algorithm>
#include <bitset>
#include <cstdint>
#include <cstdlib>
#include <functional>
#include <iostream>
#include <iterator>
#include <memory>
#include <mutex>
#include <new>
#include <set>
#include <sstream>
#include <string>
#include <tuple>
#include <type_traits>
#include <utility>
#include <vector>

namespace runtime
{

namespace ecs
{
using frame_getter_t = std::function<std::uint64_t()>;
void set_frame_getter(frame_getter_t frame_getter);
std::uint64_t get_frame();
}

static const std::size_t MAX_COMPONENTS = 128;

class component;
class component_storage
{
public:
	component_storage(std::size_t size = 100);

	inline std::size_t size() const
	{
		return data.size();
	}
	inline std::size_t capacity() const
	{
		return data.capacity();
	}
	/// Ensure at least n elements will fit in the pool.
	void expand(std::size_t n);
	void reserve(std::size_t n);
	std::shared_ptr<component> get(std::size_t n) const;

	template <typename T>
	std::shared_ptr<T> get(std::size_t n) const
	{
		static_assert(std::is_base_of<component, T>::value, "Invalid component type.");

		return std::static_pointer_cast<T>(get(n));
	}

	void destroy(std::size_t n);

	template <typename T, typename... Args>
	std::weak_ptr<T> set(unsigned int index, Args&&... args)
	{
		auto element = std::make_shared<T>(std::forward<Args>(args)...);
		data[index] = std::move(element);
		return std::static_pointer_cast<T>(data[index]);
	}

	std::weak_ptr<component> set(unsigned int index, std::shared_ptr<component> component);

private:
	std::vector<std::shared_ptr<component>> data;
};

class entity_component_system;

template <typename C>
using chandle = std::weak_ptr<C>;

/** A convenience handle around an entity::Id.
 *
 * If an entity is destroyed, any copies will be invalidated. Use valid() to
 * check for validity before using.
 *
 * Create entities with `entity_component_system`:
 *
 *     entity entity = entity_manager->create();
 */
class entity
{
public:
	SERIALIZABLE(entity)

	struct id_t
	{
		id_t()
			: id_(0)
		{
		}
		explicit id_t(std::uint64_t id)
			: id_(id)
		{
		}
		id_t(std::uint32_t index, std::uint32_t version)
			: id_(std::uint64_t(index) | std::uint64_t(version) << 32UL)
		{
		}

		std::uint64_t id() const
		{
			return id_;
		}

		bool operator==(const id_t& other) const
		{
			return id_ == other.id_;
		}
		bool operator!=(const id_t& other) const
		{
			return id_ != other.id_;
		}
		bool operator<(const id_t& other) const
		{
			return id_ < other.id_;
		}

		std::uint32_t index() const
		{
			return id_ & 0xffffffffUL;
		}
		std::uint32_t version() const
		{
			return id_ >> 32;
		}

	private:
		friend class entity_component_system;

		std::uint64_t id_;
	};

	/**
	 * Id of an invalid entity.
	 */
	static const id_t INVALID;

	entity() = default;
	entity(entity_component_system* manager, entity::id_t id)
		: id_(id)
		, manager_(manager)
	{
	}
	entity(const entity& other) = default;
	entity& operator=(const entity& other) = default;

	/**
	 * Check if entity handle is invalid.
	 */
	operator bool() const
	{
		return valid();
	}

	bool operator==(const entity& other) const
	{
		return other.manager_ == manager_ && other.id_ == id_;
	}

	bool operator!=(const entity& other) const
	{
		return !(other == *this);
	}

	bool operator<(const entity& other) const
	{
		return other.id_ < id_;
	}
	void set_name(std::string name);
	const std::string& get_name() const;
	/**
	 * Is this entity handle valid?
	 *
	 * In older versions of EntityX, there were no guarantees around entity
	 * validity if a previously allocated entity slot was reassigned. That is no
	 * longer the case: if a slot is reassigned, old entity::Id's will be
	 * invalid.
	 */
	bool valid() const;

	/**
	 * Invalidate entity handle, disassociating it from an entity_component_system
	 * and invalidating its ID.
	 *
	 * Note that this does *not* affect the underlying entity and its
	 * components. Use destroy() to destroy the associated entity and components.
	 */
	void invalidate();

	std::string to_string() const;

	id_t id() const
	{
		return id_;
	}

	template <typename C, typename... Args>
	chandle<C> assign(Args&&... args);

	chandle<component> assign(std::shared_ptr<component> component);

	template <typename C>
	void remove();

	void remove(std::shared_ptr<component> component);

	template <typename C>
	chandle<C> get_component() const;

	template <typename... Components>
	std::tuple<chandle<Components>...> components() const;

	std::vector<chandle<component>> all_components() const;

	std::vector<std::shared_ptr<component>> all_components_shared() const;

	template <typename C>
	bool has_component() const;

	bool has_component(std::shared_ptr<component> component) const;

	template <typename A, typename... Args>
	void unpack(chandle<A>& a, chandle<Args>&... args);

	/**
	 * Destroy and invalidate this entity.
	 */
	void destroy();
	std::bitset<MAX_COMPONENTS> component_mask() const;

private:
	entity::id_t id_ = INVALID;
	entity_component_system* manager_ = nullptr;
};

class component : public std::enable_shared_from_this<component>
{
	REFLECTABLEV(component)
	SERIALIZABLE(component)
	friend class entity_component_system;

public:
	//-----------------------------------------------------------------------------
	//  Name : component ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	component()
	{
		touch();
	}

	//-----------------------------------------------------------------------------
	//  Name : component ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	component(const component&)
	{
		touch();
	}

	//-----------------------------------------------------------------------------
	//  Name : ~component (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~component();

	//-----------------------------------------------------------------------------
	//  Name : touch (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void touch()
	{
		_last_touched = static_cast<std::uint32_t>(ecs::get_frame()) + 1;
	}

	//-----------------------------------------------------------------------------
	//  Name : is_dirty (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool is_dirty() const
	{
		return _last_touched >= static_cast<std::uint32_t>(ecs::get_frame());
	}

	//-----------------------------------------------------------------------------
	//  Name : on_entity_set (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_entity_set()
	{
	}

	//-----------------------------------------------------------------------------
	//  Name : get_entity ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	entity get_entity()
	{
		return _entity;
	}

protected:
	//-----------------------------------------------------------------------------
	//  Name : runtime_id (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual rtti::type_index_sequential_t::index_t runtime_id() const = 0;
	/// Was the component touched.
	std::uint32_t _last_touched = 0;
	/// Owning entity
	entity _entity;
};

template <typename T>
class component_impl : public component
{
private:
	virtual rtti::type_index_sequential_t::index_t runtime_id() const
	{
		return static_id();
	}

public:
	component_impl() = default;
	component_impl(component_impl& rhs) = delete;
	component_impl& operator=(component_impl& rhs) = delete;

	static rtti::type_index_sequential_t::index_t static_id()
	{
		return rtti::type_index_sequential_t::id<component, T>();
	}

	chandle<T> handle()
	{
		return std::static_pointer_cast<T>(shared_from_this());
	}
};

extern event<void(entity)> on_entity_created;
extern event<void(entity)> on_entity_destroyed;
extern event<void(entity, chandle<component>)> on_component_added;
extern event<void(entity, chandle<component>)> on_component_removed;

/**
 * Manages entity::Id creation and component assignment.
 */
class entity_component_system
{
public:
	typedef std::bitset<MAX_COMPONENTS> component_mask_t;

	explicit entity_component_system();
	virtual ~entity_component_system();

	/// An iterator over a view of the entities in an entity_component_system.
	/// If All is true it will iterate over all valid entities and will ignore the
	/// entity mask.
	template <class Delegate, bool All = false>
	class view_iterator : public std::iterator<std::input_iterator_tag, entity::id_t>
	{
	public:
		Delegate& operator++()
		{
			++i_;
			next();
			return *static_cast<Delegate*>(this);
		}
		bool operator==(const Delegate& rhs) const
		{
			return i_ == rhs.i_;
		}
		bool operator!=(const Delegate& rhs) const
		{
			return i_ != rhs.i_;
		}
		entity operator*()
		{
			return entity(manager_, manager_->create_id(i_));
		}
		const entity operator*() const
		{
			return entity(manager_, manager_->create_id(i_));
		}

	protected:
		view_iterator(entity_component_system* manager, std::uint32_t index)
			: manager_(manager)
			, i_(index)
			, capacity_(manager_->capacity())
			, free_cursor_(~0UL)
		{
			if(All)
			{
				std::sort(manager_->free_list_.begin(), manager_->free_list_.end());
				free_cursor_ = 0;
			}
		}
		view_iterator(entity_component_system* manager, const component_mask_t mask, std::uint32_t index)
			: manager_(manager)
			, mask_(mask)
			, i_(index)
			, capacity_(manager_->capacity())
			, free_cursor_(~0UL)
		{
			if(All)
			{
				std::sort(manager_->free_list_.begin(), manager_->free_list_.end());
				free_cursor_ = 0;
			}
		}

		void next()
		{
			while(i_ < capacity_ && !predicate())
			{
				++i_;
			}

			if(i_ < capacity_)
			{
				entity entity = manager_->get(manager_->create_id(i_));
				static_cast<Delegate*>(this)->next_entity(entity);
			}
		}

		inline bool predicate()
		{
			return (All && valid_entity()) || (manager_->entity_component_mask_[i_] & mask_) == mask_;
		}

		inline bool valid_entity()
		{
			const std::vector<std::uint32_t>& free_list = manager_->free_list_;
			if(free_cursor_ < free_list.size() && free_list[free_cursor_] == i_)
			{
				++free_cursor_;
				return false;
			}
			return true;
		}

		entity_component_system* manager_;
		component_mask_t mask_;
		std::uint32_t i_;
		size_t capacity_;
		size_t free_cursor_;
	};

	template <bool All>
	class base_view
	{
	public:
		class iterator_type : public view_iterator<iterator_type, All>
		{
		public:
			iterator_type(entity_component_system* manager, const component_mask_t mask, std::uint32_t index)
				: view_iterator<iterator_type, All>(manager, mask, index)
			{
				view_iterator<iterator_type, All>::next();
			}

			void next_entity(entity&)
			{
			}
		};

		iterator_type begin()
		{
			return iterator_type(manager_, mask_, 0);
		}
		iterator_type end()
		{
			return iterator_type(manager_, mask_, std::uint32_t(manager_->capacity()));
		}
		const iterator_type begin() const
		{
			return iterator_type(manager_, mask_, 0);
		}
		const iterator_type end() const
		{
			return iterator_type(manager_, mask_, manager_->capacity());
		}

	private:
		friend class entity_component_system;

		explicit base_view(entity_component_system* manager)
			: manager_(manager)
		{
			mask_.set();
		}
		base_view(entity_component_system* manager, component_mask_t mask)
			: manager_(manager)
			, mask_(mask)
		{
		}

		entity_component_system* manager_;
		component_mask_t mask_;
	};

	template <bool All, typename... Components>
	class typed_view : public base_view<All>
	{
	public:
		template <typename T>
		struct identity
		{
			typedef T type;
		};

		void for_each(typename identity<std::function<void(entity entity, Components&...)>>::type f)
		{
			for(auto it : *this)
				f(it, *(it.template get_component<Components>().lock().get())...);
		}

	private:
		friend class entity_component_system;

		explicit typed_view(entity_component_system* manager)
			: base_view<All>(manager)
		{
		}
		typed_view(entity_component_system* manager, component_mask_t mask)
			: base_view<All>(manager, mask)
		{
		}
	};

	template <typename... Components>
	using View = typed_view<false, Components...>;

	template <typename... Components>
	class unpacking_view
	{
	public:
		struct unpacker
		{
			explicit unpacker(chandle<Components>&... handles)
				: handles(std::tuple<chandle<Components>&...>(handles...))
			{
			}

			void unpack(entity& entity) const
			{
				unpack_<0, Components...>(entity);
			}

		private:
			template <int N, typename C>
			void unpack_(entity& entity) const
			{
				std::get<N>(handles) = entity.get_component<C>();
			}

			template <int N, typename C0, typename C1, typename... Cn>
			void unpack_(entity& entity) const
			{
				std::get<N>(handles) = entity.get_component<C0>();
				unpack_<N + 1, C1, Cn...>(entity);
			}

			std::tuple<chandle<Components>&...> handles;
		};

		class iterator_type : public view_iterator<iterator_type>
		{
		public:
			iterator_type(entity_component_system* manager, const component_mask_t mask, std::uint32_t index,
						  const unpacker& unpacker)
				: view_iterator<iterator_type>(manager, mask, index)
				, unpacker_(unpacker)
			{
				view_iterator<iterator_type>::next();
			}

			void next_entity(entity& entity)
			{
				unpacker_.unpack(entity);
			}

		private:
			const unpacker& unpacker_;
		};

		iterator_type begin()
		{
			return iterator_type(manager_, mask_, 0, unpacker_);
		}
		iterator_type end()
		{
			return iterator_type(manager_, mask_, static_cast<std::uint32_t>(manager_->capacity()),
								 unpacker_);
		}
		const iterator_type begin() const
		{
			return iterator_type(manager_, mask_, 0, unpacker_);
		}
		const iterator_type end() const
		{
			return iterator_type(manager_, mask_, static_cast<std::uint32_t>(manager_->capacity()),
								 unpacker_);
		}

	private:
		friend class entity_component_system;

		unpacking_view(entity_component_system* manager, component_mask_t mask,
					   chandle<Components>&... handles)
			: manager_(manager)
			, mask_(mask)
			, unpacker_(handles...)
		{
		}

		entity_component_system* manager_;
		component_mask_t mask_;
		unpacker unpacker_;
	};

	/**
	 * Number of managed entities.
	 */
	size_t size() const;

	/**
	 * Current entity capacity.
	 */
	size_t capacity() const;

	/**
	 * Return true if the given entity ID is still valid.
	 */
	bool valid(entity::id_t id) const;

	/**
	 * Return true if the given entity ID is still valid.
	 */
	bool valid_index(std::uint32_t index) const;
	/**
	 * Create a new entity::Id.
	 *
	 * Emits EntityCreatedEvent.
	 */
	entity create();

	/**
	 * Destroy an existing entity::Id and its associated Components.
	 *
	 * Emits EntityDestroyedEvent.
	 */
	void destroy(entity::id_t id);

	entity get(entity::id_t id);

	/**
	 * Create an entity::Id for a slot.
	 *
	 * NOTE: Does *not* check for validity, but the entity::Id constructor will
	 * fail if the ID is invalid.
	 */
	entity::id_t create_id(std::uint32_t index) const;

	/**
	 * Assign a component to an entity::Id, passing through component constructor
	 * arguments.
	 *
	 *     Position &position = em.assign<Position>(e, x, y);
	 *
	 * @returns Smart pointer to newly created component.
	 */
	template <typename C, typename... Args>
	chandle<C> assign(entity::id_t id, Args&&... args)
	{
		return std::static_pointer_cast<C>(
			assign(id, std::make_shared<C>(std::forward<Args>(args)...)).lock());
	}

	chandle<component> assign(entity::id_t id, std::shared_ptr<component> comp);

	/**
	 * Remove a component from an entity::Id
	 *
	 * Emits a ComponentRemovedEvent<C> event.
	 */
	template <typename C>
	void remove(entity::id_t id)
	{
		remove(id, rtti::type_index_sequential_t::id<component, C>());
	}
	void remove(entity::id_t id, std::shared_ptr<component> component);
	void remove(entity::id_t id, const rtti::type_index_sequential_t::index_t family);

	/**
	 * Check if an entity has a component.
	 */
	template <typename C>
	bool has_component(entity::id_t id) const
	{
		return has_component(id, rtti::type_index_sequential_t::id<component, C>());
	}

	bool has_component(entity::id_t id, std::shared_ptr<component> component) const;

	bool has_component(entity::id_t id, rtti::type_index_sequential_t::index_t family) const;
	/**
	 * Retrieve a component assigned to an entity::Id.
	 *
	 * @returns Pointer to an instance of C, or nullptr if the entity::Id does not
	 * have that component.
	 */
	template <typename C>
	chandle<C> get_component(entity::id_t id)
	{
		assert_valid(id);
		auto family = rtti::type_index_sequential_t::id<component, C>();
		// We don't bother checking the component mask, as we return a nullptr
		// anyway.
		if(family >= component_pools_.size())
			return chandle<C>();
		auto& pool = component_pools_[family];
		if(!pool || !entity_component_mask_[id.index()][family])
			return chandle<C>();
		return chandle<C>(pool->template get<C>(id.index()));
	}

	template <typename... Components>
	std::tuple<chandle<Components>...> components(entity::id_t id)
	{
		return std::make_tuple(get_component<Components>(id)...);
	}

	std::vector<chandle<component>> all_components(entity::id_t id) const;
	std::vector<std::shared_ptr<component>> all_components_shared(entity::id_t id) const;
	/**
	 * Find Entities that have all of the specified Components.
	 *
	 * @code
	 * for (entity entity : entity_manager.entities_with_components<Position,
	 * Direction>()) {
	 *   chandle<Position> position = entity.get_component<Position>();
	 *   chandle<Direction> direction = entity.get_component<Direction>();
	 *
	 *   ...
	 * }
	 * @endcode
	 */
	template <typename... Components>
	View<Components...> entities_with_components()
	{
		auto mask = component_mask<Components...>();
		return View<Components...>(this, mask);
	}

	template <typename T>
	struct identity
	{
		typedef T type;
	};

	template <typename... Components>
	void for_each(typename identity<std::function<void(entity entity, Components&...)>>::type f)
	{
		return entities_with_components<Components...>().for_each(f);
	}

	/**
	 * Find Entities that have all of the specified Components and assign them
	 * to the given parameters.
	 *
	 * @code
	 * chandle<Position> position;
	 * chandle<Direction> direction;
	 * for (entity entity : entity_manager.entities_with_components(position,
	 * direction)) {
	 *   // Use position and component here.
	 * }
	 * @endcode
	 */
	template <typename... Components>
	unpacking_view<Components...> entities_with_components(chandle<Components>&... components)
	{
		auto mask = component_mask<Components...>();
		return unpacking_view<Components...>(this, mask, components...);
	}

	/**
	 * Iterate over all *valid* entities (ie. not in the free list). Not fast
	 *
	 * @code
	 * for (entity entity : entity_manager.all_entities()) {}
	 *
	 * @return An iterator view over all valid entities.
	 */
	base_view<true> all_entities()
	{
		return base_view<true>(this);
	}

	template <typename C>
	void unpack(entity::id_t id, chandle<C>& a)
	{
		assert_valid(id);
		a = get_component<C>(id);
	}

	/**
	 * Unpack components directly into pointers.
	 *
	 * Components missing from the entity will be set to nullptr.
	 *
	 * Useful for fast bulk iterations.
	 *
	 * chandle<Position> p;
	 * chandle<Direction> d;
	 * unpack<Position, Direction>(e, p, d);
	 */
	template <typename A, typename... Args>
	void unpack(entity::id_t id, chandle<A>& a, chandle<Args>&... args)
	{
		assert_valid(id);
		a = get_component<A>(id);
		unpack<Args...>(id, args...);
	}

	/**
	 * Destroy all entities and reset the entity_component_system.
	 */
	void dispose();

	void set_entity_name(entity::id_t id, std::string name);
	const std::string& get_entity_name(entity::id_t id);

private:
	friend class entity;

	inline void assert_valid(entity::id_t id) const
	{
		expects(id.index() < entity_component_mask_.size() && "entity::Id ID outside entity vector range");
		expects(entity_version_[id.index()] == id.version() &&
				"Attempt to access entity via a stale entity::Id");
	}

	component_mask_t component_mask(entity::id_t id)
	{
		assert_valid(id);
		return entity_component_mask_.at(id.index());
	}

	component_mask_t component_mask(entity::id_t id) const
	{
		assert_valid(id);
		return entity_component_mask_.at(id.index());
	}
	template <typename C>
	component_mask_t component_mask()
	{
		component_mask_t mask;
		mask.set(rtti::type_index_sequential_t::id<component, C>());
		return mask;
	}

	template <typename C1, typename C2, typename... Components>
	component_mask_t component_mask()
	{
		return component_mask<C1>() | component_mask<C2, Components...>();
	}

	template <typename C>
	component_mask_t component_mask(const chandle<C>&)
	{
		return component_mask<C>();
	}

	template <typename C1, typename... Components>
	component_mask_t component_mask(const chandle<C1>&, const chandle<Components>&...)
	{
		return component_mask<C1, Components...>();
	}

	inline void accomodate_entity(std::uint32_t index)
	{
		if(entity_component_mask_.size() <= index)
		{
			entity_component_mask_.resize(index + 1);
			entity_version_.resize(index + 1);
			for(auto& pool : component_pools_)
			{
				if(pool)
					pool->expand(index + 1);
			}
		}
	}

	template <typename C>
	component_storage& accomodate_component()
	{
		auto family = rtti::type_index_sequential_t::id<component, C>();
		return accomodate_component(family);
	}

	component_storage& accomodate_component(rtti::type_index_sequential_t::index_t family)
	{
		if(component_pools_.size() <= family)
		{
			component_pools_.resize(family + 1);
		}
		if(!component_pools_[family])
		{

			component_pools_[family].reset(new component_storage);
			component_pools_[family]->expand(index_counter_);
		}

		return *component_pools_[family].get();
	}

	std::uint32_t index_counter_ = 0;

	// Each element in component_pools_ corresponds to a Pool for a component.
	// The index into the vector is the component::family().
	std::vector<std::unique_ptr<component_storage>> component_pools_;
	// Bitmask of components associated with each entity. Index into the vector is
	// the entity::Id.
	std::vector<component_mask_t> entity_component_mask_;
	// Vector of entity version numbers. Incremented each time an entity is
	// destroyed
	std::vector<std::uint32_t> entity_version_;
	// List of available entity slots.
	std::vector<std::uint32_t> free_list_;

	std::unordered_map<std::uint64_t, std::string> entity_names_;
};

template <typename C, typename... Args>
chandle<C> entity::assign(Args&&... args)
{
	expects(valid());
	return manager_->assign<C>(id_, std::forward<Args>(args)...);
}

inline chandle<component> entity::assign(std::shared_ptr<component> component)
{
	expects(valid());
	return manager_->assign(id_, component);
}

template <typename C>
void entity::remove()
{
	expects(valid() && has_component<C>());
	manager_->remove<C>(id_);
}

inline void entity::remove(std::shared_ptr<component> component)
{
	expects(valid() && has_component(component));
	manager_->remove(id_, component);
}

template <typename C>
chandle<C> entity::get_component() const
{
	expects(valid());
	return manager_->get_component<C>(id_);
}

template <typename... Components>
std::tuple<chandle<Components>...> entity::components() const
{
	expects(valid());
	return manager_->components<Components...>(id_);
}

inline std::vector<chandle<component>> entity::all_components() const
{
	expects(valid());
	return manager_->all_components(id_);
}

inline std::vector<std::shared_ptr<component>> entity::all_components_shared() const
{
	expects(valid());
	return manager_->all_components_shared(id_);
}

template <typename C>
bool entity::has_component() const
{
	if(!valid())
		return false;
	return manager_->has_component<C>(id_);
}

inline bool entity::has_component(std::shared_ptr<component> component) const
{
	if(!valid())
		return false;
	return manager_->has_component(id_, component);
}

template <typename A, typename... Args>
void entity::unpack(chandle<A>& a, chandle<Args>&... args)
{
	expects(valid());
	manager_->unpack(id_, a, args...);
}

inline bool entity::valid() const
{
	return manager_ && manager_->valid(id_);
}

inline std::ostream& operator<<(std::ostream& out, const entity::id_t& id)
{
	out << id.index();
	return out;
}

inline std::ostream& operator<<(std::ostream& out, const entity& entity)
{
	out << "entity (" << entity.id() << ")";
	return out;
}

inline std::string entity::to_string() const
{
	auto name = get_name();
	if(name == "")
	{
		std::ostringstream str;
		str << *this;
		name = str.str();
	}
	return name;
}
}

namespace std
{
template <>
struct hash<runtime::entity>
{
	std::size_t operator()(const runtime::entity& entity) const
	{
		return static_cast<std::size_t>(entity.id().index() ^ entity.id().version());
	}
};
}
