#pragma once
#include "core/common/basetypes.hpp"
#include "core/math/math_includes.h"
#include "frame_buffer.h"
#include <string>
#include <unordered_map>
#include <vector>

struct render_pass
{
	//-----------------------------------------------------------------------------
	//  Name : render_pass ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	render_pass(const std::string& n);

	//-----------------------------------------------------------------------------
	//  Name : bind ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void bind(frame_buffer* fb) const;

	//-----------------------------------------------------------------------------
	//  Name : clear ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear(std::uint16_t _flags, std::uint32_t _rgba = 0x000000ff, float _depth = 1.0f,
			   std::uint8_t _stencil = 0) const;

	//-----------------------------------------------------------------------------
	//  Name : clear ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear() const;

	//-----------------------------------------------------------------------------
	//  Name : set_view_proj ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_view_proj(const math::transform& v, const math::transform& p);
	void set_view_proj_ortho_full(float depth);
	//-----------------------------------------------------------------------------
	//  Name : reset ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	static void reset();

	//-----------------------------------------------------------------------------
	//  Name : get_pass ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	static std::uint8_t get_pass();
    static urect get_pass_viewport();
	///
	std::uint8_t id;
};
