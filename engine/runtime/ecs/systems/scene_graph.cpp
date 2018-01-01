#include "scene_graph.h"
#include "../../system/events.h"
#include "../components/transform_component.h"
#include "core/system/subsystem.h"

namespace runtime
{
void update_transform(entity e)
{
	if(e.valid())
	{
		auto transform_comp = e.get_component<transform_component>().lock();
		if(transform_comp)
		{
			transform_comp->resolve(true);

			auto& children = transform_comp->get_children();
			for(auto& child : children)
			{
				update_transform(child);
			}
		}
	}
}

void scene_graph::frame_update(std::chrono::duration<float> dt)
{
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	_roots.clear();
	auto all_entities = ecs.all_entities();
	for(const auto entity : all_entities)
	{
		auto transform_comp = entity.get_component<transform_component>().lock();
		if(transform_comp)
		{
			auto parent = transform_comp->get_parent();
			if(parent.valid() == false)
			{
				_roots.push_back(entity);
			}
		}
		else
		{
			_roots.push_back(entity);
		}
	}

	for(auto& entity : _roots)
	{
		update_transform(entity);
	}
}

scene_graph::scene_graph()
{
	runtime::on_frame_update.connect(this, &scene_graph::frame_update);

	transform_component::static_id();
}

scene_graph::~scene_graph()
{
	runtime::on_frame_update.disconnect(this, &scene_graph::frame_update);
}
}
