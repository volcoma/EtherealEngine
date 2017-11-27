#include "render_view.h"

bool operator==(const texture_key& key1, const texture_key& key2)
{
	return key1.id == key2.id && key1.flags == key2.flags && key1.ratio == key2.ratio &&
		key1.info.format == key2.info.format && key1.info.storageSize == key2.info.storageSize &&
		key1.info.width == key2.info.width && key1.info.height == key2.info.height &&
		key1.info.depth == key2.info.depth && key1.info.numMips == key2.info.numMips &&
		key1.info.numLayers == key2.info.numLayers && key1.info.bitsPerPixel == key2.info.bitsPerPixel &&
		key1.info.cubeMap == key2.info.cubeMap;
}

bool operator==(const fbo_key& key1, const fbo_key& key2)
{
	bool result = key1.id == key2.id && key1.textures.size() == key2.textures.size();
	if (!result)
		return false;

	std::size_t tex_count = key1.textures.size();
	for (std::size_t i = 0; i < tex_count; ++i)
	{
		if (key1.textures[i] != key2.textures[i])
			return false;
	}

	// Identical
	return true;
}

std::shared_ptr<texture> render_view::get_texture(const std::string& id, std::uint16_t _width, std::uint16_t _height,
	bool _hasMips, std::uint16_t _numLayers,
	gfx::TextureFormat::Enum _format,
	std::uint32_t _flags,
	const gfx::Memory* _mem)
{
	texture_key key;
	gfx::calcTextureSize(key.info, _width, _height, 1, false, _hasMips, _numLayers, _format);

	key.id = id;
	key.flags = _flags;
	key.ratio = gfx::BackbufferRatio::Count;

	std::shared_ptr<texture> tex;
	auto it = _textures.find(key);
	if (it != _textures.end())
	{
		tex = it->second.first;
		it->second.second = true;
	}
	else
	{
		tex = std::make_shared<texture>(_width, _height, _hasMips, _numLayers, _format, _flags, _mem);
		_textures[key] = std::pair<std::shared_ptr<texture>, bool>(tex, true);
	}

	return tex;
}

std::shared_ptr<texture> render_view::get_texture(const std::string& id, gfx::BackbufferRatio::Enum _ratio,
	bool _hasMips, std::uint16_t _numLayers,
	gfx::TextureFormat::Enum _format,
	std::uint32_t _flags)
{
	texture_key key;
	std::uint16_t _width = 0;
	std::uint16_t _height = 0;
	gfx::get_size_from_ratio(_ratio, _width, _height);
	gfx::calcTextureSize(key.info, _width, _height, 1, false, _hasMips, _numLayers, _format);

	key.id = id;
	key.flags = _flags;
	key.ratio = _ratio;

	std::shared_ptr<texture> tex;
	auto it = _textures.find(key);
	if (it != _textures.end())
	{
		tex = it->second.first;
		it->second.second = true;
	}
	else
	{
		tex = std::make_shared<texture>(_ratio, _hasMips, _numLayers, _format, _flags);
		_textures[key] = std::pair<std::shared_ptr<texture>, bool>(tex, true);
	}

	return tex;
}

std::shared_ptr<texture> render_view::get_texture(const std::string& id, std::uint16_t _width, std::uint16_t _height,
	std::uint16_t _depth, bool _hasMips,
	gfx::TextureFormat::Enum _format,
	std::uint32_t _flags,
	const gfx::Memory* _mem)
{
	texture_key key;
	gfx::calcTextureSize(key.info, _width, _height, _depth, false, _hasMips, 1, _format);

	key.id = id;
	key.flags = _flags;
	key.ratio = gfx::BackbufferRatio::Count;

	std::shared_ptr<texture> tex;
	auto it = _textures.find(key);
	if (it != _textures.end())
	{
		tex = it->second.first;
		it->second.second = true;
	}
	else
	{
		tex = std::make_shared<texture>(_width, _height, _depth, _hasMips, _format, _flags, _mem);
		_textures[key] = std::pair<std::shared_ptr<texture>, bool>(tex, true);
	}

	return tex;
}

