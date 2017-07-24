#include "standard_material.hpp"
#include "../assets/asset_handle.hpp"
#include "core/meta/math/vector.hpp"
#include "core/serialization/types/string.hpp"
#include "core/serialization/types/unordered_map.hpp"
#include "core/serialization/types/utility.hpp"
#include "material.hpp"

SAVE(standard_material)
{
	try_save(ar, cereal::make_nvp("base_type", cereal::base_class<material>(&obj)));
	try_save(ar, cereal::make_nvp("base_color", obj._base_color));
	try_save(ar, cereal::make_nvp("subsurface_color", obj._subsurface_color));
	try_save(ar, cereal::make_nvp("emissive_color", obj._emissive_color));
	try_save(ar, cereal::make_nvp("surface_data", obj._surface_data));
	try_save(ar, cereal::make_nvp("tiling", obj._tiling));
	try_save(ar, cereal::make_nvp("dither_threshold", obj._dither_threshold));
	try_save(ar, cereal::make_nvp("maps", obj._maps));
}
SAVE_INSTANTIATE(standard_material, cereal::oarchive_associative_t);

LOAD(standard_material)
{
	try_load(ar, cereal::make_nvp("base_type", cereal::base_class<material>(&obj)));
	try_load(ar, cereal::make_nvp("base_color", obj._base_color));
	try_load(ar, cereal::make_nvp("subsurface_color", obj._subsurface_color));
	try_load(ar, cereal::make_nvp("emissive_color", obj._emissive_color));
	try_load(ar, cereal::make_nvp("surface_data", obj._surface_data));
	try_load(ar, cereal::make_nvp("tiling", obj._tiling));
	try_load(ar, cereal::make_nvp("dither_threshold", obj._dither_threshold));
	try_load(ar, cereal::make_nvp("maps", obj._maps));
}
LOAD_INSTANTIATE(standard_material, cereal::iarchive_associative_t);
