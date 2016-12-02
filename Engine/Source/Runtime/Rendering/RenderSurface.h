#pragma once
#include "FrameBuffer.h"
#include "Core/common/basetypes.hpp"
#include <vector>
#include <unordered_map>
#include <string>



class RenderSurface
{
public:
	//-----------------------------------------------------------------------------
	//  Name : RenderSurface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderSurface() = default;

	//-----------------------------------------------------------------------------
	//  Name : RenderSurface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderSurface(
		std::uint16_t _width,
		std::uint16_t _height,
		gfx::TextureFormat::Enum _format,
		std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
	)
	{
		populate(_width, _height, _format, _textureFlags);
	}

	//-----------------------------------------------------------------------------
	//  Name : RenderSurface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderSurface(
		gfx::BackbufferRatio::Enum _ratio,
		gfx::TextureFormat::Enum _format,
		std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
	)
	{
		populate(_ratio, _format, _textureFlags);
	}

	//-----------------------------------------------------------------------------
	//  Name : RenderSurface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderSurface(const std::vector<std::shared_ptr<Texture>>& textures)
	{
		populate(textures);
	}

	//-----------------------------------------------------------------------------
	//  Name : RenderSurface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderSurface(const std::vector<TextureAttachment>& textures)
	{
		populate(textures);
	}

	//-----------------------------------------------------------------------------
	//  Name : RenderSurface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderSurface(
		void* _nwh,
		std::uint16_t _width,
		std::uint16_t _height,
		gfx::TextureFormat::Enum _depthFormat = gfx::TextureFormat::UnknownDepth
	)
	{
		populate(_nwh, _width, _height, _depthFormat);
	}
	//-----------------------------------------------------------------------------
	//  Name : ~RenderSurface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~RenderSurface();

	//-----------------------------------------------------------------------------
	//  Name : getSize ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	uSize getSize() const;

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		std::uint16_t _width,
		std::uint16_t _height,
		gfx::TextureFormat::Enum _format,
		std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
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
		gfx::BackbufferRatio::Enum _ratio,
		gfx::TextureFormat::Enum _format,
		std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
	);

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(const std::vector<std::shared_ptr<Texture>>& textures);

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(const std::vector<TextureAttachment>& textures);

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		void* _nwh,
		std::uint16_t _width,
		std::uint16_t _height,
		gfx::TextureFormat::Enum _depthFormat = gfx::TextureFormat::UnknownDepth
	);

	//-----------------------------------------------------------------------------
	//  Name : getBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::shared_ptr<FrameBuffer> getBuffer() const { return mBuffer; }

	//-----------------------------------------------------------------------------
	//  Name : getBufferRaw ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline FrameBuffer* getBufferRaw() const { return mBuffer.get(); }

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

	//-----------------------------------------------------------------------------
	//  Name : getId ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::uint8_t getId() const { return mId; }

	//-----------------------------------------------------------------------------
	//  Name : pushSurface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void pushSurface(std::shared_ptr<RenderSurface> surface);

	//-----------------------------------------------------------------------------
	//  Name : popSurface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void popSurface();

	//-----------------------------------------------------------------------------
	//  Name : clearStack ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	static void clearStack();
private:
	/// Frame buffer
	std::shared_ptr<FrameBuffer> mBuffer = std::make_shared<FrameBuffer>();
	/// View id
	std::uint8_t mId = 255;
};

struct RenderSurfaceScope
{
	//-----------------------------------------------------------------------------
	//  Name : RenderSurfaceScope ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderSurfaceScope(std::shared_ptr<RenderSurface> surface)
	{
		RenderSurface::pushSurface(surface);
	}

	//-----------------------------------------------------------------------------
	//  Name : ~RenderSurfaceScope ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~RenderSurfaceScope()
	{
		RenderSurface::popSurface();
	}
};