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