#include "scene.h"
#include "utils.h"

std::vector<runtime::entity> scene::instantiate()
{
	std::vector<runtime::entity> out_vec;
	if (!data)
		return out_vec;
		
	ecs::utils::deserialize_data(*data, out_vec);

	return out_vec;
}
