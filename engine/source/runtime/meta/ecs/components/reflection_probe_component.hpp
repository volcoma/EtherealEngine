#pragma once
#include "../../../ecs/components/reflection_probe_component.h"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../../meta/rendering/reflection_probe.hpp"


REFLECT(ReflectionProbeComponent)
{

	rttr::registration::class_<ReflectionProbeComponent>("ReflectionProbeComponent")
		.constructor<>()
		(
			rttr::policy::ctor::as_std_shared_ptr,
			rttr::metadata("CanExecuteInEditor", true)
		)
		;

}

SAVE(ReflectionProbeComponent)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::Component>(&obj)));
	try_save(ar, cereal::make_nvp("probe", obj._probe));
}

LOAD(ReflectionProbeComponent)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::Component>(&obj)));
	try_load(ar, cereal::make_nvp("probe", obj._probe));
}


#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(ReflectionProbeComponent);