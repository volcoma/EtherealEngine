/*
 * Copyright (C) 2012 Alec Thomas <alec@swapoff.org>
 * All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * Author: Alec Thomas <alec@swapoff.org>
 */

#pragma once


#include <cstdint>
#include <tuple>
#include <new>
#include <cstdlib>
#include <algorithm>
#include <bitset>
#include <cassert>
#include <iostream>
#include <iterator>
#include <list>
#include <memory>
#include <set>
#include <string>
#include <sstream>
#include <utility>
#include <vector>
#include <type_traits>
#include <functional>

#include "help/Storage.h"
#include "config.h"
#include "Event.h"
#include "help/NonCopyable.h"

namespace entityx
{

	class EntityManager;

	template<typename C>
	using ComponentHandle = std::weak_ptr<C>;

	/** A convenience handle around an Entity::Id.
	 *
	 * If an entity is destroyed, any copies will be invalidated. Use valid() to
	 * check for validity before using.
	 *
	 * Create entities with `EntityManager`:
	 *
	 *     Entity entity = entity_manager->create();
	 */
	class Entity
	{
	public:
		struct Id
		{
			Id() : id_(0) {}
			explicit Id(std::uint64_t id) : id_(id) {}
			Id(std::uint32_t index, std::uint32_t version) : id_(std::uint64_t(index) | std::uint64_t(version) << 32UL) {}

			std::uint64_t id() const { return id_; }

			bool operator == (const Id &other) const { return id_ == other.id_; }
			bool operator != (const Id &other) const { return id_ != other.id_; }
			bool operator < (const Id &other) const { return id_ < other.id_; }

			std::uint32_t index() const { return id_ & 0xffffffffUL; }
			std::uint32_t version() const { return id_ >> 32; }

		private:
			friend class EntityManager;

			std::uint64_t id_;
		};


		/**
		 * Id of an invalid Entity.
		 */
		static const Id INVALID;

		Entity() = default;
		Entity(EntityManager *manager, Entity::Id id) : manager_(manager), id_(id) {}
		Entity(const Entity &other) = default;
		Entity &operator = (const Entity &other) = default;

		/**
		 * Check if Entity handle is invalid.
		 */
		operator bool() const
		{
			return valid();
		}

		bool operator == (const Entity &other) const
		{
			return other.manager_ == manager_ && other.id_ == id_;
		}

		bool operator != (const Entity &other) const
		{
			return !(other == *this);
		}

		bool operator < (const Entity &other) const
		{
			return other.id_ < id_;
		}
		void setName(std::string name);
		const std::string& getName() const;
		/**
		 * Is this Entity handle valid?
		 *
		 * In older versions of EntityX, there were no guarantees around entity
		 * validity if a previously allocated entity slot was reassigned. That is no
		 * longer the case: if a slot is reassigned, old Entity::Id's will be
		 * invalid.
		 */
		bool valid() const;

		/**
		 * Invalidate Entity handle, disassociating it from an EntityManager and invalidating its ID.
		 *
		 * Note that this does *not* affect the underlying entity and its
		 * components. Use destroy() to destroy the associated Entity and components.
		 */
		void invalidate();

		std::string to_string() const;

		Id id() const { return id_; }

		template <typename C, typename ... Args>
		ComponentHandle<C> assign(Args && ... args);

		ComponentHandle<Component> assign(std::shared_ptr<Component> component);

		template <typename C>
		ComponentHandle<C> assign_from_copy(const ComponentHandle<C> &component);

		ComponentHandle<Component> assign_from_copy(std::shared_ptr<Component> component);

		template <typename C>
		void remove();

		void remove(std::shared_ptr<Component> component);

		template <typename C, typename = typename std::enable_if<!std::is_const<C>::value>::type>
		ComponentHandle<C> component();

		template <typename C, typename = typename std::enable_if<std::is_const<C>::value>::type>
		const ComponentHandle<C> component() const;

		template <typename ... Components>
		std::tuple<ComponentHandle<Components>...> components();

		template <typename ... Components>
		std::tuple<ComponentHandle<const Components>...> components() const;

		std::vector<ComponentHandle<Component>> all_components() const;

		template <typename C>
		bool has_component() const;

		bool has_component(std::shared_ptr<Component> component) const;

		template <typename A, typename ... Args>
		void unpack(ComponentHandle<A> &a, ComponentHandle<Args> & ... args);

