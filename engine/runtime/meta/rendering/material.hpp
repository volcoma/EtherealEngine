#pragma once
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/unordered_map.hpp"
#include "core/serialization/cereal/types/string.hpp"
#include "core/serialization/cereal/types/utility.hpp"
#include "core/logging/logging.h"
#include "../../rendering/material.h"
#include "../../rendering/program.h"
#include "../math/vector.hpp"
#include "../assets/asset_handle.hpp"

REFLECT(material)
{
	rttr::registration::enumeration<cull_type>("CullType")
		(
			rttr::value("None", cull_type::none),
			rttr::value("Clock-Wise", cull_type::clockwise),
			rttr::value("Counter Clock-Wise", cull_type::counter_clockwise)
			);

	rttr::registration::class_<material>("material")
		.property("Cull Type",
			&standard_material::get_cull_type,
			&standard_material::set_cull_type)
		;
}


SAVE(material)
{
	try_save(ar, cereal::make_nvp("cull_type", obj._cull_type));
}

LOAD(material)
{
	try_load(ar, cereal::make_nvp("cull_type", obj._cull_type));
}

REFLECT(standard_material)
{
	rttr::registration::class_<standard_material>("standard_material")
		.property("Base Color",
			&standard_material::get_base_color,
			&standard_material::set_base_color)
		.property("Subsurface Color",
			&standard_material::get_subsurface_color,
			&standard_material::set_subsurface_color)
		.property("Emissive Color",
			&standard_material::get_emissive_color,
			&standard_material::set_emissive_color)
		.property("Roughness",
			&standard_material::get_roughness,
			&standard_material::set_roughness)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f)
		)
		.property("Metalness",
			&standard_material::get_metalness,
			&standard_material::set_metalness)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f)
		)
		.property("Bumpiness",
			&standard_material::get_bumpiness,
			&standard_material::set_bumpiness)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 10.0f)
		)
		.property("Alpha Test Value",
			&standard_material::get_alpha_test_value,
			&standard_material::set_alpha_test_value)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f)
		)
		.property("Tiling",
			&standard_material::get_tiling,
			&standard_material::set_tiling)
		.property("Dither Threshold",
			&standard_material::get_dither_threshold,
			&standard_material::set_dither_threshold)
		.property("Color Map",
			&standard_material::get_color_map,
			&standard_material::set_color_map)
		.property("Normal Map",
			&standard_material::get_normal_map,
			&standard_material::set_normal_map)
		.property("Roughness Map",
			&standard_material::get_roughness_map,
			&standard_material::set_roughness_map)
		(
			rttr::metadata("Tooltip", "black/white texture. The more white the rougher the surface.")
		)
		.property("Metalness Map",
			&standard_material::get_metalness_map,
			&standard_material::set_metalness_map)
		(
			rttr::metadata("Tooltip", "black/white texture. The more white the more metallic the surface.")
		)
		.property("AO Map",
			&standard_material::get_ao_map,
			&standard_material::set_ao_map)
		(
			rttr::metadata("Tooltip", "black/white texture.")
		)
		;
}


SAVE(standard_material)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<material>(&obj)));
	try_save(ar, cereal::make_nvp("base_color", obj._base_color));
	try_save(ar, cereal::make_nvp("subsurface_color", obj._subsurface_color));
	try_save(ar, cereal::make_nvp("emissive_color", obj._emissive_color));
	try_save(ar, cereal::make_nvp("surface_data", obj._surface_data));
	try_save(ar, cereal::make_nvp("tiling", obj._tiling));
	try_save(ar, cereal::make_nvp("dither_threshold", obj._dither_threshold));
	try_save(ar, cereal::make_nvp("maps", obj._maps));
}

LOAD(standard_material)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<material>(&obj)));
	try_load(ar, cereal::make_nvp("base_color", obj._base_color));
	try_load(ar, cereal::make_nvp("subsurface_color", obj._subsurface_color));
	try_load(ar, cereal::make_nvp("emissive_color", obj._emissive_color));
	try_load(ar, cereal::make_nvp("surface_data", obj._surface_data));
	try_load(ar, cereal::make_nvp("tiling", obj._tiling));
	try_load(ar, cereal::make_nvp("dither_threshold", obj._dither_threshold));
	try_load(ar, cereal::make_nvp("maps", obj._maps));
}

#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(standard_material);