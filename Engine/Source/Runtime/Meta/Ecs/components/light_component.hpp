#pragma once
#include "../../../ecs/components/light_component.h"
#include "core/reflection/reflection.h"
#include "../../../meta/rendering/light.hpp"


REFLECT(LightComponent)
{
	rttr::registration::class_<LightComponent>("LightComponent")
		.constructor<>()
		(
			rttr::policy::ctor::as_std_shared_ptr,
			rttr::metadata("CanExecuteInEditor", true)
			)
		;
}


SAVE(LightComponent)
{
	ar(
		cereal::make_nvp("base_type", cereal::base_class<core::Component>(&obj))
		);
}

LOAD(LightComponent)
{
	ar(
		cereal::make_nvp("base_type", cereal::base_class<core::Component>(&obj))
		);
}


#include "core/serialization/archives.h"
CEREAL_REGISTER_TYPE(LightComponent);