		/**
		 * Destroy and invalidate this Entity.
		 */
		void destroy();

		std::bitset<entityx::MAX_COMPONENTS> component_mask() const;

	private:
		EntityManager *manager_ = nullptr;
		Entity::Id id_ = INVALID;
	};




	/**
	 * Emitted when an entity is added to the system.
	 */
	struct EntityCreatedEvent : public Event<EntityCreatedEvent>
	{
		explicit EntityCreatedEvent(Entity entity) : entity(entity) {}
		virtual ~EntityCreatedEvent();

		Entity entity;
	};


	/**
	 * Called just prior to an entity being destroyed.
	 */
	struct EntityDestroyedEvent : public Event<EntityDestroyedEvent>
	{
		explicit EntityDestroyedEvent(Entity entity) : entity(entity) {}
		virtual ~EntityDestroyedEvent();

		Entity entity;
	};


	/**
	 * Emitted when any component is added to an entity.
	 */
	template <typename C>
	struct ComponentAddedEvent : public Event<ComponentAddedEvent<C>>
	{
		ComponentAddedEvent(Entity entity, ComponentHandle<C> component) :
			entity(entity), component(component) {}

		Entity entity;
		ComponentHandle<C> component;
	};

	/**
	 * Emitted when any component is removed from an entity.
	 */
	template <typename C>
	struct ComponentRemovedEvent : public Event<ComponentRemovedEvent<C>>
	{
		ComponentRemovedEvent(Entity entity, ComponentHandle<C> component) :
			entity(entity), component(component) {}

		Entity entity;
		ComponentHandle<C> component;
	};

	/**
	 * Manages Entity::Id creation and component assignment.
	 */
	class EntityManager : entityx::help::NonCopyable
	{
	public:
		typedef std::bitset<entityx::MAX_COMPONENTS> ComponentMask;

		explicit EntityManager(EventManager &event_manager);
		virtual ~EntityManager();

		/// An iterator over a view of the entities in an EntityManager.
		/// If All is true it will iterate over all valid entities and will ignore the entity mask.
		template <class Delegate, bool All = false>
		class ViewIterator : public std::iterator<std::input_iterator_tag, Entity::Id>
		{
		public:
			Delegate &operator ++()
			{
				++i_;
				next();
				return *static_cast<Delegate*>(this);
			}
			bool operator == (const Delegate& rhs) const { return i_ == rhs.i_; }
			bool operator != (const Delegate& rhs) const { return i_ != rhs.i_; }
			Entity operator * () { return Entity(manager_, manager_->create_id(i_)); }
			const Entity operator * () const { return Entity(manager_, manager_->create_id(i_)); }

		protected:
			ViewIterator(EntityManager *manager, std::uint32_t index)
				: manager_(manager), i_(index), capacity_(manager_->capacity()), free_cursor_(~0UL)
			{
				if (All)
				{
					std::sort(manager_->free_list_.begin(), manager_->free_list_.end());
					free_cursor_ = 0;
				}
			}
			ViewIterator(EntityManager *manager, const ComponentMask mask, std::uint32_t index)
				: manager_(manager), mask_(mask), i_(index), capacity_(manager_->capacity()), free_cursor_(~0UL)
			{
				if (All)
				{
					std::sort(manager_->free_list_.begin(), manager_->free_list_.end());
					free_cursor_ = 0;
				}
			}

			void next()
			{
				while (i_ < capacity_ && !predicate())
				{
					++i_;
				}

				if (i_ < capacity_)
				{
					Entity entity = manager_->get(manager_->create_id(i_));
					static_cast<Delegate*>(this)->next_entity(entity);
				}
			}

			inline bool predicate()
			{
				return (All && valid_entity()) || (manager_->entity_component_mask_[i_] & mask_) == mask_;
			}

			inline bool valid_entity()
			{
				const std::vector<std::uint32_t> &free_list = manager_->free_list_;
				if (free_cursor_ < free_list.size() && free_list[free_cursor_] == i_)
				{
					++free_cursor_;
					return false;
				}
				return true;
			}

			EntityManager *manager_;
			ComponentMask mask_;
			std::uint32_t i_;
			size_t capacity_;
			size_t free_cursor_;
		};

