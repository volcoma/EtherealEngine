#pragma once

#include "texture.h"
#include "graphics/graphics.h"

struct TextureAttachment
{
	/// Texture handle.
	std::shared_ptr<Texture> texture;
	/// Mip level.
	uint16_t mip = 0;
	/// Cubemap side or depth layer/slice.
	uint16_t layer = 0;
};

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
	FrameBuffer(const std::vector<std::shared_ptr<Texture>>& textures)
	{
		populate(textures);
	}

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer(const std::vector<TextureAttachment>& textures)
	{
		populate(textures);
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
		mTextures.clear();

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

		mRatio = gfx::BackbufferRatio::Count;
		mCachedSize = { _width, _height };
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
		mRatio = _ratio;
		mCachedSize = { 0, 0 };
	}

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(const std::vector<std::shared_ptr<Texture>>& textures)
	{
		dispose();

		std::vector<TextureAttachment> texDescs;
		texDescs.reserve(textures.size());
		for (auto& tex : textures)
		{
			TextureAttachment texDesc;
			texDesc.texture = tex;
			texDescs.push_back(texDesc);
		}

		populate(texDescs);
	}

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(const std::vector<TextureAttachment>& textures)
	{

		dispose();

		std::vector<gfx::Attachment> buffer;
		buffer.reserve(textures.size());

		uSize size;
		gfx::BackbufferRatio::Enum ratio = gfx::BackbufferRatio::Enum::Count;
		for (auto& tex : textures)
		{
			ratio = tex.texture->ratio;
			size = { tex.texture->info.width, tex.texture->info.height };
			gfx::Attachment attachment;
			attachment.handle = tex.texture->handle;
			attachment.mip = tex.mip;
			attachment.layer = tex.layer;
			buffer.push_back(attachment);
		}
		mTextures = textures;

		handle = gfx::createFrameBuffer(static_cast<std::uint8_t>(buffer.size()), &buffer[0], false);

		if (ratio == gfx::BackbufferRatio::Count)
		{
			mRatio = ratio;
			mCachedSize = size;
		}
		else
		{
			mRatio = ratio;
			mCachedSize = { 0, 0 };
		}
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

		mCachedSize = { _width, _height };
		mRatio = gfx::BackbufferRatio::Count;
	}


	//-----------------------------------------------------------------------------
	//  Name : getSize ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	uSize getSize() const
	{
		if (mRatio == gfx::BackbufferRatio::Count)
		{
			return mCachedSize;

		} // End if Absolute
		else
		{
			std::uint16_t width;
			std::uint16_t height;
			gfx::getSizeFromRatio(mRatio, width, height);
			uSize size =
			{
				static_cast<std::uint32_t>(width),
				static_cast<std::uint32_t>(height)
			};
			return size;

		} // End if Relative
	}

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

	virtual inline bool isRenderTarget() const { return true; }

	/// Internal handle
	gfx::FrameBufferHandle handle = { gfx::invalidHandle };
	/// Back buffer ratio if any.
	gfx::BackbufferRatio::Enum mRatio = gfx::BackbufferRatio::Equal;
	/// Size of the surface. If {0,0} then it is controlled by backbuffer ratio
	uSize mCachedSize = { 0, 0 };
	/// Texture attachments to the frame buffer
	std::vector<TextureAttachment> mTextures;
};