std::shared_ptr<texture> render_view::get_texture(const std::string& id, std::uint16_t _size, bool _hasMips,
	std::uint16_t _numLayers, gfx::TextureFormat::Enum _format,
	std::uint32_t _flags,
	const gfx::Memory* _mem)
{
	texture_key key;
	gfx::calcTextureSize(key.info, _size, _size, _size, false, _hasMips, _numLayers, _format);

	key.id = id;
	key.flags = _flags;
	key.ratio = gfx::BackbufferRatio::Count;

	std::shared_ptr<texture> tex;
	auto it = _textures.find(key);
	if (it != _textures.end())
	{
		tex = it->second.first;
		it->second.second = true;
	}
	else
	{
		tex = std::make_shared<texture>(_size, _hasMips, _numLayers, _format, _flags, _mem);
		_textures[key] = std::pair<std::shared_ptr<texture>, bool>(tex, true);
	}

	return tex;
}

std::shared_ptr<frame_buffer> render_view::get_fbo(const std::string& id,
	const std::vector<std::shared_ptr<texture>>& bind_textures)
{
	fbo_key key;
	key.id = id;
	key.textures = bind_textures;
	std::shared_ptr<frame_buffer> tex;
	auto it = _fbos.find(key);
	if (it != _fbos.end())
	{
		tex = it->second.first;
		it->second.second = true;
	}
	else
	{
		tex = std::make_shared<frame_buffer>(bind_textures);
		_fbos[key] = std::pair<std::shared_ptr<frame_buffer>, bool>(tex, true);
	}

	return tex;
}

std::shared_ptr<texture> render_view::get_depth_stencil_buffer(const usize& viewport_size)
{
	static auto format = gfx::get_best_format(BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
		gfx::format_search_flags::requires_depth |
		gfx::format_search_flags::requires_stencil);
	return get_texture("DEPTH", viewport_size.width, viewport_size.height, false, 1, format);
}
std::shared_ptr<texture> render_view::get_output_buffer(const usize& viewport_size)
{
	static auto format = gfx::get_best_format(BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
		gfx::format_search_flags::four_channels |
		gfx::format_search_flags::requires_alpha);
	return get_texture("OUTPUT", viewport_size.width, viewport_size.height, false, 1, format);
}
std::shared_ptr<frame_buffer> render_view::get_output_fbo(const usize& viewport_size)
{
	return get_fbo("OUTPUT", { get_output_buffer(viewport_size), get_depth_stencil_buffer(viewport_size) });
}
std::shared_ptr<frame_buffer> render_view::get_g_buffer_fbo(const usize& viewport_size)
{
	static auto format = gfx::get_best_format(BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
		gfx::format_search_flags::four_channels |
		gfx::format_search_flags::requires_alpha);
	static auto normal_format = gfx::get_best_format(BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
		gfx::format_search_flags::four_channels |
		gfx::format_search_flags::requires_alpha |
		gfx::format_search_flags::half_precision_float);
	auto depth_buffer = get_depth_stencil_buffer(viewport_size);
	auto buffer0 = get_texture("GBUFFER0", viewport_size.width, viewport_size.height, false, 1, format);
	auto buffer1 =
		get_texture("GBUFFER1", viewport_size.width, viewport_size.height, false, 1, normal_format);
	auto buffer2 = get_texture("GBUFFER2", viewport_size.width, viewport_size.height, false, 1, format);
	auto buffer3 = get_texture("GBUFFER3", viewport_size.width, viewport_size.height, false, 1, format);
	return get_fbo("GBUFFER", { buffer0, buffer1, buffer2, buffer3, depth_buffer });
}

void render_view::release_unused_resources()
{
	auto check_resources = [](auto& associativie_container) {
		for (auto it = associativie_container.begin(); it != associativie_container.end();)
		{
			auto& item = it->second.first;
			bool& used_last_frame = it->second.second;

			if (!used_last_frame && item.use_count() == 1)
			{
				// Erase from the management list. This will automatically
				// close the associated render target handle owned by this view.
				it = associativie_container.erase(it);
			} // End if expired
			else
			{
				used_last_frame = false;
				// Increment iterator in case the current entry gets destroyed.
				it++;
			}
		} // End if destroy delay time has passed
	};

	check_resources(_fbos);
	check_resources(_textures);
}