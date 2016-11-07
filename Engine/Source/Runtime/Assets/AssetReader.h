#pragma once
#include "LoadRequest.hpp"

struct Texture;
struct Shader;
struct Mesh;
class Material;

struct AssetReader
{
	//-----------------------------------------------------------------------------
	//  Name : loadTextureFromFile ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void loadTextureFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Texture>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : loadShaderFromFile ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void loadShaderFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Shader>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : loadShaderFromMemory ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void loadShaderFromMemory(const std::string& relativeKey, const std::uint8_t* data, std::uint32_t size, LoadRequest<Shader>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : loadMeshFromFile ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void loadMeshFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Mesh>& request);
	
	//-----------------------------------------------------------------------------
	//  Name : loadMaterialFromFile ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void loadMaterialFromFile(const std::string& relativeKey, const std::string& absoluteKey, bool async, LoadRequest<Material>& request);
};