		template <bool All>
		class BaseView
		{
		public:
			class Iterator : public ViewIterator<Iterator, All>
			{
			public:
				Iterator(EntityManager *manager,
					const ComponentMask mask,
					std::uint32_t index) : ViewIterator<Iterator, All>(manager, mask, index)
				{
					ViewIterator<Iterator, All>::next();
				}

				void next_entity(Entity &entity) {}
			};

			Iterator begin() { return Iterator(manager_, mask_, 0); }
			Iterator end() { return Iterator(manager_, mask_, std::uint32_t(manager_->capacity())); }
			const Iterator begin() const { return Iterator(manager_, mask_, 0); }
			const Iterator end() const { return Iterator(manager_, mask_, manager_->capacity()); }

		private:
			friend class EntityManager;

			explicit BaseView(EntityManager *manager) : manager_(manager) { mask_.set(); }
			BaseView(EntityManager *manager, ComponentMask mask) :
				manager_(manager), mask_(mask) {}

			EntityManager *manager_;
			ComponentMask mask_;
		};

		template <bool All, typename ... Components>
		class TypedView : public BaseView<All>
		{
		public:
			template <typename T> struct identity { typedef T type; };

			void each(typename identity<std::function<void(Entity entity, Components&...)>>::type f)
			{
				for (auto it : *this)
					f(it, *(it.template component<Components>().lock().get())...);
			}

		private:
			friend class EntityManager;

			explicit TypedView(EntityManager *manager) : BaseView<All>(manager) {}
			TypedView(EntityManager *manager, ComponentMask mask) : BaseView<All>(manager, mask) {}
		};

		template <typename ... Components> using View = TypedView<false, Components...>;
		typedef BaseView<true> DebugView;

		template <typename ... Components>
		class UnpackingView
		{
		public:
			struct Unpacker
			{
				explicit Unpacker(ComponentHandle<Components> & ... handles) :
					handles(std::tuple<ComponentHandle<Components> & ...>(handles...)) {}

				void unpack(entityx::Entity &entity) const
				{
					unpack_<0, Components...>(entity);
				}


			private:
				template <int N, typename C>
				void unpack_(entityx::Entity &entity) const
				{
					std::get<N>(handles) = entity.component<C>();
				}

				template <int N, typename C0, typename C1, typename ... Cn>
				void unpack_(entityx::Entity &entity) const
				{
					std::get<N>(handles) = entity.component<C0>();
					unpack_<N + 1, C1, Cn...>(entity);
				}

				std::tuple<ComponentHandle<Components> & ...> handles;
			};


			class Iterator : public ViewIterator<Iterator>
			{
			public:
				Iterator(EntityManager *manager,
					const ComponentMask mask,
					std::uint32_t index,
					const Unpacker &unpacker) : ViewIterator<Iterator>(manager, mask, index), unpacker_(unpacker)
				{
					ViewIterator<Iterator>::next();
				}

				void next_entity(Entity &entity)
				{
					unpacker_.unpack(entity);
				}

			private:
				const Unpacker &unpacker_;
			};


			Iterator begin() { return Iterator(manager_, mask_, 0, unpacker_); }
			Iterator end() { return Iterator(manager_, mask_, static_cast<std::uint32_t>(manager_->capacity()), unpacker_); }
			const Iterator begin() const { return Iterator(manager_, mask_, 0, unpacker_); }
			const Iterator end() const { return Iterator(manager_, mask_, static_cast<std::uint32_t>(manager_->capacity()), unpacker_); }


		private:
			friend class EntityManager;

			UnpackingView(EntityManager *manager, ComponentMask mask, ComponentHandle<Components> & ... handles) :
				manager_(manager), mask_(mask), unpacker_(handles...) {}

			EntityManager *manager_;
			ComponentMask mask_;
			Unpacker unpacker_;
		};

		/**
		 * Number of managed entities.
		 */
		size_t size() const { return entity_component_mask_.size() - free_list_.size(); }

		/**
		 * Current entity capacity.
		 */
		size_t capacity() const { return entity_component_mask_.size(); }

		/**
		 * Return true if the given entity ID is still valid.
		 */
		bool valid(Entity::Id id) const
		{
			return id.index() < entity_version_.size() && entity_version_[id.index()] == id.version();
		}

