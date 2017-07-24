#pragma once
#include "../../rendering/material.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT(material)
{
	rttr::registration::enumeration<cull_type>("cull_type")(
		rttr::value("none", cull_type::none), rttr::value("clockwise", cull_type::clockwise),
		rttr::value("counter_clockwise", cull_type::counter_clockwise));

	rttr::registration::class_<material>("material")
		.property("cull_type", &material::get_cull_type,
				  &material::set_cull_type)(rttr::metadata("pretty_name", "Cull Type"));
}

SAVE_EXTERN(material);
LOAD_EXTERN(material);
