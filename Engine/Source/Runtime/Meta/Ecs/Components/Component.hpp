#pragma once
#include "../../../Ecs/entityx/Component.h"
#include "Core/reflection/reflection.h"



REFLECT(entityx::Component)
{
	rttr::registration::class_<entityx::Component>("Component");

}

namespace entityx
{
	SAVE(Component)
	{
		ar(
			cereal::make_nvp("owner", obj.mEntity)
		);
	}

	LOAD(Component)
	{
		ar(
			cereal::make_nvp("owner", obj.mEntity)
		);
	}
}
