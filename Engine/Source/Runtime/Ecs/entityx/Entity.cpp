/*
 * Copyright (C) 2012 Alec Thomas <alec@swapoff.org>
 * All rights reserved.
 *
 * This software is licensed as described in the file COPYING, which
 * you should have received as part of this distribution.
 *
 * Author: Alec Thomas <alec@swapoff.org>
 */

#include <algorithm>
#include "Entity.h"
#include "Component.h"
namespace entityx
{
	const Entity::Id Entity::INVALID;

	void Entity::setName(std::string name)
	{
		assert(valid());
		manager_->nameEntity(id_, name);
	}


	const std::string& Entity::getName() const
	{
		assert(valid());
		return manager_->getEntityName(id_);
	}

	void Entity::invalidate()
	{
		id_ = INVALID;
		manager_ = nullptr;
	}


	ComponentHandle<Component> Entity::assign_from_copy(std::shared_ptr<Component> component)
	{
		assert(valid());
		return manager_->assign(id_, component->clone());
	}

	void Entity::destroy()
	{
		assert(valid());
		manager_->destroy(id_);
		invalidate();
	}

	std::bitset<entityx::MAX_COMPONENTS> Entity::component_mask() const
	{
		return manager_->component_mask(id_);
	}

	EntityManager::EntityManager(EventManager &event_manager) : event_manager_(event_manager)
	{
	}

	EntityManager::~EntityManager()
	{
		reset();
	}

	void EntityManager::nameEntity(Entity::Id id, std::string name)
	{
		mEntityNames[id.id()] = name;
	}

	const std::string& EntityManager::getEntityName(Entity::Id id)
	{
		return mEntityNames[id.id()];
	}

	void EntityManager::reset()
	{
		for (Entity entity : entities_for_debugging()) entity.destroy();
		for (ComponentStorage *pool : component_pools_)
		{
			if (pool)
				delete pool;
		}
		component_pools_.clear();
		entity_component_mask_.clear();
		entity_version_.clear();
		free_list_.clear();
		index_counter_ = 0;
	}

	void EntityManager::remove(Entity::Id id, std::shared_ptr<Component> component)
	{
		remove(id, component->getId_v());
	}

	void EntityManager::remove(Entity::Id id, const ComponentId family)
	{
		assert_valid(id);
		const std::uint32_t index = id.index();

		// Find the pool for this component family.
		ComponentStorage *pool = component_pools_[family];
		ComponentHandle<Component> component(pool->get(id.index()));
		event_manager_.emit<ComponentRemovedEvent<Component>>(get(id), component);

		// Remove component bit.
		entity_component_mask_[id.index()].reset(family);

		// Call destructor.
		pool->destroy(index);
	}

	bool EntityManager::has_component(Entity::Id id, std::shared_ptr<Component> component) const
	{
		return has_component(id, component->getId_v());
	}

	bool EntityManager::has_component(Entity::Id id, ComponentId family) const
	{
		assert_valid(id);
		// We don't bother checking the component mask, as we return a nullptr anyway.
		if (family >= component_pools_.size())
			return false;
		ComponentStorage *pool = component_pools_[family];
		if (!pool || !entity_component_mask_[id.index()][family])
			return false;
		return true;
	}

	std::vector<ComponentHandle<Component>> EntityManager::all_components(Entity::Id id) const
	{
		std::vector<ComponentHandle<Component>> components;
		auto mask = component_mask(id);
		for (size_t i = 0; i < component_pools_.size(); i++)
		{
			if (mask.test(i))
			{
				auto pool = component_pools_[i];
				if (pool)
					components.push_back(ComponentHandle<Component>(pool->get(id.index())));
			}

		}

		return components;
	}

	ComponentHandle<Component> EntityManager::assign(Entity::Id id, std::shared_ptr<Component> component)
	{
		assert_valid(id);
		const auto family = component->getId_v();
		//assert(!entity_component_mask_[id.index()].test(family));

		// Placement new into the component pool.
		ComponentStorage *pool = accomodate_component(family);

		auto ptr = pool->set(id.index(), component);
		// Set the bit for this component.
		entity_component_mask_[id.index()].set(family);

		// Create and return handle.
		component->mEntity = get(id);
		component->onEntitySet();
		ComponentHandle<Component> handle(ptr);
		event_manager_.emit<ComponentAddedEvent<Component>>(get(id), handle);
		return handle;
	}

	void EntityManager::destroy(Entity::Id id)
	{
		mEntityNames[id.id()] = "";
		assert_valid(id);
		std::uint32_t index = id.index();
		auto mask = entity_component_mask_[index];
		for (size_t i = 0; i < component_pools_.size(); i++)
		{
			if (mask.test(i))
			{
				auto pool = component_pools_[i];
				if (pool)
				{
					auto handle = pool->get(index);
					handle->mEntity.remove(handle);
				}

			}
		}
		event_manager_.emit<EntityDestroyedEvent>(get(id));
		entity_component_mask_[index].reset();
		entity_version_[index]++;
		free_list_.push_back(index);

	}

	entityx::Entity EntityManager::create_from_copy(Entity original)
	{
		assert(original.valid());
		auto clone = create();
		auto mask = original.component_mask();
		for (size_t i = 0; i < component_pools_.size(); ++i)
		{
			if (mask.test(i))
			{
				auto component = component_pools_[i]->get(original.id().index());

				clone.assign(component->clone());
			}
		}
		clone.setName(original.getName());
		return clone;
	}

	EntityCreatedEvent::~EntityCreatedEvent() {}
	EntityDestroyedEvent::~EntityDestroyedEvent() {}


}  // namespace entityx
