#pragma once
#include "../../../Ecs/Components/LightComponent.h"
#include "Core/reflection/reflection.h"
#include "../../../Meta/Rendering/Light.hpp"


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
		cereal::make_nvp("base_type", cereal::base_class<ecs::Component>(&obj))
		);
}

LOAD(LightComponent)
{
	ar(
		cereal::make_nvp("base_type", cereal::base_class<ecs::Component>(&obj))
		);
}


#include "Core/serialization/archives.h"
CEREAL_REGISTER_TYPE(LightComponent);