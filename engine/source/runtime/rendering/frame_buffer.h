#pragma once

#include "texture.h"
#include "graphics/graphics.h"

struct TextureAttachment
{
	/// Texture handle.
	std::shared_ptr<Texture> texture;
	/// Mip level.
	uint16_t mip = 0;
	/// Cubemap side or depth layer/slice.
	uint16_t layer = 0;
};

struct FrameBuffer
{
	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer() = default;

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer(
		std::uint16_t _width
		, std::uint16_t _height
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
	)
	{
		populate(_width, _height, _format, _textureFlags);
	}

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer(
		gfx::BackbufferRatio::Enum _ratio
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
	)
	{
		populate(_ratio, _format, _textureFlags);
	}

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer(const std::vector<std::shared_ptr<Texture>>& textures)
	{
		populate(textures);
	}

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer(const std::vector<TextureAttachment>& textures)
	{
		populate(textures);
	}

	//-----------------------------------------------------------------------------
	//  Name : FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	FrameBuffer(
		void* _nwh
		, std::uint16_t _width
		, std::uint16_t _height
		, gfx::TextureFormat::Enum _depthFormat = gfx::TextureFormat::UnknownDepth
	)
	{
		populate(_nwh, _width, _height, _depthFormat);
	}
	//-----------------------------------------------------------------------------
	//  Name : ~FrameBuffer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~FrameBuffer()
	{
		dispose();
	}

	//-----------------------------------------------------------------------------
	//  Name : is_valid ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool is_valid() const
	{
		return gfx::isValid(handle);
	}

	//-----------------------------------------------------------------------------
	//  Name : dispose ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void dispose()
	{
		_textures.clear();

		if (is_valid())
			gfx::destroyFrameBuffer(handle);

		handle = { bgfx::invalidHandle };
	}

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		std::uint16_t _width
		, std::uint16_t _height
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
	)
	{
		populate(std::vector<std::shared_ptr<Texture>>
		{
			std::make_shared<Texture>(_width, _height, false, 1, _format, _textureFlags),
		});
	}

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		gfx::BackbufferRatio::Enum _ratio
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _textureFlags = BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP
	)
	{
		populate(std::vector<std::shared_ptr<Texture>>
		{
			std::make_shared<Texture>(_ratio, false, 1, _format, _textureFlags),
		});
	}

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(const std::vector<std::shared_ptr<Texture>>& textures)
	{
		dispose();

		std::vector<TextureAttachment> texDescs;
		texDescs.reserve(textures.size());
		for (auto& tex : textures)
		{
			TextureAttachment texDesc;
			texDesc.texture = tex;
			texDescs.push_back(texDesc);
		}

		populate(texDescs);
	}

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(const std::vector<TextureAttachment>& textures)
	{

		dispose();

		std::vector<gfx::Attachment> buffer;
		buffer.reserve(textures.size());

		uSize size;
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

	//-----------------------------------------------------------------------------
	//  Name : populate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void populate(
		void* _nwh
		, std::uint16_t _width
		, std::uint16_t _height
		, gfx::TextureFormat::Enum _depthFormat = gfx::TextureFormat::UnknownDepth
	)
	{
		dispose();

		handle = gfx::createFrameBuffer(_nwh, _width, _height, _depthFormat);

		_cached_size = { _width, _height };
		_bbratio = gfx::BackbufferRatio::Count;
	}


	//-----------------------------------------------------------------------------
	//  Name : get_size ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	uSize get_size() const
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
			uSize size =
			{
				static_cast<std::uint32_t>(width),
				static_cast<std::uint32_t>(height)
			};
			return size;

		} // End if Relative
	}

	//-----------------------------------------------------------------------------
	//  Name : get_attachment ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const TextureAttachment& get_attachment(std::uint32_t index) const { return _textures[index]; }

	//-----------------------------------------------------------------------------
	//  Name : get_attachment_count ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const std::size_t get_attachment_count() const { return _textures.size(); }

	/// Internal handle
	gfx::FrameBufferHandle handle = { gfx::invalidHandle };
	/// Back buffer ratio if any.
	gfx::BackbufferRatio::Enum _bbratio = gfx::BackbufferRatio::Equal;
	/// Size of the surface. If {0,0} then it is controlled by backbuffer ratio
	uSize _cached_size = { 0, 0 };
	/// Texture attachments to the frame buffer
	std::vector<TextureAttachment> _textures;
};


#include <functional>
#include <chrono>
#include "core/common/utils.h"

struct TextureKey
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

struct FboKey
{
	/// User id
	std::string id;

	std::vector<std::shared_ptr<Texture>> textures;
	
};


