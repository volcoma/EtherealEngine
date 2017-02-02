#pragma once

#include "inspector.h"
#include "runtime/assets/asset_handle.h"

struct Texture;
class Mesh;
struct Prefab;
class Material;

struct Inspector_AssetHandle_Texture : public Inspector
{
	REFLECTABLE(Inspector_AssetHandle_Texture, Inspector)

	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_AssetHandle_Texture, AssetHandle<Texture>)


struct Inspector_AssetHandle_Material : public Inspector
{
	REFLECTABLE(Inspector_AssetHandle_Material, Inspector)

	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_AssetHandle_Material, AssetHandle<Material>)


struct Inspector_AssetHandle_Mesh : public Inspector
{
	REFLECTABLE(Inspector_AssetHandle_Mesh, Inspector)

	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_AssetHandle_Mesh, AssetHandle<Mesh>)

struct Inspector_AssetHandle_Prefab : public Inspector
{
	REFLECTABLE(Inspector_AssetHandle_Prefab, Inspector)

	bool inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata);
};
INSPECTOR_REFLECT(Inspector_AssetHandle_Prefab, AssetHandle<Prefab>)

