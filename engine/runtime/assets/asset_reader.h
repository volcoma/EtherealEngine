#pragma once
#include "asset_request.hpp"
#include "core/filesystem/filesystem.h"

struct texture;
struct shader;
class mesh;
class material;
struct prefab;
struct scene;

namespace runtime
{
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
		static void load_texture_from_file(const std::string& key, const fs::path& absoluteKey, bool async, request<texture>& request);

		//-----------------------------------------------------------------------------
		//  Name : load_shader_from_file ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		static void load_shader_from_file(const std::string& key, const fs::path& absoluteKey, bool async, request<shader>& request);

		//-----------------------------------------------------------------------------
		//  Name : load_shader_from_memory ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		static void load_shader_from_memory(const std::string& key, const std::uint8_t* data, std::uint32_t size, request<shader>& request);

		//-----------------------------------------------------------------------------
		//  Name : load_mesh_from_file ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		static void load_mesh_from_file(const std::string& key, const fs::path& absoluteKey, bool async, request<mesh>& request);

		//-----------------------------------------------------------------------------
		//  Name : load_material_from_file ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		static void load_material_from_file(const std::string& key, const fs::path& absoluteKey, bool async, request<material>& request);

		//-----------------------------------------------------------------------------
		//  Name : load_prefab_from_file ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		static void load_prefab_from_file(const std::string& key, const fs::path& absoluteKey, bool async, request<prefab>& request);

		//-----------------------------------------------------------------------------
		//  Name : load_scene_from_file ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		static void load_scene_from_file(const std::string& key, const fs::path& absoluteKey, bool async, request<scene>& request);

	};
}