		/**
		* Return true if the given entity ID is still valid.
		*/
		bool valid_index(std::uint32_t index) const
		{
			return index < entity_version_.size();
		}
		/**
		 * Create a new Entity::Id.
		 *
		 * Emits EntityCreatedEvent.
		 */
		Entity create()
		{
			std::uint32_t index, version;
			if (free_list_.empty())
			{
				index = index_counter_++;
				accomodate_entity(index);
				version = entity_version_[index] = 1;
			}
			else
			{
				index = free_list_.back();
				free_list_.pop_back();
				version = entity_version_[index];
			}
			Entity entity(this, Entity::Id(index, version));
			event_manager_.emit<EntityCreatedEvent>(entity);
			return entity;
		}

		/**
		 * Create a new Entity by copying another. Copy-constructs each component.
		 *
		 * Emits EntityCreatedEvent.
		 */
		Entity create_from_copy(Entity original);


		/**
		 * Destroy an existing Entity::Id and its associated Components.
		 *
		 * Emits EntityDestroyedEvent.
		 */
		void destroy(Entity::Id id);

		Entity get(Entity::Id id)
		{
			assert_valid(id);
			return Entity(this, id);
		}

		/**
		 * Create an Entity::Id for a slot.
		 *
		 * NOTE: Does *not* check for validity, but the Entity::Id constructor will
		 * fail if the ID is invalid.
		 */
		Entity::Id create_id(std::uint32_t index) const
		{
			return Entity::Id(index, entity_version_[index]);
		}

		/**
		 * Assign a Component to an Entity::Id, passing through Component constructor arguments.
		 *
		 *     Position &position = em.assign<Position>(e, x, y);
		 *
		 * @returns Smart pointer to newly created component.
		 */
		template <typename C, typename ... Args>
		ComponentHandle<C> assign(Entity::Id id, Args && ... args)
		{
			return std::static_pointer_cast<C>(assign(id, std::make_shared<C>(std::forward<Args>(args) ...)).lock());
		}

		ComponentHandle<Component> assign(Entity::Id id, std::shared_ptr<Component> component);

		/**
		 * Remove a Component from an Entity::Id
		 *
		 * Emits a ComponentRemovedEvent<C> event.
		 */
		template <typename C>
		void remove(Entity::Id id)
		{
			remove(id, C::getId());
		}
		void remove(Entity::Id id, std::shared_ptr<Component> component);
		void remove(Entity::Id id, const ComponentId family);

		/**
		 * Check if an Entity has a component.
		 */
		template <typename C>
		bool has_component(Entity::Id id) const
		{
			return has_component(id, C::getId());
		}


		bool has_component(Entity::Id id, std::shared_ptr<Component> component) const;

		bool has_component(Entity::Id id, ComponentId family) const;
		/**
		 * Retrieve a Component assigned to an Entity::Id.
		 *
		 * @returns Pointer to an instance of C, or nullptr if the Entity::Id does not have that Component.
		 */
		template <typename C, typename = typename std::enable_if<!std::is_const<C>::value>::type>
		ComponentHandle<C> component(Entity::Id id)
		{
			assert_valid(id);
			auto family = C::getId();
			// We don't bother checking the component mask, as we return a nullptr anyway.
			if (family >= component_pools_.size())
				return ComponentHandle<C>();
			ComponentStorage *pool = component_pools_[family];
			if (!pool || !entity_component_mask_[id.index()][family])
				return ComponentHandle<C>();
			return ComponentHandle<C>(pool->get<C>(id.index()));
		}

		/**
		 * Retrieve a Component assigned to an Entity::Id.
		 *
		 * @returns Component instance, or nullptr if the Entity::Id does not have that Component.
		 */
		template <typename C, typename = typename std::enable_if<std::is_const<C>::value>::type>
		const ComponentHandle<C> component(Entity::Id id) const
		{
			assert_valid(id);
			auto family = C::getId();
			// We don't bother checking the component mask, as we return a nullptr anyway.
			if (family >= component_pools_.size())
				return ComponentHandle<C>();
			ComponentStorage *pool = component_pools_[family];
			if (!pool || !entity_component_mask_[id.index()][family])
				return ComponentHandle<C>();
			return ComponentHandle<C>(pool->get<C>(id.index()));
		}

		template <typename ... Components>
		std::tuple<ComponentHandle<Components>...> components(Entity::Id id)
		{
			return std::make_tuple(component<Components>(id)...);
		}

		template <typename ... Components>
		std::tuple<ComponentHandle<const Components>...> components(Entity::Id id) const
		{
			return std::make_tuple(component<const Components>(id)...);
		}