inline bool operator==(const TextureKey& key1, const TextureKey& key2)
{
	return key1.id == key2.id &&
		key1.flags == key2.flags &&
		key1.ratio == key2.ratio &&
		key1.info.format == key2.info.format &&
		key1.info.storageSize == key2.info.storageSize &&
		key1.info.width == key2.info.width &&
		key1.info.height == key2.info.height &&
		key1.info.depth == key2.info.depth &&
		key1.info.numMips == key2.info.numMips &&
		key1.info.numLayers == key2.info.numLayers &&
		key1.info.bitsPerPixel == key2.info.bitsPerPixel &&
		key1.info.cubeMap == key2.info.cubeMap;
}

inline bool operator==(const FboKey& key1, const FboKey& key2)
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
namespace std
{
	template<> struct hash<TextureKey>
	{
		typedef TextureKey argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const
		{
			size_t seed = 0;
			utils::hash_combine(seed, s.id);
			utils::hash_combine(seed, s.info.format);
			utils::hash_combine(seed, s.info.storageSize);
			utils::hash_combine(seed, s.info.width);
			utils::hash_combine(seed, s.info.height);
			utils::hash_combine(seed, s.info.depth);
			utils::hash_combine(seed, s.info.numLayers);
			utils::hash_combine(seed, s.info.numMips);
			utils::hash_combine(seed, s.info.bitsPerPixel);
			utils::hash_combine(seed, s.info.cubeMap);
			utils::hash_combine(seed, s.flags);
			utils::hash_combine(seed, s.ratio);
			return seed;
		}
	};
}

namespace std
{
	template<> struct hash<FboKey>
	{
		typedef FboKey argument_type;
		typedef std::size_t result_type;
		result_type operator()(argument_type const& s) const
		{
			size_t seed = 0;
			utils::hash_combine(seed, s.id);
			for (auto& tex : s.textures)
			{
				utils::hash_combine(seed, tex);
			}
			
			return seed;
		}
	};
}
class RenderView
{
public:
	std::shared_ptr<Texture> get_texture(
		const std::string& id
		, std::uint16_t _width
		, std::uint16_t _height
		, bool _hasMips
		, std::uint16_t _numLayers
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = gfx::get_default_rt_sampler_flags()
		, const gfx::Memory* _mem = nullptr
	)
	{
		TextureKey key;
		gfx::calcTextureSize(key.info
			, _width
			, _height
			, 1
			, false
			, _hasMips
			, _numLayers
			, _format
		);

		key.id = id;
		key.flags = _flags;
		key.ratio = gfx::BackbufferRatio::Count;

		std::shared_ptr<Texture> tex;
		auto it = _textures.find(key);
		if (it != _textures.end())
		{
			tex = it->second.first;
			it->second.second = true;
		}
		else
		{
			tex = std::make_shared<Texture>(_width, _height, _hasMips, _numLayers, _format, _flags, _mem);
			_textures[key] = std::pair<std::shared_ptr<Texture>, bool>(tex, true);
		}

		return tex;
	}

	std::shared_ptr<Texture> get_texture(
		const std::string& id
		, gfx::BackbufferRatio::Enum _ratio
		, bool _hasMips
		, std::uint16_t _numLayers
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = gfx::get_default_rt_sampler_flags()
	)
	{
		TextureKey key;
		std::uint16_t _width = 0;
		std::uint16_t _height = 0;
		gfx::get_size_from_ratio(_ratio, _width, _height);
		gfx::calcTextureSize(key.info
			, _width
			, _height
			, 1
			, false
			, _hasMips
			, _numLayers
			, _format
		);

		key.id = id;
		key.flags = _flags;
		key.ratio = _ratio;

		std::shared_ptr<Texture> tex;
		auto it = _textures.find(key);
		if (it != _textures.end())
		{
			tex = it->second.first;
			it->second.second = true;
		}
		else
		{
			tex = std::make_shared<Texture>(_ratio, _hasMips, _numLayers, _format, _flags);
			_textures[key] = std::pair<std::shared_ptr<Texture>, bool>(tex, true);
		}

		return tex;
	}

	std::shared_ptr<Texture> get_texture(
		const std::string& id
		, std::uint16_t _width
		, std::uint16_t _height
		, std::uint16_t _depth
		, bool _hasMips
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = gfx::get_default_rt_sampler_flags()
		, const gfx::Memory* _mem = nullptr
	)
	{
		TextureKey key;
		gfx::calcTextureSize(key.info
			, _width
			, _height
			, _depth
			, false
			, _hasMips
			, 1
			, _format
		);

		key.id = id;
		key.flags = _flags;
		key.ratio = gfx::BackbufferRatio::Count;

		std::shared_ptr<Texture> tex;
		auto it = _textures.find(key);
		if (it != _textures.end())
		{
			tex = it->second.first;
			it->second.second = true;
		}
		else
		{
			tex = std::make_shared<Texture>(_width, _height, _depth, _hasMips, _format, _flags, _mem);
			_textures[key] = std::pair<std::shared_ptr<Texture>, bool>(tex, true);
		}

		return tex;
	}

