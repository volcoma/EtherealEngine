#include "prefab.h"
#include "utils.h"

runtime::entity prefab::instantiate()
{
	std::vector<runtime::entity> outDataVec;
	if(!data)
		return runtime::entity();

	if(!ecs::utils::deserialize_data(*data, outDataVec))
		return runtime::entity();

	if(outDataVec.empty())
		return runtime::entity();
	else
		return outDataVec[0];
}
