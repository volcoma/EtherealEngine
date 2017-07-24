#pragma once

#include "inspector.h"
#include "runtime/assets/asset_handle.h"

struct texture;
class mesh;
struct prefab;
class material;

struct inspector_asset_handle_texture : public inspector
{
	REFLECTABLE(inspector_asset_handle_texture, inspector)

	bool inspect(rttr::variant& var, bool readOnly,
				 std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(inspector_asset_handle_texture, asset_handle<texture>)

struct inspector_asset_handle_material : public inspector
{
	REFLECTABLE(inspector_asset_handle_material, inspector)

	bool inspect(rttr::variant& var, bool readOnly,
				 std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(inspector_asset_handle_material, asset_handle<material>)

struct inspector_asset_handle_mesh : public inspector
{
	REFLECTABLE(inspector_asset_handle_mesh, inspector)

	bool inspect(rttr::variant& var, bool readOnly,
				 std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(inspector_asset_handle_mesh, asset_handle<mesh>)

struct inspector_asset_handle_prefab : public inspector
{
	REFLECTABLE(inspector_asset_handle_prefab, inspector)

	bool inspect(rttr::variant& var, bool readOnly,
				 std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(inspector_asset_handle_prefab, asset_handle<prefab>)
