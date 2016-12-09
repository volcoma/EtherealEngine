#include "Prefab.h"
#include "Utils.h"

Entity Prefab::instantiate()
{
	std::vector<Entity> outDataVec;
	if (!data)
		return Entity();
		
	if(!ecs::utils::deserializeData(*data, outDataVec))
		return Entity();

	if (outDataVec.empty())
		return Entity();
	else
		return outDataVec[0];

}
