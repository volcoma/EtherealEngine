#pragma once

#include "bgfx/bgfx.h"
#include "bgfx/embedded_shader.h"
#include "bgfx/platform.h"
#include <cstdint>
#include <string>
#include <functional>
namespace gfx
{

using namespace bgfx;
using namespace bx;

void set_info_logger(std::function<void(const std::string& log_msg)> logger);
void set_warning_logger(std::function<void(const std::string& log_msg)> logger);
void set_error_logger(std::function<void(const std::string& log_msg)> logger);

template<typename T>
struct vertex
{
	static VertexDecl get_decl()
	{
		static VertexDecl s_decl = []()
		{
			VertexDecl decl;
			T::init(decl);
			return decl;
		}();
		return s_decl;
	}
};

struct pos_texcoord0_vertex : vertex<pos_texcoord0_vertex>
{
	float x, y, z;
	float u, v;

	static void init(VertexDecl& decl)
	{
		decl.begin()
			.add(Attrib::Position, 3, AttribType::Float)
			.add(Attrib::TexCoord0, 2, AttribType::Float)
			.end();
	}
};

struct mesh_vertex : vertex<mesh_vertex>
{
	static void init(VertexDecl& decl)
	{
		decl.begin()
			.add(Attrib::Position, 3, AttribType::Float)
			.add(Attrib::Color1, 4, AttribType::Uint8, true)
			.add(Attrib::Normal, 4, AttribType::Uint8, true, true)
			.add(Attrib::Tangent, 4, AttribType::Uint8, true, true)
			.add(Attrib::Bitangent, 4, AttribType::Uint8, true, true)
			.add(Attrib::TexCoord0, 2, AttribType::Float)
			.end();
	}
};

inline float get_half_texel()
{
	const RendererType::Enum renderer = getRendererType();
	float half_texel = RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
	return half_texel;
}

inline std::uint32_t get_max_blend_transforms()
{
	return 128;
}

inline bool is_origin_bottom_left()
{
	return getCaps()->originBottomLeft;
}

inline bool is_homogeneous_depth()
{
	return getCaps()->homogeneousDepth;
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
	one_channel = 0x1,
	two_channels = 0x2,
	four_channels = 0x8,
	requires_alpha = 0x10,
	requires_stencil = 0x20,
	prefer_compressed = 0x40,

	allow_padding_channels = 0x100,
	requires_depth = 0x200,

	half_precision_float = 0x1000,
	full_precision_float = 0x2000,
	floating_point = 0xF000,
};

}; // End Namespace : format_search_flags

TextureFormat::Enum get_best_format(std::uint16_t type, std::uint32_t search_flags);

inline std::uint32_t get_default_rt_sampler_flags()
{
	static std::uint32_t sampler_flags = 0 | BGFX_TEXTURE_RT | BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP;

	return sampler_flags;
}

void shutdown();

bool init(RendererType::Enum _type = RendererType::Count, uint16_t _vendorId = BGFX_PCI_ID_NONE,
		  uint16_t _deviceId = 0, bx::AllocatorI* _reallocator = NULL);

bool is_initted();
std::uint64_t screen_quad(float dest_width, float dest_height, float depth = 0.0f, float width = 1.0f,
						  float height = 1.0f);
std::uint64_t clip_quad(float depth = 0.0f, float width = 1.0f, float height = 1.0f);

void get_size_from_ratio(BackbufferRatio::Enum _ratio, uint16_t& _width, uint16_t& _height);
const std::string& get_renderer_filename_extension();
}
