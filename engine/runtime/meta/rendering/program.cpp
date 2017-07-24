#include "program.hpp"
#include "../assets/asset_handle.hpp"
#include "core/serialization/associative_archive.h"
#include "core/serialization/types/vector.hpp"

SAVE(program)
{
	try_save(ar, cereal::make_nvp("shaders", obj.shaders));
}
SAVE_INSTANTIATE(program, cereal::oarchive_associative_t);

LOAD(program)
{
	std::vector<asset_handle<shader>> shaders;

	try_load(ar, cereal::make_nvp("shaders", shaders));

	for(auto shader : shaders)
	{
		obj.add_shader(shader);
	}
	obj.populate();
}
LOAD_INSTANTIATE(program, cereal::iarchive_associative_t);
