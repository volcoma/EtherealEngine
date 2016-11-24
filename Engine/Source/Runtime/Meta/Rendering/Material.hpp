#pragma once
#include "Core/reflection/reflection.h"
#include "Core/serialization/serialization.h"
#include "Core/serialization/cereal/types/vector.hpp"
#include "Core/serialization/cereal/types/string.hpp"
#include "Core/serialization/cereal/types/utility.hpp"
#include "../../Rendering/Material.h"
#include "../../Rendering/Program.h"
#include "../Math/Vector.hpp"
#include "../Assets/AssetHandle.hpp"

REFLECT(Material)
{
	rttr::registration::enumeration<CullType>("CullType")
		(
			rttr::value("None", CullType::None),
			rttr::value("Clock-Wise", CullType::ClockWise),
			rttr::value("Counter Clock-Wise", CullType::CounterClockWise)
			);

	rttr::registration::class_<Material>("Material")
		.property("Cull Type",
			&StandardMaterial::getCullType,
			&StandardMaterial::setCullType)
		;
}


SAVE(Material)
{
	ar(
		cereal::make_nvp("cull_type", obj.mCullType)
	);
}

LOAD(Material)
{
	ar(
		cereal::make_nvp("cull_type", obj.mCullType)
	);
}

REFLECT(StandardMaterial)
{
	rttr::registration::class_<StandardMaterial>("StandardMaterial")
		.property("Base Color",
			&StandardMaterial::getBaseColor,
			&StandardMaterial::setBaseColor)
		.property("Specular Color",
			&StandardMaterial::getSpecularColor,
			&StandardMaterial::setSpecularColor)
		.property("Emissive Color",
			&StandardMaterial::getEmissiveColor,
			&StandardMaterial::setEmissiveColor)
		.property("Roughness",
			&StandardMaterial::getRoughness,
			&StandardMaterial::setRoughness)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f)
		)
		.property("Metalness",
			&StandardMaterial::getMetalness,
			&StandardMaterial::setMetalness)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f)
		)
		.property("Bumpiness",
			&StandardMaterial::getBumpiness,
			&StandardMaterial::setBumpiness)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 10.0f)
		)
		.property("Alpha Test Value",
			&StandardMaterial::getAlphaTestValue,
			&StandardMaterial::setAlphaTestValue)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f)
		)
		.property("Tiling",
			&StandardMaterial::getTiling,
			&StandardMaterial::setTiling)
		.property("Dither Threshold",
			&StandardMaterial::getDitherThreshold,
			&StandardMaterial::setDitherThreshold)
		.property("Color Map",
			&StandardMaterial::getColorMap,
			&StandardMaterial::setColorMap)
		.property("Normal Map",
			&StandardMaterial::getNormalMap,
			&StandardMaterial::setNormalMap)
		.property("Roughness Map",
			&StandardMaterial::getRoughnessMap,
			&StandardMaterial::setRoughnessMap)
		.property("Metalness Map",
			&StandardMaterial::getMetalnessMap,
			&StandardMaterial::setMetalnessMap)
		;
}


SAVE(StandardMaterial)
{
	ar(
		cereal::make_nvp("base_type", cereal::base_class<Material>(&obj))
		, cereal::make_nvp("base_color", obj.mBaseColor)
		, cereal::make_nvp("specular_color", obj.mSpecularColor)
		, cereal::make_nvp("emissive_color", obj.mEmissiveColor)
		, cereal::make_nvp("surface_data", obj.mSurfaceData)
		, cereal::make_nvp("tiling", obj.mTiling)
		, cereal::make_nvp("dither_threshold", obj.mDitherThreshold)
		, cereal::make_nvp("color_map", obj.mColorMap)
		, cereal::make_nvp("normal_map", obj.mNormalMap)
		, cereal::make_nvp("roughness_map", obj.mRoughnessMap)
		, cereal::make_nvp("metalness_map", obj.mMetalnessMap)
	);

}

LOAD(StandardMaterial)
{
	ar(
		cereal::make_nvp("base_type", cereal::base_class<Material>(&obj))
		, cereal::make_nvp("base_color", obj.mBaseColor)
		, cereal::make_nvp("specular_color", obj.mSpecularColor)
		, cereal::make_nvp("emissive_color", obj.mEmissiveColor)
		, cereal::make_nvp("surface_data", obj.mSurfaceData)
		, cereal::make_nvp("tiling", obj.mTiling)
		, cereal::make_nvp("dither_threshold", obj.mDitherThreshold)
 		, cereal::make_nvp("color_map", obj.mColorMap)
 		, cereal::make_nvp("normal_map", obj.mNormalMap)
		, cereal::make_nvp("roughness_map", obj.mRoughnessMap)
		, cereal::make_nvp("metalness_map", obj.mMetalnessMap)
	);
}

#include "Core/serialization/archives.h"
CEREAL_REGISTER_TYPE(StandardMaterial);