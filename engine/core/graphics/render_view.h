#pragma once

#include "../common/hash.hpp"
#include "../common/basetypes.hpp"
#include "frame_buffer.h"
#include <chrono>
#include <functional>
#include <memory>
#include <unordered_map>

struct texture_key
{
	/// User id
	std::string id;
	/// Texture detail info.
	gfx::TextureInfo info;
	/// Creation flags.
	std::uint32_t flags = BGFX_TEXTURE_NONE;
	/// Back buffer ratio if any.
	gfx::BackbufferRatio::Enum ratio = gfx::BackbufferRatio::Count;
};

struct fbo_key
{
	/// User id
	std::string id;

	std::vector<std::shared_ptr<texture>> textures;
};

bool operator==(const texture_key& key1, const texture_key& key2);
bool operator==(const fbo_key& key1, const fbo_key& key2);

namespace std
{
template <>
struct hash<texture_key>
{
	typedef texture_key argument_type;
	typedef std::size_t result_type;
	result_type operator()(argument_type const& s) const
	{
		size_t seed = 0;
		utils::hash_combine(seed, s.id);
		utils::hash_combine(seed, static_cast<int>(s.info.format));
		utils::hash_combine(seed, s.info.storageSize);
		utils::hash_combine(seed, s.info.width);
		utils::hash_combine(seed, s.info.height);
		utils::hash_combine(seed, s.info.depth);
		utils::hash_combine(seed, s.info.numLayers);
		utils::hash_combine(seed, s.info.numMips);
		utils::hash_combine(seed, s.info.bitsPerPixel);
		utils::hash_combine(seed, s.info.cubeMap);
		utils::hash_combine(seed, s.flags);
		utils::hash_combine(seed, static_cast<int>(s.ratio));
		return seed;
	}
};

template <>
struct hash<fbo_key>
{
	typedef fbo_key argument_type;
	typedef std::size_t result_type;
	result_type operator()(argument_type const& s) const
	{
		size_t seed = 0;
		utils::hash_combine(seed, s.id);
		for(auto& tex : s.textures)
		{
			utils::hash_combine(seed, tex);
		}

		return seed;
	}
};
}
class render_view
{
public:
	std::shared_ptr<texture> get_texture(const std::string& id, std::uint16_t _width, std::uint16_t _height,
		bool _hasMips, std::uint16_t _numLayers,
		gfx::TextureFormat::Enum _format,
		std::uint32_t _flags = gfx::get_default_rt_sampler_flags(),
		const gfx::Memory* _mem = nullptr);

	std::shared_ptr<texture> get_texture(const std::string& id, gfx::BackbufferRatio::Enum _ratio,
		bool _hasMips, std::uint16_t _numLayers,
		gfx::TextureFormat::Enum _format,
		std::uint32_t _flags = gfx::get_default_rt_sampler_flags());

	std::shared_ptr<texture> get_texture(const std::string& id, std::uint16_t _width, std::uint16_t _height,
		std::uint16_t _depth, bool _hasMips,
		gfx::TextureFormat::Enum _format,
		std::uint32_t _flags = gfx::get_default_rt_sampler_flags(),
		const gfx::Memory* _mem = nullptr);

	std::shared_ptr<texture> get_texture(const std::string& id, std::uint16_t _size, bool _hasMips,
		std::uint16_t _numLayers, gfx::TextureFormat::Enum _format,
		std::uint32_t _flags = gfx::get_default_rt_sampler_flags(),
		const gfx::Memory* _mem = nullptr);

	std::shared_ptr<frame_buffer> get_fbo(const std::string& id,
		const std::vector<std::shared_ptr<texture>>& bind_textures);

	std::shared_ptr<texture> get_depth_stencil_buffer(const usize& viewport_size);
	std::shared_ptr<texture> get_output_buffer(const usize& viewport_size);
	std::shared_ptr<frame_buffer> get_output_fbo(const usize& viewport_size);
	std::shared_ptr<frame_buffer> get_g_buffer_fbo(const usize& viewport_size);

	void release_unused_resources();

private:
	std::unordered_map<texture_key, std::pair<std::shared_ptr<texture>, bool>> _textures;
	std::unordered_map<fbo_key, std::pair<std::shared_ptr<frame_buffer>, bool>> _fbos;
};
