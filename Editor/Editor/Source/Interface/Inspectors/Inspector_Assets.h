#pragma once

#include "Inspector.h"
#include "Runtime/Assets/AssetHandle.h"

struct Texture;
struct Mesh;
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

