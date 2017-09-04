#include "prefab.h"
#include "utils.h"

runtime::entity prefab::instantiate()
{
	std::vector<runtime::entity> out_data;
	if(!data)
		return {};

	if(!ecs::utils::deserialize_data(*data, out_data))
		return {};

	if(out_data.empty())
		return {};
	else
		return out_data.front();
}
