#pragma once

#include "../common/hash.hpp"
#include "texture.h"
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace gfx
{
struct texture_key
{
	/// User id
	std::string id;
	/// Texture detail info.
	texture_info info;
	/// Creation flags.
	std::uint64_t flags = BGFX_TEXTURE_NONE;
	/// Back buffer ratio if any.
	backbuffer_ratio ratio = backbuffer_ratio::Count;
};

struct fbo_key
{
	/// User id
	std::string id;
	///
	std::vector<std::shared_ptr<texture>> textures;
};

bool operator==(const texture_key& key1, const texture_key& key2);
bool operator==(const fbo_key& key1, const fbo_key& key2);
}

namespace std
{
template <>
struct hash<gfx::texture_key>
{
	using argument_type = gfx::texture_key;
	using result_type = std::size_t;
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
struct hash<gfx::fbo_key>
{
	using argument_type = gfx::fbo_key;
	using result_type = std::size_t;
	result_type operator()(argument_type const& s) const
	{
		std::size_t seed = 0;
		utils::hash_combine(seed, s.id);
		for(auto& tex : s.textures)
		{
			utils::hash_combine(seed, tex);
		}

		return seed;
	}
};
}
