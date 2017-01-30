#include "scene.h"
#include "utils.h"

std::vector<runtime::Entity> Scene::instantiate()
{
	std::vector<runtime::Entity> out_vec;
	if (!data)
		return out_vec;
		
	ecs::utils::deserialize_data(*data, out_vec);

	return out_vec;
}
