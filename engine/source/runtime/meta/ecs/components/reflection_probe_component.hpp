#pragma once
#include "../../../ecs/components/reflection_probe_component.h"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../../meta/rendering/reflection_probe.hpp"


REFLECT(reflection_probe_component)
{

	rttr::registration::class_<reflection_probe_component>("reflection_probe_component")
		(
			rttr::metadata("Category", "Lighting"),
			rttr::metadata("Id", "Reflection Probe")
		)
		.constructor<>()
		(		
			rttr::policy::ctor::as_std_shared_ptr
		);

}

SAVE(reflection_probe_component)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_save(ar, cereal::make_nvp("probe", obj._probe));
}

LOAD(reflection_probe_component)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::component>(&obj)));
	try_load(ar, cereal::make_nvp("probe", obj._probe));
}


#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(reflection_probe_component);