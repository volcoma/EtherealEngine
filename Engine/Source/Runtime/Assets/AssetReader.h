#pragma once
#include "LoadRequest.hpp"

struct Texture;
struct Shader;
struct Mesh;
class Material;

struct AssetReader
{
	static void loadTextureFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Texture>& request);
	static void loadShaderFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Shader>& request);
	static void loadShaderFromMemory(const std::string& relativeKey, const std::uint8_t* data, std::uint32_t size, LoadRequest<Shader>& request);
	static void loadMeshFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Mesh>& request);
	static void loadMaterialFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Material>& request);
};