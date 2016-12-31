#pragma once
#include "core/serialization/serialization.h"
#include "core/serialization/cereal/types/vector.hpp"
#include "core/reflection/reflection.h"
#include "../../rendering/program.h"
#include "../assets/asset_handle.hpp"

SAVE(Program)
{
	ar(
		cereal::make_nvp("shaders", obj.shaders),
	);

}

LOAD(Program)
{
	std::vector<AssetHandle<Shader>> shaders;
	ar(
		cereal::make_nvp("shaders", shaders),
	);

	for (auto shader : shaders)
	{
		obj.addShader(shader);
	}
	obj.populate();
}