		std::vector<ComponentHandle<Component>> all_components(Entity::Id id) const;

		/**
		 * Find Entities that have all of the specified Components.
		 *
		 * @code
		 * for (Entity entity : entity_manager.entities_with_components<Position, Direction>()) {
		 *   ComponentHandle<Position> position = entity.component<Position>();
		 *   ComponentHandle<Direction> direction = entity.component<Direction>();
		 *
		 *   ...
		 * }
		 * @endcode
		 */
		template <typename ... Components>
		View<Components...> entities_with_components()
		{
			auto mask = component_mask<Components ...>();
			return View<Components...>(this, mask);
		}

		template <typename T> struct identity { typedef T type; };

		template <typename ... Components>
		void each(typename identity<std::function<void(Entity entity, Components&...)>>::type f)
		{
			return entities_with_components<Components...>().each(f);
		}

		/**
		 * Find Entities that have all of the specified Components and assign them
		 * to the given parameters.
		 *
		 * @code
		 * ComponentHandle<Position> position;
		 * ComponentHandle<Direction> direction;
		 * for (Entity entity : entity_manager.entities_with_components(position, direction)) {
		 *   // Use position and component here.
		 * }
		 * @endcode
		 */
		template <typename ... Components>
		UnpackingView<Components...> entities_with_components(ComponentHandle<Components> & ... components)
		{
			auto mask = component_mask<Components...>();
			return UnpackingView<Components...>(this, mask, components...);
		}

		/**
		 * Iterate over all *valid* entities (ie. not in the free list). Not fast,
		 * so should only be used for debugging.
		 *
		 * @code
		 * for (Entity entity : entity_manager.entities_for_debugging()) {}
		 *
		 * @return An iterator view over all valid entities.
		 */
		DebugView entities_for_debugging()
		{
			return DebugView(this);
		}

		template <typename C>
		void unpack(Entity::Id id, ComponentHandle<C> &a)
		{
			assert_valid(id);
			a = component<C>(id);
		}

		/**
		 * Unpack components directly into pointers.
		 *
		 * Components missing from the entity will be set to nullptr.
		 *
		 * Useful for fast bulk iterations.
		 *
		 * ComponentHandle<Position> p;
		 * ComponentHandle<Direction> d;
		 * unpack<Position, Direction>(e, p, d);
		 */
		template <typename A, typename ... Args>
		void unpack(Entity::Id id, ComponentHandle<A> &a, ComponentHandle<Args> & ... args)
		{
			assert_valid(id);
			a = component<A>(id);
			unpack<Args ...>(id, args ...);
		}

		/**
		 * Destroy all entities and reset the EntityManager.
		 */
		void reset();

		void nameEntity(Entity::Id id, std::string name);
		const std::string& getEntityName(Entity::Id id);
	private:
		friend class Entity;

		inline void assert_valid(Entity::Id id) const
		{
			assert(id.index() < entity_component_mask_.size() && "Entity::Id ID outside entity vector range");
			assert(entity_version_[id.index()] == id.version() && "Attempt to access Entity via a stale Entity::Id");
		}

		ComponentMask component_mask(Entity::Id id)
		{
			assert_valid(id);
			return entity_component_mask_.at(id.index());
		}

		ComponentMask component_mask(Entity::Id id) const
		{
			assert_valid(id);
			return entity_component_mask_.at(id.index());
		}
		template <typename C>
		ComponentMask component_mask()
		{
			ComponentMask mask;
			mask.set(C::getId());
			return mask;
		}

		template <typename C1, typename C2, typename ... Components>
		ComponentMask component_mask()
		{
			return component_mask<C1>() | component_mask<C2, Components ...>();
		}

		template <typename C>
		ComponentMask component_mask(const ComponentHandle<C> &c)
		{
			return component_mask<C>();
		}

		template <typename C1, typename ... Components>
		ComponentMask component_mask(const ComponentHandle<C1> &c1, const ComponentHandle<Components> &... args)
		{
			return component_mask<C1, Components ...>();
		}

		inline void accomodate_entity(std::uint32_t index)
		{
			if (entity_component_mask_.size() <= index)
			{
				entity_component_mask_.resize(index + 1);
				entity_version_.resize(index + 1);
				for (ComponentStorage *pool : component_pools_)
					if (pool) pool->expand(index + 1);
			}
		}

