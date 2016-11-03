#pragma once

#include "Core/serialization/serialization.h"
#include "Core/serialization/cereal/types/vector.hpp"
#include "Core/reflection/reflection.h"
#include "../../Rendering/Model.h"
#include "../Assets/AssetHandle.hpp"
REFLECT(Model)
{
	rttr::registration::class_<Model>("Model")
		.property("Levels of Detail",
			&Model::getLods,
			&Model::setLods)
		.property("Materials",
			&Model::getMaterials,
			&Model::setMaterials)
		.property("Transition Time",
			&Model::getTransitionTime,
			&Model::setTransitionTime)
		.property("Max Distance",
			&Model::getMaxDistance,
			&Model::setMaxDistance)
		.property("Min Distance",
			&Model::getMinDistance,
			&Model::setMinDistance)
		;
}

SAVE(Model)
{
	ar(
		cereal::make_nvp("lods", obj.mMeshLods),
		cereal::make_nvp("materials", obj.mMaterials),
		cereal::make_nvp("transition_time", obj.mTransitionTime),
		cereal::make_nvp("max_distance", obj.mMaxDistance),
		cereal::make_nvp("min_distance", obj.mMinDistance)
	);

}

LOAD(Model)
{
	ar(
		cereal::make_nvp("lods", obj.mMeshLods),
		cereal::make_nvp("materials", obj.mMaterials),
		cereal::make_nvp("transition_time", obj.mTransitionTime),
		cereal::make_nvp("max_distance", obj.mMaxDistance),
		cereal::make_nvp("min_distance", obj.mMinDistance)
	);
}