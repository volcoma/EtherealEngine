#pragma once
#include "core/ecs.h"
#include "core/reflection/reflection.h"



REFLECT(core::Component)
{
	rttr::registration::class_<core::Component>("Component");

}

namespace core
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
