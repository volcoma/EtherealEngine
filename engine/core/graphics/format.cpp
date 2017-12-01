#include "format.h"

namespace gfx
{

texture_format get_best_format(std::uint16_t type_flags, std::uint32_t search_flags)
{
	//( "DX11", "Go back over the list and find good formats for DX11" )
	bool is_depth = ((search_flags & format_search_flags::requires_depth) != 0);
	// Get best format for the target case.

	if(!is_depth)
	{
		//( "Try some of the new compressed formats (two channel, one channel,
		// floating point, etc.)" )

		// Does the user prefer compressed textures in this case?
		// We will select compressed formats only in the
		// four channel non-floating point cases.
		if((search_flags & format_search_flags::prefer_compressed) &&
		   (search_flags & format_search_flags::four_channels) &&
		   !(search_flags & format_search_flags::floating_point))
		{
			// Alpha is required?
			if((search_flags & format_search_flags::requires_alpha))
			{
				if(is_format_supported(type_flags, texture_format::BC2))
					return texture_format::BC2;
				if(is_format_supported(type_flags, texture_format::BC3))
					return texture_format::BC3;

			} // End if alpha required
			else
			{
				if(is_format_supported(type_flags, texture_format::BC1))
					return texture_format::BC1;

			} // End if no alpha required

		} // End if prefer compressed formats

		// Standard formats, and fallback for compression unsupported case
		bool accept_padding = ((search_flags & format_search_flags::allow_padding_channels) != 0);
		bool requires_alpha = ((search_flags & format_search_flags::requires_alpha) != 0);
		if((search_flags & format_search_flags::floating_point))
		{
			// Floating point formats ONLY!
			bool accept_half = ((search_flags & format_search_flags::half_precision_float) != 0);
			bool accept_full = ((search_flags & format_search_flags::full_precision_float) != 0);

			// How many channels?
			if((search_flags & format_search_flags::four_channels))
			{
				if(accept_full && is_format_supported(type_flags, texture_format::RGBA32F))
					return texture_format::RGBA32F;
				else if(accept_half && is_format_supported(type_flags, texture_format::RGBA16F))
					return texture_format::RGBA16F;

			} // End if FourChannel
			else if((search_flags & format_search_flags::two_channels))
			{
				if(!requires_alpha)
				{
					if(accept_full && is_format_supported(type_flags, texture_format::RG32F))
						return texture_format::RG32F;
					else if(accept_half && is_format_supported(type_flags, texture_format::RG16F))
						return texture_format::RG16F;
					else if(accept_padding && accept_half &&
							is_format_supported(type_flags, texture_format::RGBA16F))
						return texture_format::RGBA16F;
					else if(accept_padding && accept_full &&
							is_format_supported(type_flags, texture_format::RGBA32F))
						return texture_format::RGBA32F;

				} // End if !requires_alpha
				else
				{
					if(accept_padding && accept_half &&
					   is_format_supported(type_flags, texture_format::RGBA16F))
						return texture_format::RGBA16F;
					else if(accept_padding && accept_full &&
							is_format_supported(type_flags, texture_format::RGBA32F))
						return texture_format::RGBA32F;

				} // End if requires_alpha

			} // End if TwoChannel
			else if((search_flags & format_search_flags::one_channel))
			{
				if(!requires_alpha)
				{
					if(accept_full && is_format_supported(type_flags, texture_format::R32F))
						return texture_format::R32F;
					else if(accept_half && is_format_supported(type_flags, texture_format::R16F))
						return texture_format::R16F;
					else if(accept_padding && accept_half &&
							is_format_supported(type_flags, texture_format::RG16F))
						return texture_format::RG16F;
					else if(accept_padding && accept_full &&
							is_format_supported(type_flags, texture_format::RG32F))
						return texture_format::RG32F;
					else if(accept_padding && accept_half &&
							is_format_supported(type_flags, texture_format::RGBA16F))
						return texture_format::RGBA16F;
					else if(accept_padding && accept_full &&
							is_format_supported(type_flags, texture_format::RGBA32F))
						return texture_format::RGBA32F;

				} // End if !requires_alpha
				else
				{
					if(accept_padding && accept_half &&
					   is_format_supported(type_flags, texture_format::RGBA16F))
						return texture_format::RGBA16F;
					else if(accept_padding && accept_full &&
							is_format_supported(type_flags, texture_format::RGBA32F))
						return texture_format::RGBA32F;

				} // End if requires_alpha

			} // End if one_channel

		} // End if float
		else
		{
			// How many channels?
			if((search_flags & format_search_flags::four_channels))
			{
				if(!requires_alpha)
				{
					if(is_format_supported(type_flags, texture_format::RGBA8))
						return texture_format::RGBA8;
					else if(is_format_supported(type_flags, texture_format::BGRA8))
						return texture_format::BGRA8;
					else if(is_format_supported(type_flags, texture_format::RGB10A2))
						return texture_format::RGB10A2;
					else if(is_format_supported(type_flags, texture_format::RGBA16))
						return texture_format::RGBA16;
					else if(is_format_supported(type_flags, texture_format::R5G6B5))
						return texture_format::R5G6B5;
					else if(is_format_supported(type_flags, texture_format::RGB5A1))
						return texture_format::RGB5A1;

				} // End if !requires_alpha
				else
				{
					if(is_format_supported(type_flags, texture_format::RGBA8))
						return texture_format::RGBA8;
					else if(is_format_supported(type_flags, texture_format::RGBA16))
						return texture_format::RGBA16;
					else if(is_format_supported(type_flags, texture_format::RGB10A2))
						return texture_format::RGB10A2;
					else if(is_format_supported(type_flags, texture_format::RGB5A1))
						return texture_format::RGB5A1;

				} // End if requires_alpha

			} // End if FourChannel
			else if((search_flags & format_search_flags::two_channels))
			{
				if(!requires_alpha)
				{
					if(is_format_supported(type_flags, texture_format::RG16))
						return texture_format::RG16;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGB8))
						return texture_format::RGB8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::BGRA8))
						return texture_format::BGRA8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGBA8))
						return texture_format::RGBA8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGB10A2))
						return texture_format::RGB10A2;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGBA16))
						return texture_format::RGBA16;
					else if(accept_padding && is_format_supported(type_flags, texture_format::R5G6B5))
						return texture_format::R5G6B5;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGB5A1))
						return texture_format::RGB5A1;

				} // End if !requires_alpha
				else
				{
					if(accept_padding && is_format_supported(type_flags, texture_format::BGRA8))
						return texture_format::BGRA8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGBA8))
						return texture_format::RGBA8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGBA16))
						return texture_format::RGBA16;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGB10A2))
						return texture_format::RGB10A2;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGB5A1))
						return texture_format::RGB5A1;

				} // End if requires_alpha

			} // End if two_channels
			else if((search_flags & format_search_flags::one_channel))
			{
				if(!requires_alpha)
				{
					if(is_format_supported(type_flags, texture_format::R8))
						return texture_format::R8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RG16))
						return texture_format::RG16;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGB8))
						return texture_format::RGB8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::BGRA8))
						return texture_format::BGRA8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGBA8))
						return texture_format::RGBA8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGB10A2))
						return texture_format::RGB10A2;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGBA16))
						return texture_format::RGBA16;
					else if(accept_padding && is_format_supported(type_flags, texture_format::R5G6B5))
						return texture_format::R5G6B5;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGB5A1))
						return texture_format::RGB5A1;

				} // End if !requires_alpha
				else
				{
					if(is_format_supported(type_flags, texture_format::A8))
						return texture_format::A8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::BGRA8))
						return texture_format::BGRA8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGBA8))
						return texture_format::RGBA8;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGBA16))
						return texture_format::RGBA16;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGB10A2))
						return texture_format::RGB10A2;
					else if(accept_padding && is_format_supported(type_flags, texture_format::RGB5A1))
						return texture_format::RGB5A1;

				} // End if requires_alpha

			} // End if one_channel

		} // End if !float

	} // End if color formats
	else
	{
		bool requires_stencil = ((search_flags & format_search_flags::requires_stencil) != 0);
		if((search_flags & format_search_flags::floating_point))
		{
			// Floating point formats ONLY!
			// bool accept_half = ((search_flags &
			// format_search_flags::half_precision_float) != 0);
			bool accept_full = ((search_flags & format_search_flags::full_precision_float) != 0);
			if(!requires_stencil)
			{
				if(accept_full && is_format_supported(type_flags, texture_format::D32F))
					return texture_format::D32F;
				else if(accept_full && is_format_supported(type_flags, texture_format::D24F))
					return texture_format::D24F;

			} // End if !requires_stencil
			else
			{
				// no floating point that supports stencil
				// if (accept_full && is_format_supported(type_flags,
				// texture_format::D24S8))
				//	return texture_format::D24S8;

			} // End if requires_stencil

		} // End if float
		else
		{
			if(!requires_stencil)
			{
				if(is_format_supported(type_flags, texture_format::D32))
					return texture_format::D32;
				else if(is_format_supported(type_flags, texture_format::D24))
					return texture_format::D24;
				else if(is_format_supported(type_flags, texture_format::D16))
					return texture_format::D16;

			} // End if !requires_stencil
			else
			{
				if(is_format_supported(type_flags, texture_format::D24S8))
					return texture_format::D24S8;

			} // End if requires_stencil

		} // End if !float

	} // End if depth formats

	// Unsupported format.
	return texture_format::Unknown;
}

uint32_t get_default_rt_sampler_flags()
{
    static std::uint32_t sampler_flags = 0 | BGFX_TEXTURE_RT | BGFX_TEXTURE_U_CLAMP | BGFX_TEXTURE_V_CLAMP;
    
    return sampler_flags;
}

bool is_format_supported(uint16_t flags, texture_format format)
{
    const std::uint32_t formatCaps = bgfx::getCaps()->formats[format];
    return 0 != (formatCaps & flags);
}

}
