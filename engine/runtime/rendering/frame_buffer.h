#pragma once

#include "core/graphics/graphics.h"
#include "texture.h"

struct fbo_attachment
{
	/// Texture handle.
	std::shared_ptr<::texture> texture;
	/// Mip level.
	uint16_t mip = 0;
	/// Cubemap side or depth layer/slice.
	uint16_t layer = 0;
};

struct frame_buffer
{
	//-----------------------------------------------------------------------------
	//  Name : frame_buffer ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	frame_buffer() = default;

	//-----------------------------------------------------------------------------
	//  Name : frame_buffer ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	frame_buffer(std::uint16_t _width, std::uint16_t _height, gfx::TextureFormat::Enum _format,
		std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP);

	//-----------------------------------------------------------------------------
	//  Name : frame_buffer ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	frame_buffer(gfx::BackbufferRatio::Enum _ratio, gfx::TextureFormat::Enum _format,
		std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP);

	//-----------------------------------------------------------------------------
	//  Name : frame_buffer ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	frame_buffer(const std::vector<std::shared_ptr<texture>>& textures);

	//-----------------------------------------------------------------------------
	//  Name : frame_buffer ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	frame_buffer(const std::vector<fbo_attachment>& textures);

	//-----------------------------------------------------------------------------
	//  Name : frame_buffer ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	frame_buffer(void* _nwh, std::uint16_t _width, std::uint16_t _height,
		gfx::TextureFormat::Enum _depthFormat = gfx::TextureFormat::UnknownDepth);
	//-----------------------------------------------------------------------------
	//  Name : ~frame_buffer ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	~frame_buffer();

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
	void populate(std::uint16_t _width, std::uint16_t _height, gfx::TextureFormat::Enum _format,
		std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP);

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(gfx::BackbufferRatio::Enum _ratio, gfx::TextureFormat::Enum _format,
		std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP);
	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(const std::vector<std::shared_ptr<texture>>& textures);

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(const std::vector<fbo_attachment>& textures);

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(void* _nwh, std::uint16_t _width, std::uint16_t _height,
		gfx::TextureFormat::Enum _depthFormat = gfx::TextureFormat::UnknownDepth);

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
	//  Name : get_attachment ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	const fbo_attachment& get_attachment(std::uint32_t index) const;

	//-----------------------------------------------------------------------------
	//  Name : get_attachment_count ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::size_t get_attachment_count() const;

	/// Internal handle
	gfx::FrameBufferHandle handle = BGFX_INVALID_HANDLE;
	/// Back buffer ratio if any.
	gfx::BackbufferRatio::Enum _bbratio = gfx::BackbufferRatio::Equal;
	/// Size of the surface. If {0,0} then it is controlled by backbuffer ratio
	usize _cached_size = {0, 0};
	/// Texture attachments to the frame buffer
	std::vector<fbo_attachment> _textures;
};
