#pragma once
#include "core/serialization/serialization.h"
#include "core/serialization/types/vector.hpp"
#include "core/reflection/reflection.h"
#include "../../rendering/program.h"
#include "../assets/asset_handle.hpp"

SAVE(program)
{
    try_save(ar, cereal::make_nvp("shaders", obj.shaders));
}

LOAD(program)
{
	std::vector<asset_handle<shader>> shaders;

    try_load(ar, cereal::make_nvp("shaders", shaders));

	for (auto shader : shaders)
	{
		obj.add_shader(shader);
	}
	obj.populate();
}
