#pragma once

#include <string>
#include <cstdint>
#include "bgfx/bgfx.h"
#include "bgfx/platform.h"
#include "bgfx/embedded_shader.h"

namespace gfx
{

	using namespace bgfx;
	using namespace bx;
	
	struct pos_texcoord0_vertex
	{
		float x, y, z;
		float u, v;

		static void init()
		{
			decl
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();
		}

		static bgfx::VertexDecl decl;
	};

	struct mesh_vertex
	{
		static void init()
		{
			decl
				.begin()
				.add(bgfx::Attrib::Position, 3, bgfx::AttribType::Float)
				.add(bgfx::Attrib::Color1, 4, bgfx::AttribType::Uint8, true)
				.add(bgfx::Attrib::Normal, 4, bgfx::AttribType::Uint8, true, true)
				.add(bgfx::Attrib::Tangent, 4, bgfx::AttribType::Uint8, true, true)
				.add(bgfx::Attrib::Bitangent, 4, bgfx::AttribType::Uint8, true, true)
				.add(bgfx::Attrib::TexCoord0, 2, bgfx::AttribType::Float)
				.end();
		}

		static bgfx::VertexDecl decl;
	};

	inline float get_half_texel()
	{
		const RendererType::Enum renderer = getRendererType();
		float half_texel = RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
		return half_texel;
	}

	inline std::uint32_t get_max_blend_transforms()
	{
		//BGFX_CONFIG_MAX_BONES
		return 128;
	}

	inline bool is_origin_bottom_left()
	{
		return getCaps()->originBottomLeft;
	}

	inline bool is_homogeneous_depth()
	{
		return gfx::getCaps()->homogeneousDepth;
	}

	inline bool is_format_supported(std::uint16_t flags, TextureFormat::Enum format)
	{
		const std::uint32_t formatCaps = getCaps()->formats[format];
		return 0 != (formatCaps & flags);
	}

	namespace format_search_flags
	{
		enum e
		{
			OneChannel = 0x1,
			TwoChannels = 0x2,
			FourChannels = 0x8,
			RequireAlpha = 0x10,
			RequireStencil = 0x20,
			PreferCompressed = 0x40,

			AllowPaddingChannels = 0x100,
			RequireDepth = 0x200,

			HalfPrecisionFloat = 0x1000,
			FullPrecisionFloat = 0x2000,
			FloatingPoint = 0xF000,
		};

	}; // End Namespace : format_search_flags

	TextureFormat::Enum get_best_format(std::uint16_t type, std::uint32_t search_flags);

	inline std::uint32_t get_default_rt_sampler_flags()
	{
		static std::uint32_t sampler_flags = 0
			| BGFX_TEXTURE_RT
			| BGFX_TEXTURE_U_CLAMP
			| BGFX_TEXTURE_V_CLAMP
			;

		return sampler_flags;
	}

	void shutdown();

	bool init(
		RendererType::Enum _type = RendererType::Count
		, uint16_t _vendorId = BGFX_PCI_ID_NONE
		, uint16_t _deviceId = 0
		, CallbackI* _callback = NULL
		, bx::AllocatorI* _reallocator = NULL
	);

	bool is_initted();
	std::uint64_t screen_quad(float dest_width, float dest_height, float depth = 0.0f, float width = 1.0f, float height = 1.0f);
	std::uint64_t clip_quad(float depth = 0.0f, float width = 1.0f, float height = 1.0f);

	void get_size_from_ratio(BackbufferRatio::Enum _ratio, uint16_t& _width, uint16_t& _height);
	const std::string& get_renderer_filename_extension();
}