#pragma once
#include "frame_buffer.h"
#include "core/common/basetypes.hpp"
#include <vector>
#include <unordered_map>
#include <string>

struct RenderPass
{
	RenderPass(const std::string& n);

	//-----------------------------------------------------------------------------
	//  Name : bind ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void bind(FrameBuffer* fb) const;

	//-----------------------------------------------------------------------------
	//  Name : clear ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear(std::uint16_t _flags
		, std::uint32_t _rgba = 0x000000ff
		, float _depth = 1.0f
		, std::uint8_t _stencil = 0) const;

	//-----------------------------------------------------------------------------
	//  Name : clear ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void clear() const;

	static void reset();
	static std::uint8_t getPass();
	std::uint8_t id;
};