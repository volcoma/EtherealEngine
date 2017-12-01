#pragma once

#include "../assets/asset_handle.h"
#include "core/graphics/program.h"

class gpu_program
{
public:
	gpu_program(asset_handle<gfx::shader> compute_shader);

	gpu_program(asset_handle<gfx::shader> vertex_shader, asset_handle<gfx::shader> fragment_shader);

	bool begin();
    void end();
	void add_shader(asset_handle<gfx::shader> shader);

	void populate();

	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::frame_buffer* _handle,
					 uint8_t _attachment = 0,
					 std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

	//-----------------------------------------------------------------------------
	//  Name : set_texture ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_texture(std::uint8_t _stage, const std::string& _sampler, gfx::texture* _texture,
					 std::uint32_t _flags = std::numeric_limits<std::uint32_t>::max());

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
	std::shared_ptr<gfx::uniform> get_uniform(const std::string& _name, bool texture = false);

    gfx::program::handle_type_t native_handle() const;
    
    const std::vector<asset_handle<gfx::shader>>& get_shaders() const;
private:
	/// Shaders that created this program.
	std::vector<asset_handle<gfx::shader>> _shaders;
	/// Shaders that created this program.
	std::vector<std::uint16_t> _shaders_cached;
	/// program
	std::unique_ptr<gfx::program> _program;
};
