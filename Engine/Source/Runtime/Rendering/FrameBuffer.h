#pragma once

#include "Texture.h"
#include "Graphics/graphics.h"

struct FrameBuffer : public ITexture
{
	REFLECTABLE(FrameBuffer, ITexture)

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer() = default;

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer(
		std::uint16_t _width
		, std::uint16_t _height
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
	)
	{
		populate(_width, _height, _format, _textureFlags);
	}

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer(
		gfx::BackbufferRatio::Enum _ratio
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
	)
	{
		populate(_ratio, _format, _textureFlags);
	}

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer(
		std::uint8_t _num
		, const gfx::TextureHandle* _handles
		, bool _destroyTextures = false
	)
	{
		populate(_num, _handles, _destroyTextures);
	}

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer(
		std::uint8_t _num
		, const gfx::Attachment* _attachment
		, bool _destroyTextures = false
	)
	{
		populate(_num, _attachment, _destroyTextures);
	}

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer(
		void* _nwh
		, std::uint16_t _width
		, std::uint16_t _height
		, gfx::TextureFormat::Enum _depthFormat = gfx::TextureFormat::UnknownDepth
	)
	{
		populate(_nwh, _width, _height, _depthFormat);
	}
	//-----------------------------------------------------------------------------
	//  Name : ~FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~FrameBuffer()
	{
		dispose();
	}

	//-----------------------------------------------------------------------------
	//  Name : isValid ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool isValid() const
	{
		return gfx::isValid(handle);
	}

	//-----------------------------------------------------------------------------
	//  Name : dispose ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void dispose()
	{
		if (isValid())
			gfx::destroyFrameBuffer(handle);

		handle = { bgfx::invalidHandle };
	}

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
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
	)
	{
		dispose();

		handle = gfx::createFrameBuffer(_width, _height, _format, _textureFlags);
	}

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
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
	)
	{
		dispose();

		handle = gfx::createFrameBuffer(_ratio, _format, _textureFlags);
	}

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		std::uint8_t _num
		, const gfx::TextureHandle* _handles
		, bool _destroyTextures = false
	)
	{
		dispose();

		handle = gfx::createFrameBuffer(_num, _handles, _destroyTextures);
	}

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		std::uint8_t _num
		, const gfx::Attachment* _attachment
		, bool _destroyTextures = false
	)
	{
		dispose();

		handle = gfx::createFrameBuffer(_num, _attachment, _destroyTextures);
	}

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		void* _nwh
		, std::uint16_t _width
		, std::uint16_t _height
		, gfx::TextureFormat::Enum _depthFormat = gfx::TextureFormat::UnknownDepth
	)
	{
		dispose();

		handle = gfx::createFrameBuffer(_nwh, _width, _height, _depthFormat);
	}

	virtual inline bool isRenderTarget() const { return true; }

	/// Internal handle
	gfx::FrameBufferHandle handle = { gfx::invalidHandle };
};