#include "prefab.h"
#include "utils.h"

runtime::Entity Prefab::instantiate()
{
	std::vector<runtime::Entity> outDataVec;
	if (!data)
		return runtime::Entity();
		
	if(!ecs::utils::deserialize_data(*data, outDataVec))
		return runtime::Entity();

	if (outDataVec.empty())
		return runtime::Entity();
	else
		return outDataVec[0];

}
