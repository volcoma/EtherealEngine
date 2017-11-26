#include "frame_buffer.h"

frame_buffer::frame_buffer(std::uint16_t _width, std::uint16_t _height, gfx::TextureFormat::Enum _format,
	std::uint32_t _textureFlags)
{
	populate(_width, _height, _format, _textureFlags);
}

frame_buffer::frame_buffer(gfx::BackbufferRatio::Enum _ratio, gfx::TextureFormat::Enum _format,
	std::uint32_t _textureFlags)
{
	populate(_ratio, _format, _textureFlags);
}

frame_buffer::frame_buffer(const std::vector<std::shared_ptr<texture>>& textures)
{
	populate(textures);
}

frame_buffer::frame_buffer(const std::vector<fbo_attachment>& textures)
{
	populate(textures);
}

frame_buffer::frame_buffer(void* _nwh, std::uint16_t _width, std::uint16_t _height,
	gfx::TextureFormat::Enum _depthFormat)
{
	populate(_nwh, _width, _height, _depthFormat);
}

frame_buffer::~frame_buffer()
{
	dispose();
}

bool frame_buffer::is_valid() const
{
	return gfx::isValid(handle);
}

void frame_buffer::dispose()
{
	_textures.clear();

	if (is_valid())
		gfx::destroy(handle);

	handle = BGFX_INVALID_HANDLE;
}

void frame_buffer::populate(std::uint16_t _width, std::uint16_t _height, gfx::TextureFormat::Enum _format,
	std::uint32_t _textureFlags)
{
	populate(std::vector<std::shared_ptr<texture>>{
		std::make_shared<texture>(_width, _height, false, 1, _format, _textureFlags),
	});
}

void frame_buffer::populate(gfx::BackbufferRatio::Enum _ratio, gfx::TextureFormat::Enum _format,
	std::uint32_t _textureFlags)
{
	populate(std::vector<std::shared_ptr<texture>>{
		std::make_shared<texture>(_ratio, false, 1, _format, _textureFlags),
	});
}

void frame_buffer::populate(const std::vector<std::shared_ptr<texture>>& textures)
{
	dispose();

	std::vector<fbo_attachment> texDescs;
	texDescs.reserve(textures.size());
	for (auto& tex : textures)
	{
		fbo_attachment texDesc;
		texDesc.texture = tex;
		texDescs.push_back(texDesc);
	}

	populate(texDescs);
}

void frame_buffer::populate(const std::vector<fbo_attachment>& textures)
{

	dispose();

	std::vector<gfx::Attachment> buffer;
	buffer.reserve(textures.size());

	usize size;
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

void frame_buffer::populate(void* _nwh, std::uint16_t _width, std::uint16_t _height,
	gfx::TextureFormat::Enum _depthFormat)
{
	dispose();

	handle = gfx::createFrameBuffer(_nwh, _width, _height, _depthFormat);

	_cached_size = { _width, _height };
	_bbratio = gfx::BackbufferRatio::Count;
}

usize frame_buffer::get_size() const
{
	if (_bbratio == gfx::BackbufferRatio::Count)
	{
		return _cached_size;

	} // End if Absolute
	else
	{
		std::uint16_t width;
		std::uint16_t height;
		gfx::get_size_from_ratio(_bbratio, width, height);
		usize size = { static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height) };
		return size;

	} // End if Relative
}

const fbo_attachment& frame_buffer::get_attachment(std::uint32_t index) const
{
	return _textures[index];
}

std::size_t frame_buffer::get_attachment_count() const
{
	return _textures.size();
}