#pragma once

#include "../assets/asset_handle.h"
#include "graphics/graphics.h"
#include <vector>
#include <unordered_map>
#include <limits>

struct FrameBuffer;
struct Texture;
struct Shader;
struct Uniform;

struct Program
{
	//-----------------------------------------------------------------------------
	//  Name : Program ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	Program() = default;
	
	//-----------------------------------------------------------------------------
	//  Name : Program ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	Program(AssetHandle<Shader> computeShader);
	
	//-----------------------------------------------------------------------------
	//  Name : Program ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	Program(AssetHandle<Shader> vertexShader, AssetHandle<Shader> fragmentShader);
	
	//-----------------------------------------------------------------------------
	//  Name : ~Program ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~Program();

	//-----------------------------------------------------------------------------
	//  Name : dispose ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void dispose();

	//-----------------------------------------------------------------------------
	//  Name : is_valid ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_valid() const;

	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage
		, const std::string& _sampler
		, FrameBuffer* _handle
		, uint8_t _attachment = 0
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());
	
	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage
		, const std::string& _sampler
		, gfx::FrameBufferHandle _handle
		, uint8_t _attachment = 0
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage
		, const std::string& _sampler
		, Texture* _texture
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());
	
	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage
		, const std::string& _sampler
		, gfx::TextureHandle _texture
		, std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());
	
	//-----------------------------------------------------------------------------
	//  Name : set_uniform ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_uniform(const std::string& _name, const void* _value, std::uint16_t _num = 1);

	//-----------------------------------------------------------------------------
	//  Name : get_uniform ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<Uniform> get_uniform(const std::string& _name);

	//-----------------------------------------------------------------------------
	//  Name : add_shader ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void add_shader(AssetHandle<Shader> shader);
	
	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate();

	//-----------------------------------------------------------------------------
	//  Name : begin_pass ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void begin_pass();

	/// Shaders that created this program.
	std::vector<AssetHandle<Shader>> shaders;
	/// Shaders that created this program.
	std::vector<std::uint16_t> shaders_cached;
	/// All uniforms for this program.
	std::unordered_map<std::string, std::shared_ptr<Uniform>> uniforms;
	/// Internal handle
	gfx::ProgramHandle handle = { gfx::invalidHandle };
};