#pragma once

#include "handle_impl.h"
#include <limits>
#include <memory>
#include <unordered_map>
#include <vector>

namespace gfx
{
struct frame_buffer;
struct texture;
struct shader;
struct uniform;

struct program : public handle_impl<program_handle>
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
	program(std::shared_ptr<shader> compute_shader);

	//-----------------------------------------------------------------------------
	//  Name : Program ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	program(std::shared_ptr<shader> vertex_shader, std::shared_ptr<shader> fragment_shader);

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

	/// All uniforms for this program.
	std::unordered_map<std::string, std::shared_ptr<gfx::uniform>> uniforms;
};
}
