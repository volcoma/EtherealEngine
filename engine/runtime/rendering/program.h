#pragma once

#include "../assets/asset_handle.h"
#include "core/graphics/graphics.h"
#include <vector>
#include <unordered_map>
#include <limits>

struct frame_buffer;
struct texture;
struct shader;
struct uniform;

struct program
{
	//-----------------------------------------------------------------------------
	//  Name : Program ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	program() = default;
	
	//-----------------------------------------------------------------------------
	//  Name : Program ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	program(asset_handle<shader> computeShader);
	
	//-----------------------------------------------------------------------------
	//  Name : Program ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	program(asset_handle<shader> vertexShader, asset_handle<shader> fragmentShader);
	
	//-----------------------------------------------------------------------------
	//  Name : ~Program ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~program();

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
		, frame_buffer* _handle
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
		, texture* _texture
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
	std::shared_ptr<uniform> get_uniform(const std::string& _name, bool texture = false);

	//-----------------------------------------------------------------------------
	//  Name : add_shader ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void add_shader(asset_handle<shader> shader);
	
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
	bool begin_pass();

	/// Shaders that created this program.
	std::vector<asset_handle<shader>> shaders;
	/// Shaders that created this program.
	std::vector<std::uint16_t> shaders_cached;
	/// All uniforms for this program.
	std::unordered_map<std::string, std::shared_ptr<uniform>> uniforms;
	/// Internal handle
	gfx::ProgramHandle handle = { gfx::kInvalidHandle };
};