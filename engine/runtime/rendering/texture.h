#pragma once

#include "core/graphics/graphics.h"
#include "core/common/basetypes.hpp"
#include <memory>

struct texture
{

	//-----------------------------------------------------------------------------
	//  Name : Texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	texture() = default;
	
	//-----------------------------------------------------------------------------
	//  Name : Texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	texture(const gfx::Memory* _mem
		, std::uint32_t _flags = BGFX_TEXTURE_NONE
		, std::uint8_t _skip = 0
		, gfx::TextureInfo* _info = nullptr)
	{
		populate(_mem, _flags, _skip, _info);
	}

	//-----------------------------------------------------------------------------
	//  Name : Texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	texture(std::uint16_t _width
		, std::uint16_t _height
		, bool _hasMips
		, std::uint16_t _numLayers
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = BGFX_TEXTURE_NONE
		, const gfx::Memory* _mem = nullptr
	)
	{
		populate(_width, _height, _hasMips, _numLayers, _format, _flags, _mem);
	}

	//-----------------------------------------------------------------------------
	//  Name : Texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	texture(gfx::BackbufferRatio::Enum _ratio
		, bool _hasMips
		, std::uint16_t _numLayers
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = BGFX_TEXTURE_NONE
	)
	{
		populate(_ratio, _hasMips, _numLayers, _format, _flags);
	}

	//-----------------------------------------------------------------------------
	//  Name : Texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	texture(std::uint16_t _width
		, std::uint16_t _height
		, std::uint16_t _depth
		, bool _hasMips
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = BGFX_TEXTURE_NONE
		, const gfx::Memory* _mem = nullptr
	)
	{
		populate(_width, _height, _depth, _hasMips, _format, _flags, _mem);
	}

	//-----------------------------------------------------------------------------
	//  Name : Texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	texture(std::uint16_t _size
		, bool _hasMips
		, std::uint16_t _numLayers
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = BGFX_TEXTURE_NONE
		, const gfx::Memory* _mem = nullptr
	)
	{
		populate(_size, _hasMips, _numLayers, _format, _flags, _mem);
	}

	//-----------------------------------------------------------------------------
	//  Name : ~Texture ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~texture();

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
	//  Name : dispose ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void dispose();

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(const gfx::Memory* _mem
		, std::uint32_t _flags = BGFX_TEXTURE_NONE
		, std::uint8_t _skip = 0
		, gfx::TextureInfo* _info = nullptr);

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		std::uint16_t _width
		, std::uint16_t _height
		, bool     _hasMips
		, std::uint16_t _numLayers
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = BGFX_TEXTURE_NONE
		, const gfx::Memory* _mem = nullptr
	);

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		gfx::BackbufferRatio::Enum _ratio
		, bool _hasMips
		, std::uint16_t _numLayers
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = BGFX_TEXTURE_NONE
	);

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		std::uint16_t _width
		, std::uint16_t _height
		, std::uint16_t _depth
		, bool _hasMips
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = BGFX_TEXTURE_NONE
		, const gfx::Memory* _mem = nullptr
	);

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		std::uint16_t _size
		, bool _hasMips
		, std::uint16_t _numLayers
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = BGFX_TEXTURE_NONE
		, const gfx::Memory* _mem = nullptr
	);

	//-----------------------------------------------------------------------------
	//  Name : get_size ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	usize get_size() const;

	//-----------------------------------------------------------------------------
	//  Name : is_render_target ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool is_render_target() const { return 0 != (flags & BGFX_TEXTURE_RT_MASK); }

	/// Texture detail info.
	gfx::TextureInfo info;
	/// Creation flags.
	std::uint32_t flags = BGFX_TEXTURE_NONE;
	/// Back buffer ratio if any.
	gfx::BackbufferRatio::Enum ratio = gfx::BackbufferRatio::Count;
	/// Internal handle
	gfx::TextureHandle handle = { gfx::kInvalidHandle };
};