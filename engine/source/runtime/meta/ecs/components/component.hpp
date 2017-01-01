#pragma once
#include "../../../ecs/ecs.h"
#include "core/reflection/reflection.h"



REFLECT(runtime::Component)
{
	rttr::registration::class_<runtime::Component>("Component");

}

namespace runtime
{
	SAVE(Component)
	{
		ar(
			cereal::make_nvp("owner", obj._entity)
		);
	}

	LOAD(Component)
	{
		ar(
			cereal::make_nvp("owner", obj._entity)
		);
	}
}
