#pragma once
#include "../../../ecs/ecs.h"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"


REFLECT(runtime::Component)
{
	rttr::registration::class_<runtime::Component>("Component");

}

namespace runtime
{
	SAVE(Component)
	{
		try_save(ar, cereal::make_nvp("owner", obj._entity));
	}

	LOAD(Component)
	{
		try_load(ar, cereal::make_nvp("owner", obj._entity));
	}
}