		template <typename C>
		ComponentStorage *accomodate_component()
		{
			auto family = C::getId();
			return accomodate_component(family);
		}

		ComponentStorage *accomodate_component(ComponentId family)
		{
			if (component_pools_.size() <= family)
			{
				component_pools_.resize(family + 1, nullptr);
			}
			if (!component_pools_[family])
			{
				ComponentStorage *pool = new ComponentStorage();
				pool->expand(index_counter_);
				component_pools_[family] = pool;
			}

			return component_pools_[family];
		}

		std::uint32_t index_counter_ = 0;

		EventManager &event_manager_;
		// Each element in component_pools_ corresponds to a Pool for a Component.
		// The index into the vector is the Component::family().
		std::vector<ComponentStorage*> component_pools_;
		// Bitmask of components associated with each entity. Index into the vector is the Entity::Id.
		std::vector<ComponentMask> entity_component_mask_;
		// Vector of entity version numbers. Incremented each time an entity is destroyed
		std::vector<std::uint32_t> entity_version_;
		// List of available entity slots.
		std::vector<std::uint32_t> free_list_;

		std::unordered_map<std::uint64_t, std::string>	mEntityNames;
	};


	template <typename C, typename ... Args>
	ComponentHandle<C> Entity::assign(Args && ... args)
	{
		assert(valid());
		return manager_->assign<C>(id_, std::forward<Args>(args) ...);
	}

	inline ComponentHandle<Component> Entity::assign(std::shared_ptr<Component> component)
	{
		assert(valid());
		return manager_->assign(id_, component);
	}

	template <typename C>
	ComponentHandle<C> Entity::assign_from_copy(const ComponentHandle<C> &component)
	{
		assert(valid());
		return manager_->assign(id_, component.get()->clone());
	}

	template <typename C>
	void Entity::remove()
	{
		assert(valid() && has_component<C>());
		manager_->remove<C>(id_);
	}

	inline void Entity::remove(std::shared_ptr<Component> component)
	{
		assert(valid() && has_component(component));
		manager_->remove(id_, component);
	}

	template <typename C, typename>
	ComponentHandle<C> Entity::component()
	{
		assert(valid());
		return manager_->component<C>(id_);
	}

	template <typename C, typename>
	const ComponentHandle<C> Entity::component() const
	{
		assert(valid());
		return const_cast<const EntityManager*>(manager_)->component<const C>(id_);
	}

	template <typename ... Components>
	std::tuple<ComponentHandle<Components>...> Entity::components() {
		assert(valid());
		return manager_->components<Components...>(id_);
	}

	inline std::vector<ComponentHandle<Component>> Entity::all_components() const
	{
		assert(valid());
		return manager_->all_components(id_);
	}

	template <typename ... Components>
	std::tuple<ComponentHandle<const Components>...> Entity::components() const
	{
		assert(valid());
		return const_cast<const EntityManager*>(manager_)->components<const Components...>(id_);
	}


	template <typename C>
	bool Entity::has_component() const
	{
		assert(valid());
		return manager_->has_component<C>(id_);
	}

	inline bool Entity::has_component(std::shared_ptr<Component> component) const
	{
		assert(valid());
		return manager_->has_component(id_, component);
	}

	template <typename A, typename ... Args>
	void Entity::unpack(ComponentHandle<A> &a, ComponentHandle<Args> & ... args)
	{
		assert(valid());
		manager_->unpack(id_, a, args ...);
	}

	inline bool Entity::valid() const
	{
		return manager_ && manager_->valid(id_);
	}



	inline std::ostream &operator << (std::ostream &out, const Entity::Id &id)
	{
		out << "Entity::Id(" << id.index() << "." << id.version() << ")";
		return out;
	}


	inline std::ostream &operator << (std::ostream &out, const Entity &entity)
	{
		out << "Entity(" << entity.id() << ")";
		return out;
	}

	inline std::string Entity::to_string() const
	{
		auto name = getName();
		if (name == "")
		{
			std::ostringstream str;
			str << *this;
			name = str.str();
		}
		return name;
	}

}  // namespace entityx


namespace std
{
	template <> struct hash<entityx::Entity>
	{
		std::size_t operator () (const entityx::Entity &entity) const
		{
			return static_cast<std::size_t>(entity.id().index() ^ entity.id().version());
		}
	};
}

