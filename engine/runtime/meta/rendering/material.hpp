#pragma once
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"
#include "core/serialization/types/unordered_map.hpp"
#include "core/serialization/types/string.hpp"
#include "core/serialization/types/utility.hpp"
#include "../../rendering/material.h"
#include "../../rendering/program.h"
#include "core/meta/math/vector.hpp"
#include "../assets/asset_handle.hpp"

REFLECT(material)
{
	rttr::registration::enumeration<cull_type>("cull_type")
		(
			rttr::value("none", cull_type::none),
			rttr::value("clockwise", cull_type::clockwise),
			rttr::value("counter_clockwise", cull_type::counter_clockwise)
			);

	rttr::registration::class_<material>("material")
		.property("cull_type",
			&standard_material::get_cull_type,
			&standard_material::set_cull_type)
		(
			rttr::metadata("pretty_name", "Cull Type")
		)
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
		.property("base_color",
			&standard_material::get_base_color,
			&standard_material::set_base_color)
		(
			rttr::metadata("pretty_name", "Base Color")
		)
		.property("",
			&standard_material::get_subsurface_color,
			&standard_material::set_subsurface_color)
		(
			rttr::metadata("pretty_name", "Subsurface Color")
		)
		.property("emissive_color",
			&standard_material::get_emissive_color,
			&standard_material::set_emissive_color)
		(
			rttr::metadata("pretty_name", "Emissive Color")
		)
		.property("roughness",
			&standard_material::get_roughness,
			&standard_material::set_roughness)
		(
			rttr::metadata("pretty_name", "Roughness"),
			rttr::metadata("min", 0.0f),
			rttr::metadata("max", 1.0f)
		)
		.property("metalness",
			&standard_material::get_metalness,
			&standard_material::set_metalness)
		(
			rttr::metadata("pretty_name", "Metalness"),
			rttr::metadata("min", 0.0f),
			rttr::metadata("max", 1.0f)
		)
		.property("bumpiness",
			&standard_material::get_bumpiness,
			&standard_material::set_bumpiness)
		(
			rttr::metadata("pretty_name", "Bumpiness"),
			rttr::metadata("min", 0.0f),
			rttr::metadata("max", 10.0f)
		)
		.property("alpha_test_value",
			&standard_material::get_alpha_test_value,
			&standard_material::set_alpha_test_value)
		(
			rttr::metadata("pretty_name", "Alpha Test Value"),
			rttr::metadata("min", 0.0f),
			rttr::metadata("max", 1.0f)
		)
		.property("tiling",
			&standard_material::get_tiling,
			&standard_material::set_tiling)
		(
			rttr::metadata("pretty_name", "Tiling")
		)
		.property("dither_threshold",
			&standard_material::get_dither_threshold,
			&standard_material::set_dither_threshold)
		(
			rttr::metadata("pretty_name", "Dither Threshold")
		)
		.property("color_map",
			&standard_material::get_color_map,
			&standard_material::set_color_map)
		(
			rttr::metadata("pretty_name", "Color Map")
		)
		.property("normal_map",
			&standard_material::get_normal_map,
			&standard_material::set_normal_map)
		(
			rttr::metadata("pretty_name", "Normal Map")
		)
		.property("roughness_map",
			&standard_material::get_roughness_map,
			&standard_material::set_roughness_map)
		(
			rttr::metadata("pretty_name", "Roughness Map"),
			rttr::metadata("Tooltip", "black/white texture. The more white the rougher the surface.")
		)
		.property("metalness_map",
			&standard_material::get_metalness_map,
			&standard_material::set_metalness_map)
		(
			rttr::metadata("pretty_name", "Metalness Map"),
			rttr::metadata("Tooltip", "black/white texture. The more white the more metallic the surface.")
		)
		.property("ao_map",
			&standard_material::get_ao_map,
			&standard_material::set_ao_map)
		(
			rttr::metadata("pretty_name", "AO Map"),
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

#include "core/serialization/associative_archive.h"
CEREAL_REGISTER_TYPE(standard_material)
