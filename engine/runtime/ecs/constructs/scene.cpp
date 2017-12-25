#include "scene.h"
#include "utils.h"

std::vector<runtime::entity> scene::instantiate(mode mod)
{
	if(mod == mode::standard)
	{
		auto& ecs = core::get_subsystem<runtime::entity_component_system>();
		ecs.dispose();
	}

	std::vector<runtime::entity> out_vec;
	if(!data)
		return out_vec;

	ecs::utils::deserialize_data(*data, out_vec);

	return out_vec;
}
