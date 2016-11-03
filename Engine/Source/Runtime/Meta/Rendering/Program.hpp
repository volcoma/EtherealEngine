#pragma once
#include "Core/serialization/serialization.h"
#include "Core/serialization/cereal/types/vector.hpp"
#include "Core/reflection/reflection.h"
#include "../../Rendering/Program.h"
#include "../Assets/AssetHandle.hpp"

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
