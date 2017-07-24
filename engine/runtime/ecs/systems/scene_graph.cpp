#include "scene_graph.h"
#include "../../system/engine.h"
#include "../components/transform_component.h"
namespace runtime
{
void update_transform(chandle<transform_component> hTransform, std::chrono::duration<float> dt)
{
	auto pTransform = hTransform.lock();
	if(pTransform)
	{
		pTransform->resolve(true, dt.count());

		auto& children = pTransform->get_children();
		for(auto& child : children)
		{
			update_transform(child, dt);
		}
	}
}

void scene_graph::frame_update(std::chrono::duration<float> dt)
{
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	_roots.clear();
	ecs.each<transform_component>([this](runtime::entity e, transform_component& transformComponent) {
		auto parent = transformComponent.get_parent();
		if(parent.expired())
		{
			_roots.push_back(transformComponent.handle());
		}
	});

	for(auto& hComponent : _roots)
	{
		update_transform(hComponent, dt);
	}
}

bool scene_graph::initialize()
{
	runtime::on_frame_update.connect(this, &scene_graph::frame_update);

	return true;
}

void scene_graph::dispose()
{
	runtime::on_frame_update.disconnect(this, &scene_graph::frame_update);
}
}
