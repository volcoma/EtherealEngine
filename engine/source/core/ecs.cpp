#include "ecs.h"

namespace core
{
	event<void(Entity)> onEntityCreated;
	event<void(Entity)> onEntityDestroyed;
	event<void(Entity, CHandle<Component>)> onComponentAdded;
	event<void(Entity, CHandle<Component>)> onComponentRemoved;

	ComponentStorage::ComponentStorage(std::size_t size)
	{
		expand(size);
	}

	ComponentStorage::~ComponentStorage()
	{
	}

	void ComponentStorage::expand(std::size_t n)
	{
		data.resize(n);
	}

	void ComponentStorage::reserve(std::size_t n)
	{
		data.reserve(n);
	}

	std::shared_ptr<Component> ComponentStorage::get(std::size_t n)
	{
		Expects(n < size());
		return data[n];
	}

	const std::shared_ptr<Component> ComponentStorage::get(std::size_t n) const
	{
		Expects(n < size());
		return data[n];
	}

	void ComponentStorage::destroy(std::size_t n)
	{
		Expects(n < size());
		auto& element = data[n];
		element.reset();
	}

	std::weak_ptr<Component> ComponentStorage::set(unsigned int index, std::shared_ptr<Component> component)
	{
		data[index] = component;
		return component;
	}

	/////////////////////////////////////////////////////////////////////////////
	const Entity::Id Entity::INVALID;

	void Entity::set_name(std::string name)
	{
		Expects(valid());
		manager_->set_entity_name(id_, name);
	}

	const std::string& Entity::get_name() const
	{
		Expects(valid());
		return manager_->get_entity_name(id_);
	}

	void Entity::invalidate()
	{
		id_ = INVALID;
		manager_ = nullptr;
	}

	CHandle<Component> Entity::assign_from_copy(std::shared_ptr<Component> component)
	{
		Expects(valid());
		return manager_->assign(id_, component->clone());
	}

	void Entity::destroy()
	{
		Expects(valid());
		manager_->destroy(id_);
		invalidate();
	}

	std::bitset<MAX_COMPONENTS> Entity::component_mask() const
	{
		return manager_->component_mask(id_);
	}

	EntityComponentSystem::EntityComponentSystem()
	{
	}

	EntityComponentSystem::~EntityComponentSystem()
	{
		dispose();
	}

	void EntityComponentSystem::set_entity_name(Entity::Id id, std::string name)
	{
		entity_names_[id.id()] = name;
	}

	const std::string& EntityComponentSystem::get_entity_name(Entity::Id id)
	{
		return entity_names_[id.id()];
	}

	void EntityComponentSystem::dispose()
	{
		for (Entity entity : all_entities()) entity.destroy();
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

	void EntityComponentSystem::remove(Entity::Id id, std::shared_ptr<Component> component)
	{
		remove(id, component->runtime_id());
	}

	void EntityComponentSystem::remove(Entity::Id id, const TypeInfo::index_t family)
	{
		assert_valid(id);
		const std::uint32_t index = id.index();

		// Find the pool for this component family.
		ComponentStorage *pool = component_pools_[family];
		CHandle<Component> handle(pool->get(id.index()));
		onComponentRemoved(get(id), handle);
		// Remove component bit.
		entity_component_mask_[id.index()].reset(family);

		// Call destructor.
		pool->destroy(index);
	}

	bool EntityComponentSystem::has_component(Entity::Id id, std::shared_ptr<Component> component) const
	{
		return has_component(id, component->runtime_id());
	}

	bool EntityComponentSystem::has_component(Entity::Id id, TypeInfo::index_t family) const
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

	std::vector<CHandle<Component>> EntityComponentSystem::all_components(Entity::Id id) const
	{
		std::vector<CHandle<Component>> components;
		auto mask = component_mask(id);
		for (size_t i = 0; i < component_pools_.size(); ++i)
		{
			if (mask.test(i))
			{
				auto pool = component_pools_[i];
				if (pool)
					components.push_back(CHandle<Component>(pool->get(id.index())));
			}
		}
		return components;
	}

	std::vector<std::shared_ptr<Component>> EntityComponentSystem::all_components_shared(Entity::Id id) const
	{
		std::vector<std::shared_ptr<Component>> components;
		auto mask = component_mask(id);
		for (size_t i = 0; i < component_pools_.size(); ++i)
		{
			if (mask.test(i))
			{
				auto pool = component_pools_[i];
				if (pool)
					components.push_back(pool->get(id.index()));
			}
		}
		return components;
	}

	CHandle<Component> EntityComponentSystem::assign(Entity::Id id, std::shared_ptr<Component> component)
	{
		assert_valid(id);
		const auto family = component->runtime_id();
		//assert(!entity_component_mask_[id.index()].test(family));

		// Placement new into the component pool.
		ComponentStorage *pool = accomodate_component(family);

		auto ptr = pool->set(id.index(), component);
		// Set the bit for this component.
		entity_component_mask_[id.index()].set(family);

		// Create and return handle.
		component->mEntity = get(id);
		component->onEntitySet();
		CHandle<Component> handle(ptr);
		onComponentAdded(get(id), handle);
		return handle;
	}

	void EntityComponentSystem::destroy(Entity::Id id)
	{
		entity_names_[id.id()] = "";
		assert_valid(id);
		std::uint32_t index = id.index();
		auto mask = entity_component_mask_[index];
		for (size_t i = 0; i < component_pools_.size(); ++i)
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

		onEntityDestroyed(get(id));
		entity_component_mask_[index].reset();
		entity_version_[index]++;
		free_list_.push_back(index);

	}

	Entity EntityComponentSystem::create_from_copy(Entity original)
	{
		Expects(original.valid());
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
		clone.set_name(original.get_name());
		return clone;
	}

	// 	EntityCreatedEvent::~EntityCreatedEvent() {}
	// 	EntityDestroyedEvent::~EntityDestroyedEvent() }
}