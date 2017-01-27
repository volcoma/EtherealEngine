#pragma once
#include "../../../ecs/components/light_component.h"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../../meta/rendering/light.hpp"


REFLECT(LightComponent)
{
	rttr::registration::class_<LightComponent>("Component/Light")
		.constructor<>()
		(
			rttr::policy::ctor::as_std_shared_ptr,
			rttr::metadata("CanExecuteInEditor", true)
			)
		;
}


SAVE(LightComponent)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::Component>(&obj)));
}

LOAD(LightComponent)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<runtime::Component>(&obj)));
}


#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(LightComponent);