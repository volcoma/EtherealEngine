#pragma once
#include "load_request.hpp"
#include "core/filesystem/filesystem.h"

struct texture;
struct shader;
class mesh;
class material;
struct prefab;
struct scene;

struct asset_reader
{
	//-----------------------------------------------------------------------------
	//  Name : load_texture_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_texture_from_file(const std::string& key, const fs::path& absoluteKey, bool async, load_request<texture>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : load_shader_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_shader_from_file(const std::string& key, const fs::path& absoluteKey, bool async, load_request<shader>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : load_shader_from_memory ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_shader_from_memory(const std::string& key, const std::uint8_t* data, std::uint32_t size, load_request<shader>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : load_mesh_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_mesh_from_file(const std::string& key, const fs::path& absoluteKey, bool async, load_request<mesh>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : load_material_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_material_from_file(const std::string& key, const fs::path& absoluteKey, bool async, load_request<material>& request);

	//-----------------------------------------------------------------------------
	//  Name : load_prefab_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_prefab_from_file(const std::string& key, const fs::path& absoluteKey, bool async, load_request<prefab>& request);

	//-----------------------------------------------------------------------------
	//  Name : load_scene_from_file ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void load_scene_from_file(const std::string& key, const fs::path& absoluteKey, bool async, load_request<scene>& request);

};