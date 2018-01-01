#include "ecs.h"

namespace runtime
{

namespace ecs
{
namespace detail
{
frame_getter_t& get_frame_getter()
{
	static frame_getter_t f;
	return f;
}
}

void set_frame_getter(frame_getter_t frame_getter)
{
	detail::get_frame_getter() = frame_getter;
}

uint64_t get_frame()
{
	const auto& f = detail::get_frame_getter();
	if(f)
	{
		return f();
	}
	return 0;
}
}

event<void(entity)> on_entity_created;
event<void(entity)> on_entity_destroyed;
event<void(entity, chandle<component>)> on_component_added;
event<void(entity, chandle<component>)> on_component_removed;

component_storage::component_storage(std::size_t size)
{
	expand(size);
}

void component_storage::expand(std::size_t n)
{
	data.resize(n);
}

void component_storage::reserve(std::size_t n)
{
	data.reserve(n);
}

std::shared_ptr<component> component_storage::get(std::size_t n) const
{
	expects(n < size());
	return data[n];
}

void component_storage::destroy(std::size_t n)
{
	expects(n < size());
	auto& element = data[n];
	element.reset();
}

std::weak_ptr<component> component_storage::set(unsigned int index, std::shared_ptr<component> component)
{
	data[index] = component;
	return component;
}

/////////////////////////////////////////////////////////////////////////////
const entity::id_t entity::INVALID;

void entity::set_name(std::string name)
{
	expects(valid());
	manager_->set_entity_name(id_, name);
}

const std::string& entity::get_name() const
{
	expects(valid());
	return manager_->get_entity_name(id_);
}

void entity::invalidate()
{
	id_ = INVALID;
	manager_ = nullptr;
}

void entity::destroy()
{
	expects(valid());
	manager_->destroy(id_);
	invalidate();
}

std::bitset<MAX_COMPONENTS> entity::component_mask() const
{
	return manager_->component_mask(id_);
}

entity_component_system::entity_component_system()
{
}

entity_component_system::~entity_component_system()
{
	dispose();
}

size_t entity_component_system::size() const
{
	return entity_component_mask_.size() - free_list_.size();
}

size_t entity_component_system::capacity() const
{
	return entity_component_mask_.size();
}

bool entity_component_system::valid(entity::id_t id) const
{
	return id.index() < entity_version_.size() && entity_version_[id.index()] == id.version();
}

bool entity_component_system::valid_index(uint32_t index) const
{
	return index < entity_version_.size();
}

entity entity_component_system::create()
{
	std::uint32_t index, version;
	if(free_list_.empty())
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
	entity entity(this, entity::id_t(index, version));
	on_entity_created(entity);
	return entity;
}

void entity_component_system::set_entity_name(entity::id_t id, std::string name)
{
	entity_names_[id.id()] = name;
}

const std::string& entity_component_system::get_entity_name(entity::id_t id)
{
	return entity_names_[id.id()];
}

void entity_component_system::dispose()
{
	for(entity entity : all_entities())
	{
		entity.destroy();
	}

	component_pools_.clear();
	entity_component_mask_.clear();
	entity_version_.clear();
	free_list_.clear();
	index_counter_ = 0;
}

void entity_component_system::remove(entity::id_t id, std::shared_ptr<component> component)
{
	remove(id, component->runtime_id());
}

void entity_component_system::remove(entity::id_t id, const rtti::type_index_sequential_t::index_t family)
{
	assert_valid(id);
	const std::uint32_t index = id.index();

	// Find the pool for this component family.
	auto& pool = component_pools_[family];
	chandle<component> handle(pool->get(id.index()));
	on_component_removed(get(id), handle);
	// Remove component bit.
	entity_component_mask_[id.index()].reset(family);

	// Call destructor.
	pool->destroy(index);
}

bool entity_component_system::has_component(entity::id_t id, std::shared_ptr<component> component) const
{
	return has_component(id, component->runtime_id());
}

bool entity_component_system::has_component(entity::id_t id,
											rtti::type_index_sequential_t::index_t family) const
{
	assert_valid(id);
	// We don't bother checking the component mask, as we return a nullptr anyway.
	if(family >= component_pools_.size())
		return false;
	auto& pool = component_pools_[family];
	if(!pool || !entity_component_mask_[id.index()][family])
		return false;
	return true;
}

std::vector<chandle<component>> entity_component_system::all_components(entity::id_t id) const
{
	std::vector<chandle<component>> components;
	auto mask = component_mask(id);
	for(size_t i = 0; i < component_pools_.size(); ++i)
	{
		if(mask.test(i))
		{
			auto& pool = component_pools_[i];
			if(pool)
				components.push_back(chandle<component>(pool->get(id.index())));
		}
	}
	return components;
}

std::vector<std::shared_ptr<component>> entity_component_system::all_components_shared(entity::id_t id) const
{
	std::vector<std::shared_ptr<component>> components;
	auto mask = component_mask(id);
	for(size_t i = 0; i < component_pools_.size(); ++i)
	{
		if(mask.test(i))
		{
			auto& pool = component_pools_[i];
			if(pool)
				components.push_back(pool->get(id.index()));
		}
	}
	return components;
}

chandle<component> entity_component_system::assign(entity::id_t id, std::shared_ptr<component> comp)
{
	assert_valid(id);
	const auto family = comp->runtime_id();
	// assert(!entity_component_mask_[id.index()].test(family));

	// Placement new into the component pool.
	auto& pool = accomodate_component(family);

	auto ptr = pool.set(id.index(), comp);
	// Set the bit for this component.
	entity_component_mask_[id.index()].set(family);

	// Create and return handle.
	comp->_entity = get(id);
	comp->on_entity_set();
	chandle<component> handle(ptr);
	on_component_added(get(id), handle);
	return handle;
}

void entity_component_system::destroy(entity::id_t id)
{
	entity_names_[id.id()] = "";
	assert_valid(id);
	std::uint32_t index = id.index();
	auto mask = entity_component_mask_[index];
	for(size_t i = 0; i < component_pools_.size(); ++i)
	{
		if(mask.test(i))
		{
			auto& pool = component_pools_[i];
			if(pool)
			{
				auto handle = pool->get(index);
				handle->_entity.remove(handle);
			}
		}
	}

	on_entity_destroyed(get(id));
	entity_component_mask_[index].reset();
	entity_version_[index]++;
	free_list_.push_back(index);
}

entity entity_component_system::get(entity::id_t id)
{
	assert_valid(id);
	return entity(this, id);
}

entity::id_t entity_component_system::create_id(uint32_t index) const
{
	return entity::id_t(index, entity_version_[index]);
}

component::~component()
{
}
}
