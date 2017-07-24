#pragma once
#include "../../rendering/reflection_probe.h"
#include "core/reflection/reflection.h"
#include "core/serialization/serialization.h"

REFLECT(reflection_probe)
{
	rttr::registration::enumeration<probe_type>("probe_type")(rttr::value("box", probe_type::box),
															  rttr::value("sphere", probe_type::sphere));
	rttr::registration::enumeration<reflect_method>("reflect_method")(
		rttr::value("environment", reflect_method::environment),
		rttr::value("static_only", reflect_method::static_only));
	rttr::registration::class_<reflection_probe::box>("box")
		.property("extents", &reflection_probe::box::extents)(rttr::metadata("pretty_name", "Extents"))
		.property("transition_distance", &reflection_probe::box::transition_distance)(
			rttr::metadata("pretty_name", "Transition Distance"));
	rttr::registration::class_<reflection_probe::sphere>("sphere").property(
		"range", &reflection_probe::sphere::range)(rttr::metadata("pretty_name", "Range"));
	rttr::registration::class_<reflection_probe>("reflection_probe")
		.property("type", &reflection_probe::type)(rttr::metadata("pretty_name", "Type"))
		.property("method", &reflection_probe::method)(rttr::metadata("pretty_name", "Method"));
}

SAVE_EXTERN(reflection_probe);
LOAD_EXTERN(reflection_probe);
