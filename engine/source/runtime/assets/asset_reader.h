#pragma once
#include "load_request.hpp"
#include "../system/fileSystem.h"

struct Texture;
struct Shader;
class Mesh;
class Material;
struct Prefab;
struct Scene;

struct AssetReader
{
	//-----------------------------------------------------------------------------
	//  Name : load_texture_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_texture_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Texture>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : load_shader_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_shader_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Shader>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : load_shader_from_memory ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_shader_from_memory(const std::string& key, const std::uint8_t* data, std::uint32_t size, LoadRequest<Shader>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : load_mesh_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_mesh_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Mesh>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : load_material_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_material_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Material>& request);

	//-----------------------------------------------------------------------------
	//  Name : load_prefab_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_prefab_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Prefab>& request);

	//-----------------------------------------------------------------------------
	//  Name : load_scene_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_scene_from_file(const std::string& key, const fs::path& absoluteKey, bool async, LoadRequest<Scene>& request);

};