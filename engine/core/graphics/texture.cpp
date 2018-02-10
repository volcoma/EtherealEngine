#include "texture.h"

namespace gfx
{

texture::texture(const memory_view* _mem, std::uint32_t _flags /*= BGFX_TEXTURE_NONE */,
				 std::uint8_t _skip /*= 0 */, texture_info* _info /*= nullptr*/)
{
	texture_info* pInfo = _info;
	if(pInfo == nullptr)
	{
		pInfo = &info;
	}

	handle = create_texture(_mem, _flags, _skip, pInfo);

	if(pInfo != nullptr)
	{
		info = *pInfo;
	}

	flags = _flags;
	ratio = backbuffer_ratio::Count;
}

texture::texture(std::uint16_t _width, std::uint16_t _height, bool _hasMips, std::uint16_t _numLayers,
				 texture_format _format, std::uint32_t _flags /*= BGFX_TEXTURE_NONE */,
				 const memory_view* _mem /*= nullptr */)
{
	handle = create_texture_2d(_width, _height, _hasMips, _numLayers, _format, _flags, _mem);

	calc_texture_size(info, _width, _height, 1, false, _hasMips, _numLayers, _format);

	flags = _flags;
	ratio = backbuffer_ratio::Count;
}

texture::texture(std::uint16_t _width, std::uint16_t _height, std::uint16_t _depth, bool _hasMips,
				 texture_format _format, std::uint32_t _flags /*= BGFX_TEXTURE_NONE */,
				 const memory_view* _mem /*= nullptr */)
{
	handle = create_texture_3d(_width, _height, _depth, _hasMips, _format, _flags, _mem);

	calc_texture_size(info, _width, _height, _depth, false, _hasMips, 1, _format);

	flags = _flags;
	ratio = backbuffer_ratio::Count;
}

texture::texture(std::uint16_t _size, bool _hasMips, std::uint16_t _numLayers, texture_format _format,
				 std::uint32_t _flags /*= BGFX_TEXTURE_NONE */, const memory_view* _mem /*= nullptr */)
{
	handle = create_texture_cube(_size, _hasMips, _numLayers, _format, _flags, _mem);

	calc_texture_size(info, _size, _size, _size, false, _hasMips, _numLayers, _format);

	flags = _flags;
	ratio = backbuffer_ratio::Count;
}

texture::texture(backbuffer_ratio _ratio, bool _hasMips, std::uint16_t _numLayers, texture_format _format,
				 std::uint32_t _flags /*= BGFX_TEXTURE_NONE */)
{
	handle = create_texture_2d(_ratio, _hasMips, _numLayers, _format, _flags);

	std::uint16_t _width = 0;
	std::uint16_t _height = 0;
	get_size_from_ratio(_ratio, _width, _height);
	calc_texture_size(info, _width, _height, 1, false, _hasMips, _numLayers, _format);

	flags = _flags;
	ratio = _ratio;
}

usize32_t texture::get_size() const
{
	if(ratio == backbuffer_ratio::Count)
	{
		usize32_t size = {static_cast<std::uint32_t>(info.width), static_cast<std::uint32_t>(info.height)};
		return size;

	} // End if Absolute

	std::uint16_t width;
	std::uint16_t height;
	get_size_from_ratio(ratio, width, height);
	usize32_t size = {static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)};
	return size;
	// End if Relative
}

bool texture::is_render_target() const
{
	return 0 != (flags & BGFX_TEXTURE_RT_MASK);
}
}
