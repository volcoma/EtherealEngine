#pragma once
#include "FrameBuffer.h"
#include "Core/common/basetypes.hpp"
#include <vector>
#include <unordered_map>
#include <string>

struct TextureAttachment
{
	/// Texture handle.
	std::shared_ptr<Texture> texture;
	/// Mip level.
	uint16_t mip = 0;
	/// Cubemap side or depth layer/slice.
	uint16_t layer = 0;
};

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
	//  Name : setSize ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setSize(const uSize& size);

	//-----------------------------------------------------------------------------
	//  Name : setSize ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setSize(gfx::BackbufferRatio::Enum ratio);

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
	//  Name : getAttachment ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const TextureAttachment& getAttachment(std::uint32_t index) const { return mTextures[index]; }

	//-----------------------------------------------------------------------------
	//  Name : getAttachmentCount ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const std::size_t getAttachmentCount() const { return mTextures.size(); }

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
	/// Back buffer ratio if any.
	gfx::BackbufferRatio::Enum mRatio = gfx::BackbufferRatio::Equal;
	/// Size of the surface. If {0,0} then it is controlled by backbuffer ratio
	uSize mSize = { 0, 0 };
	/// Texture attachments to the frame buffer
	std::vector<TextureAttachment> mTextures;
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