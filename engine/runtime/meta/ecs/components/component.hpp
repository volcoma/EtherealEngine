#pragma once
#include "../../../ecs/ecs.h"
#include "core/reflection/reflection.h"
#include "../entity.hpp"

REFLECT(runtime::component)
{
	rttr::registration::class_<runtime::component>("component");

}

namespace runtime
{
	SAVE(component)
	{
		try_save(ar, cereal::make_nvp("owner", obj._entity));
	}

	LOAD(component)
	{
		try_load(ar, cereal::make_nvp("owner", obj._entity));
	}
}