	std::shared_ptr<Texture> get_texture(
		const std::string& id
		, std::uint16_t _size
		, bool _hasMips
		, std::uint16_t _numLayers
		, gfx::TextureFormat::Enum _format
		, std::uint32_t _flags = gfx::get_default_rt_sampler_flags()
		, const gfx::Memory* _mem = nullptr
	)
	{
		TextureKey key;
		gfx::calcTextureSize(key.info
			, _size
			, _size
			, _size
			, false
			, _hasMips
			, _numLayers
			, _format
		);

		key.id = id;
		key.flags = _flags;
		key.ratio = gfx::BackbufferRatio::Count;

		std::shared_ptr<Texture> tex;
		auto it = _textures.find(key);
		if (it != _textures.end())
		{
			tex = it->second.first;
			it->second.second = true;
		}
		else
		{
			tex = std::make_shared<Texture>(_size, _hasMips, _numLayers, _format, _flags, _mem);
			_textures[key] = std::pair<std::shared_ptr<Texture>, bool>(tex, true);
		}

		return tex;
	}
	
	std::shared_ptr<FrameBuffer> get_fbo(
		const std::string& id,
		const std::vector<std::shared_ptr<Texture>>& bind_textures)
	{
		FboKey key;
		key.id = id;
		key.textures = bind_textures;
		std::shared_ptr<FrameBuffer> tex;
		auto it = _fbos.find(key);
		if (it != _fbos.end())
		{
			tex = it->second.first;
			it->second.second = true;
		}
		else
		{
			tex = std::make_shared<FrameBuffer>(bind_textures);
			_fbos[key] = std::pair<std::shared_ptr<FrameBuffer>, bool>(tex, true);
		}

		return tex;
	}

	std::shared_ptr<Texture> get_depth_stencil_buffer(const uSize& viewport_size)
	{
		static auto format = gfx::get_best_format(
			BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
			gfx::FormatSearchFlags::RequireDepth |
			gfx::FormatSearchFlags::RequireStencil);
		return get_texture("DEPTH", viewport_size.width, viewport_size.height, false, 1, format);
	}
	std::shared_ptr<Texture> get_output_buffer(const uSize& viewport_size)
	{
		static auto format = gfx::get_best_format(
			BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
			gfx::FormatSearchFlags::FourChannels |
			gfx::FormatSearchFlags::RequireAlpha);
		return get_texture("OUTPUT", viewport_size.width, viewport_size.height, false, 1, format);
	}
	std::shared_ptr<FrameBuffer> get_output_fbo(const uSize& viewport_size)
	{
		return get_fbo("OUTPUT", { get_output_buffer(viewport_size), get_depth_stencil_buffer(viewport_size) });
	}
	std::shared_ptr<FrameBuffer> get_g_buffer_fbo(const uSize& viewport_size)
	{
		static auto format = gfx::get_best_format(
			BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
			gfx::FormatSearchFlags::FourChannels |
			gfx::FormatSearchFlags::RequireAlpha);
		static auto normal_format = gfx::get_best_format(
			BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER,
			gfx::FormatSearchFlags::FourChannels |
			gfx::FormatSearchFlags::RequireAlpha |
			gfx::FormatSearchFlags::HalfPrecisionFloat);
		auto depth_buffer = get_depth_stencil_buffer(viewport_size);
		auto buffer0 = get_texture("GBUFFER0", viewport_size.width, viewport_size.height, false, 1, format);
		auto buffer1 = get_texture("GBUFFER1", viewport_size.width, viewport_size.height, false, 1, normal_format);
		auto buffer2 = get_texture("GBUFFER2", viewport_size.width, viewport_size.height, false, 1, format);
		auto buffer3 = get_texture("GBUFFER3", viewport_size.width, viewport_size.height, false, 1, format);
		return get_fbo("GBUFFER", { buffer0, buffer1, buffer2, buffer3, depth_buffer });
	}

	void release_unused_resources()
	{
		auto check_resources = [](auto& associativie_container)
		{
			auto it = associativie_container.begin();
			for (; it != associativie_container.end();)
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
					// Increment iterator in case the current entry gets destroyed.
					it++;
				}

				used_last_frame = false;
			} // End if destroy delay time has passed
		};
		
		check_resources(_fbos);
		check_resources(_textures);
	}

private:
	std::unordered_map<TextureKey, std::pair<std::shared_ptr<Texture>, bool>> _textures;
	std::unordered_map<FboKey, std::pair<std::shared_ptr<FrameBuffer>, bool>> _fbos;
};