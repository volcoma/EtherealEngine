#include "frame_buffer.h"

namespace gfx
{

frame_buffer::frame_buffer(std::uint16_t _width, std::uint16_t _height, texture_format _format,
						   std::uint32_t _textureFlags)
	: frame_buffer(std::vector<std::shared_ptr<texture>>{
		  std::make_shared<texture>(_width, _height, false, 1, _format, _textureFlags),
	  })
{
}

frame_buffer::frame_buffer(backbuffer_ratio _ratio, texture_format _format, std::uint32_t _textureFlags)
	: frame_buffer(std::vector<std::shared_ptr<texture>>{
		  std::make_shared<texture>(_ratio, false, 1, _format, _textureFlags),
	  })
{
}

frame_buffer::frame_buffer(const std::vector<std::shared_ptr<texture>>& textures)
{
    std::vector<fbo_attachment> texDescs;
	texDescs.reserve(textures.size());
	for(auto& tex : textures)
	{
		fbo_attachment texDesc;
		texDesc.texture = tex;
		texDescs.push_back(texDesc);
	}

	populate(texDescs);
}

frame_buffer::frame_buffer(const std::vector<fbo_attachment>& textures)
{
    populate(textures);
}

frame_buffer::frame_buffer(void *_nwh, uint16_t _width, uint16_t _height, texture_format _depth_format)
{
    handle = create_frame_buffer(_nwh, _width, _height, _depth_format);

	_cached_size = {_width, _height};
	_bbratio = backbuffer_ratio::Count;
}

void frame_buffer::populate(const std::vector<fbo_attachment>& textures)
{
	std::vector<attachment> buffer;
	buffer.reserve(textures.size());

	usize size = {0, 0};
	auto ratio = backbuffer_ratio::Count;
	for(auto& tex : textures)
	{
		ratio = tex.texture->ratio;
		size = {tex.texture->info.width, tex.texture->info.height};
		attachment att;
		att.handle = tex.texture->native_handle();
		att.mip = tex.mip;
		att.layer = tex.layer;
		buffer.push_back(att);
	}
	_textures = textures;

	handle = create_frame_buffer(static_cast<std::uint8_t>(buffer.size()), &buffer[0], false);

	if(ratio == backbuffer_ratio::Count)
	{
		_bbratio = ratio;
		_cached_size = size;
	}
	else
	{
		_bbratio = ratio;
		_cached_size = {0, 0};
	}
}

usize frame_buffer::get_size() const
{
	if(_bbratio == backbuffer_ratio::Count)
	{
		return _cached_size;

	} // End if Absolute
	else
	{
		std::uint16_t width;
		std::uint16_t height;
		gfx::get_size_from_ratio(_bbratio, width, height);
		usize size = {static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)};
		return size;

	} // End if Relative
}

const fbo_attachment& frame_buffer::get_attachment(std::uint32_t index) const
{
    return _textures[index];
}

const std::shared_ptr<texture> &frame_buffer::get_texture(uint32_t index) const
{
    return get_attachment(index).texture;
}

std::size_t frame_buffer::get_attachment_count() const
{
	return _textures.size();
}
}
