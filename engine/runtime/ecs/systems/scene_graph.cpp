#include "scene_graph.h"
#include "../../system/events.h"
#include "../components/transform_component.h"
#include "core/system/subsystem.h"

namespace runtime
{

void scene_graph::frame_update(delta_t dt)
{
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	roots_.clear();
	auto all_entities = ecs.all_entities();
	for(const auto entity : all_entities)
	{
		auto transform_comp = entity.get_component<transform_component>().lock();
		if(transform_comp)
		{
			auto parent = transform_comp->get_parent();
			if(parent.valid() == false)
			{
				roots_.push_back(entity);
			}
		}
		else
		{
			roots_.push_back(entity);
		}
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
