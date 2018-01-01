#pragma once

#include "inspector.h"
#include "runtime/assets/asset_handle.h"

namespace gfx
{
struct texture;
}
namespace runtime
{
struct animation;
}
namespace audio
{
class sound;
}
class mesh;
struct prefab;
class material;

struct inspector_asset_handle_texture : public inspector
{
	REFLECTABLEV(inspector_asset_handle_texture, inspector)

	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_asset_handle_texture, asset_handle<gfx::texture>)

struct inspector_asset_handle_material : public inspector
{
	REFLECTABLEV(inspector_asset_handle_material, inspector)

	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_asset_handle_material, asset_handle<material>)

struct inspector_asset_handle_mesh : public inspector
{
	REFLECTABLEV(inspector_asset_handle_mesh, inspector)

	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_asset_handle_mesh, asset_handle<mesh>)

struct inspector_asset_handle_sound : public inspector
{
	REFLECTABLEV(inspector_asset_handle_sound, inspector)

	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_asset_handle_sound, asset_handle<audio::sound>)

struct inspector_asset_handle_animation : public inspector
{
	REFLECTABLEV(inspector_asset_handle_animation, inspector)

	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_asset_handle_animation, asset_handle<runtime::animation>)

struct inspector_asset_handle_prefab : public inspector
{
	REFLECTABLEV(inspector_asset_handle_prefab, inspector)

	bool inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata);
};
INSPECTOR_REFLECT(inspector_asset_handle_prefab, asset_handle<prefab>)
