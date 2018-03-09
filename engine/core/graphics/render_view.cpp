#include "render_view.h"

namespace gfx
{

std::shared_ptr<texture> render_view::get_texture(const std::string& id, std::uint16_t _width,
												  std::uint16_t _height, bool _hasMips,
												  std::uint16_t _numLayers, texture_format _format,
												  std::uint32_t _flags, const memory_view* _mem)
{
	texture_key key;
	calc_texture_size(key.info, _width, _height, 1, false, _hasMips, _numLayers, _format);

	key.id = id;
	key.flags = _flags;
	key.ratio = backbuffer_ratio::Count;

	std::shared_ptr<texture> tex;
	auto it = textures_.find(key);
	if(it != textures_.end())
	{
		tex = it->second.first;
		it->second.second = true;
	}
	else
	{
		tex = std::make_shared<texture>(_width, _height, _hasMips, _numLayers, _format, _flags, _mem);
		textures_[key] = std::pair<std::shared_ptr<texture>, bool>(tex, true);
	}

	return tex;
}

std::shared_ptr<texture> render_view::get_texture(const std::string& id, backbuffer_ratio _ratio,
												  bool _hasMips, std::uint16_t _numLayers,
												  texture_format _format, std::uint32_t _flags)
{
	texture_key key;
	std::uint16_t _width = 0;
	std::uint16_t _height = 0;
	get_size_from_ratio(_ratio, _width, _height);
	calc_texture_size(key.info, _width, _height, 1, false, _hasMips, _numLayers, _format);

	key.id = id;
	key.flags = _flags;
	key.ratio = _ratio;

	std::shared_ptr<texture> tex;
	auto it = textures_.find(key);
	if(it != textures_.end())
	{
		tex = it->second.first;
		it->second.second = true;
	}
	else
	{
		tex = std::make_shared<texture>(_ratio, _hasMips, _numLayers, _format, _flags);
		textures_[key] = std::pair<std::shared_ptr<texture>, bool>(tex, true);
	}

	return tex;
}

std::shared_ptr<texture> render_view::get_texture(const std::string& id, std::uint16_t _width,
												  std::uint16_t _height, std::uint16_t _depth, bool _hasMips,
												  texture_format _format, std::uint32_t _flags,
												  const memory_view* _mem)
{
	texture_key key;
	calc_texture_size(key.info, _width, _height, _depth, false, _hasMips, 1, _format);

	key.id = id;
	key.flags = _flags;
	key.ratio = backbuffer_ratio::Count;

	std::shared_ptr<texture> tex;
	auto it = textures_.find(key);
	if(it != textures_.end())
	{
		tex = it->second.first;
		it->second.second = true;
	}
	else
	{
		tex = std::make_shared<texture>(_width, _height, _depth, _hasMips, _format, _flags, _mem);
		textures_[key] = std::pair<std::shared_ptr<texture>, bool>(tex, true);
	}

	return tex;
}

std::shared_ptr<texture> render_view::get_texture(const std::string& id, std::uint16_t _size, bool _hasMips,
												  std::uint16_t _numLayers, texture_format _format,
												  std::uint32_t _flags, const memory_view* _mem)
{
	texture_key key;
	calc_texture_size(key.info, _size, _size, _size, false, _hasMips, _numLayers, _format);

	key.id = id;
	key.flags = _flags;
	key.ratio = backbuffer_ratio::Count;

	std::shared_ptr<texture> tex;
	auto it = textures_.find(key);
	if(it != textures_.end())
	{
		tex = it->second.first;
		it->second.second = true;
	}
	else
	{
		tex = std::make_shared<texture>(_size, _hasMips, _numLayers, _format, _flags, _mem);
		textures_[key] = std::pair<std::shared_ptr<texture>, bool>(tex, true);
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
	auto it = fbos_.find(key);
	if(it != fbos_.end())
	{
		tex = it->second.first;
		it->second.second = true;
	}
	else
	{
		tex = std::make_shared<frame_buffer>(bind_textures);
		fbos_[key] = std::pair<std::shared_ptr<frame_buffer>, bool>(tex, true);
	}

	return tex;
}

std::shared_ptr<texture> render_view::get_depth_stencil_buffer(const usize32_t& viewport_size)
{
	static auto format =
		get_best_format(BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
						format_search_flags::requires_depth | format_search_flags::requires_stencil);
	return get_texture("DEPTH_STENCIL", viewport_size.width, viewport_size.height, false, 1, format);
}

std::shared_ptr<texture> render_view::get_depth_buffer(const usize32_t& viewport_size)
{
	static auto format =
		get_best_format(BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER, format_search_flags::requires_depth);
	return get_texture("DEPTH", viewport_size.width, viewport_size.height, false, 1, format);
}
std::shared_ptr<texture> render_view::get_output_buffer(const usize32_t& viewport_size)
{
	static auto format =
		get_best_format(BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
						format_search_flags::four_channels | format_search_flags::requires_alpha);
	return get_texture("OUTPUT", viewport_size.width, viewport_size.height, false, 1, format);
}
std::shared_ptr<frame_buffer> render_view::get_output_fbo(const usize32_t& viewport_size)
{
	return get_fbo("OUTPUT", {get_output_buffer(viewport_size), get_depth_buffer(viewport_size)});
}
std::shared_ptr<frame_buffer> render_view::get_g_buffer_fbo(const usize32_t& viewport_size)
{
	static auto format =
		get_best_format(BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
						format_search_flags::four_channels | format_search_flags::requires_alpha);
	static auto normal_format =
		get_best_format(BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER, format_search_flags::four_channels |
																  format_search_flags::requires_alpha |
																  format_search_flags::half_precision_float);
	auto depth_buffer = get_depth_buffer(viewport_size);
	auto buffer0 = get_texture("GBUFFER0", viewport_size.width, viewport_size.height, false, 1, format);
	auto buffer1 =
		get_texture("GBUFFER1", viewport_size.width, viewport_size.height, false, 1, normal_format);
	auto buffer2 = get_texture("GBUFFER2", viewport_size.width, viewport_size.height, false, 1, format);
	auto buffer3 = get_texture("GBUFFER3", viewport_size.width, viewport_size.height, false, 1, format);
	return get_fbo("GBUFFER", {buffer0, buffer1, buffer2, buffer3, depth_buffer});
}

void render_view::release_unused_resources()
{
	auto check_resources = [](auto& associativie_container) {
		for(auto it = associativie_container.begin(); it != associativie_container.end();)
		{
			auto& item = it->second.first;
			bool& used_last_frame = it->second.second;

			if(!used_last_frame && item.use_count() == 1)
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

	check_resources(fbos_);
	check_resources(textures_);
}
}
