#include "render_view_keys.h"

namespace gfx
{
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
	if(!result)
		return false;

	std::size_t tex_count = key1.textures.size();
	for(std::size_t i = 0; i < tex_count; ++i)
	{
		if(key1.textures[i] != key2.textures[i])
			return false;
	}

	// Identical
	return true;
}
}
