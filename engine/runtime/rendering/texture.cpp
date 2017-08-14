#include "texture.h"

texture::~texture()
{
	dispose();
}

bool texture::is_valid() const
{
	return gfx::isValid(handle);
}

void texture::dispose()
{
	if(is_valid())
		gfx::destroy(handle);

	handle = {bgfx::kInvalidHandle};
}

void texture::populate(const gfx::Memory* _mem, std::uint32_t _flags /*= BGFX_TEXTURE_NONE */,
					   std::uint8_t _skip /*= 0 */, gfx::TextureInfo* _info /*= nullptr*/)
{
	dispose();

	gfx::TextureInfo* pInfo = _info;
	if(!pInfo)
		pInfo = &info;

	handle = gfx::createTexture(_mem, _flags, _skip, pInfo);

	if(pInfo)
		info = *pInfo;

	flags = _flags;
	ratio = gfx::BackbufferRatio::Count;
}

void texture::populate(std::uint16_t _width, std::uint16_t _height, bool _hasMips, std::uint16_t _numLayers,
					   gfx::TextureFormat::Enum _format, std::uint32_t _flags /*= BGFX_TEXTURE_NONE */,
					   const gfx::Memory* _mem /*= nullptr */)
{
	dispose();

	handle = gfx::createTexture2D(_width, _height, _hasMips, _numLayers, _format, _flags, _mem);

	gfx::calcTextureSize(info, _width, _height, 1, false, _hasMips, _numLayers, _format);

	flags = _flags;
	ratio = gfx::BackbufferRatio::Count;
}

void texture::populate(std::uint16_t _width, std::uint16_t _height, std::uint16_t _depth, bool _hasMips,
					   gfx::TextureFormat::Enum _format, std::uint32_t _flags /*= BGFX_TEXTURE_NONE */,
					   const gfx::Memory* _mem /*= nullptr */)
{
	dispose();

	handle = gfx::createTexture3D(_width, _height, _depth, _hasMips, _format, _flags, _mem);

	gfx::calcTextureSize(info, _width, _height, _depth, false, _hasMips, 1, _format);

	flags = _flags;
	ratio = gfx::BackbufferRatio::Count;
}

void texture::populate(std::uint16_t _size, bool _hasMips, std::uint16_t _numLayers,
					   gfx::TextureFormat::Enum _format, std::uint32_t _flags /*= BGFX_TEXTURE_NONE */,
					   const gfx::Memory* _mem /*= nullptr */)
{
	dispose();

	handle = gfx::createTextureCube(_size, _hasMips, _numLayers, _format, _flags, _mem);

	gfx::calcTextureSize(info, _size, _size, _size, false, _hasMips, _numLayers, _format);

	flags = _flags;
	ratio = gfx::BackbufferRatio::Count;
}

void texture::populate(gfx::BackbufferRatio::Enum _ratio, bool _hasMips, std::uint16_t _numLayers,
					   gfx::TextureFormat::Enum _format, std::uint32_t _flags /*= BGFX_TEXTURE_NONE */)
{
	dispose();

	handle = gfx::createTexture2D(_ratio, _hasMips, _numLayers, _format, _flags);

	std::uint16_t _width = 0;
	std::uint16_t _height = 0;
	gfx::get_size_from_ratio(_ratio, _width, _height);
	gfx::calcTextureSize(info, _width, _height, 1, false, _hasMips, _numLayers, _format);

	flags = _flags;
	ratio = _ratio;
}

usize texture::get_size() const
{
	if(ratio == gfx::BackbufferRatio::Count)
	{
		usize size = {static_cast<std::uint32_t>(info.width), static_cast<std::uint32_t>(info.height)};
		return size;

	} // End if Absolute
	else
	{
		std::uint16_t width;
		std::uint16_t height;
		gfx::get_size_from_ratio(ratio, width, height);
		usize size = {static_cast<std::uint32_t>(width), static_cast<std::uint32_t>(height)};
		return size;

	} // End if Relative
}
