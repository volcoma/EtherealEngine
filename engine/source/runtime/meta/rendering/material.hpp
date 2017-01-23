#pragma once
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"
#include "core/serialization/cereal/types/string.hpp"
#include "core/serialization/cereal/types/utility.hpp"
#include "core/logging/logging.h"
#include "../../rendering/material.h"
#include "../../rendering/program.h"
#include "../math/vector.hpp"
#include "../assets/asset_handle.hpp"

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
			&StandardMaterial::get_cull_type,
			&StandardMaterial::set_cull_type)
		;
}


SAVE(Material)
{
	try_save(ar, cereal::make_nvp("cull_type", obj._cull_type));
}

LOAD(Material)
{
	try_load(ar, cereal::make_nvp("cull_type", obj._cull_type));
}

REFLECT(StandardMaterial)
{
	rttr::registration::class_<StandardMaterial>("StandardMaterial")
		.property("Base Color",
			&StandardMaterial::get_base_color,
			&StandardMaterial::set_base_color)
		.property("Emissive Color",
			&StandardMaterial::get_emissive_color,
			&StandardMaterial::set_emissive_color)
		.property("Roughness",
			&StandardMaterial::get_roughness,
			&StandardMaterial::set_roughness)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f)
		)
		.property("Metalness",
			&StandardMaterial::get_metalness,
			&StandardMaterial::set_metalness)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f)
		)
		.property("Bumpiness",
			&StandardMaterial::get_bumpiness,
			&StandardMaterial::set_bumpiness)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 20.0f)
		)
		.property("Alpha Test Value",
			&StandardMaterial::get_alpha_test_value,
			&StandardMaterial::set_alpha_test_value)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f)
		)
		.property("Tiling",
			&StandardMaterial::get_tiling,
			&StandardMaterial::set_tiling)
		.property("Dither Threshold",
			&StandardMaterial::get_dither_threshold,
			&StandardMaterial::set_dither_threshold)
		.property("Color Map",
			&StandardMaterial::get_color_map,
			&StandardMaterial::set_color_map)
		.property("Normal Map",
			&StandardMaterial::get_normal_map,
			&StandardMaterial::set_normal_map)
		.property("Roughness Map",
			&StandardMaterial::get_roughness_map,
			&StandardMaterial::set_roughness_map)
		(
			rttr::metadata("Tooltip", "black/white texture. The more white the rougher the surface.")
		)
		.property("Metalness Map",
			&StandardMaterial::get_metalness_map,
			&StandardMaterial::set_metalness_map)
		(
			rttr::metadata("Tooltip", "black/white texture. The more white the more metallic the surface.")
		)
		;
}


SAVE(StandardMaterial)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<Material>(&obj)));
	try_save(ar, cereal::make_nvp("base_color", obj._base_color));
	try_save(ar, cereal::make_nvp("emissive_color", obj._emissive_color));
	try_save(ar, cereal::make_nvp("surface_data", obj._surface_data));
	try_save(ar, cereal::make_nvp("tiling", obj._tiling));
	try_save(ar, cereal::make_nvp("dither_threshold", obj._dither_threshold));
	try_save(ar, cereal::make_nvp("color_map", obj._color_map));
	try_save(ar, cereal::make_nvp("normal_map", obj._normal_map));
	try_save(ar, cereal::make_nvp("roughness_map", obj._roughness_map));
	try_save(ar, cereal::make_nvp("metalness_map", obj._metalness_map));
}

LOAD(StandardMaterial)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<Material>(&obj)));
	try_load(ar, cereal::make_nvp("base_color", obj._base_color));
	try_load(ar, cereal::make_nvp("emissive_color", obj._emissive_color));
	try_load(ar, cereal::make_nvp("surface_data", obj._surface_data));
	try_load(ar, cereal::make_nvp("tiling", obj._tiling));
	try_load(ar, cereal::make_nvp("dither_threshold", obj._dither_threshold));
	try_load(ar, cereal::make_nvp("color_map", obj._color_map));
	try_load(ar, cereal::make_nvp("normal_map", obj._normal_map));
	try_load(ar, cereal::make_nvp("roughness_map", obj._roughness_map));
	try_load(ar, cereal::make_nvp("metalness_map", obj._metalness_map));
}

#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(StandardMaterial);