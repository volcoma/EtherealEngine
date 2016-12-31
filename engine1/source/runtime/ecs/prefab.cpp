#include "Prefab.h"
#include "Utils.h"

core::Entity Prefab::instantiate()
{
	std::vector<core::Entity> outDataVec;
	if (!data)
		return core::Entity();
		
	if(!ecs::utils::deserializeData(*data, outDataVec))
		return core::Entity();

	if (outDataVec.empty())
		return core::Entity();
	else
		return outDataVec[0];

}
