#pragma once

#include "bgfx/bgfx.h"
#include <cstdint>
namespace gfx
{

using texture_format = bgfx::TextureFormat::Enum;

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
};

bool is_format_supported(std::uint16_t flags, texture_format format);

texture_format get_best_format(std::uint16_t type, std::uint32_t search_flags);

std::uint32_t get_default_rt_sampler_flags();
}
