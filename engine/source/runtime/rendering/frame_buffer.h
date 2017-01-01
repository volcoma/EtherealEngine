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
	//  Name : is_valid ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool is_valid() const
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
		_textures.clear();

		if (is_valid())
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

		_bbratio = gfx::BackbufferRatio::Count;
		_cached_size = { _width, _height };
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
		_bbratio = _ratio;
		_cached_size = { 0, 0 };
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
		_textures = textures;

		handle = gfx::createFrameBuffer(static_cast<std::uint8_t>(buffer.size()), &buffer[0], false);

		if (ratio == gfx::BackbufferRatio::Count)
		{
			_bbratio = ratio;
			_cached_size = size;
		}
		else
		{
			_bbratio = ratio;
			_cached_size = { 0, 0 };
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

		_cached_size = { _width, _height };
		_bbratio = gfx::BackbufferRatio::Count;
	}


	//-----------------------------------------------------------------------------
	//  Name : get_size ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	uSize get_size() const
	{
		if (_bbratio == gfx::BackbufferRatio::Count)
		{
			return _cached_size;

		} // End if Absolute
		else
		{
			std::uint16_t width;
			std::uint16_t height;
			gfx::getSizeFromRatio(_bbratio, width, height);
			uSize size =
			{
				static_cast<std::uint32_t>(width),
				static_cast<std::uint32_t>(height)
			};
			return size;

		} // End if Relative
	}

	//-----------------------------------------------------------------------------
	//  Name : get_attachment ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const TextureAttachment& get_attachment(std::uint32_t index) const { return _textures[index]; }

	//-----------------------------------------------------------------------------
	//  Name : get_attachment_count ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const std::size_t get_attachment_count() const { return _textures.size(); }

	//-----------------------------------------------------------------------------
	//  Name : is_render_target (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual inline bool is_render_target() const { return true; }

	/// Internal handle
	gfx::FrameBufferHandle handle = { gfx::invalidHandle };
	/// Back buffer ratio if any.
	gfx::BackbufferRatio::Enum _bbratio = gfx::BackbufferRatio::Equal;
	/// Size of the surface. If {0,0} then it is controlled by backbuffer ratio
	uSize _cached_size = { 0, 0 };
	/// Texture attachments to the frame buffer
	std::vector<TextureAttachment> _textures;
};