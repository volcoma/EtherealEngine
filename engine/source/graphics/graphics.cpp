#include "graphics.h"

namespace gfx
{
	VertexDecl PosTexCoord0Vertex::decl;
	VertexDecl MeshVertex::decl;

	TextureFormat::Enum get_best_format(std::uint16_t type_flags, std::uint32_t search_flags)
	{
		//( "DX11", "Go back over the list and find good formats for DX11" )
		bool is_depth = ((search_flags & FormatSearchFlags::RequireDepth) != 0);
		// Get best format for the target case.

		if (!is_depth)
		{
			//( "Try some of the new compressed formats (two channel, one channel, floating point, etc.)" )

			// Does the user prefer compressed textures in this case?
			// We will select compressed formats only in the
			// four channel non-floating point cases.
			if ((search_flags & FormatSearchFlags::PreferCompressed) &&
				(search_flags & FormatSearchFlags::FourChannels) &&
				!(search_flags & FormatSearchFlags::FloatingPoint))
			{
				// Alpha is required?
				if ((search_flags & FormatSearchFlags::RequireAlpha))
				{
					if (is_format_supported(type_flags, TextureFormat::BC2))
						return TextureFormat::BC2;
					if (is_format_supported(type_flags, TextureFormat::BC3))
						return TextureFormat::BC3;

				} // End if alpha required
				else
				{
					if (is_format_supported(type_flags, TextureFormat::BC1))
						return TextureFormat::BC1;

				} // End if no alpha required

			} // End if prefer compressed formats

			// Standard formats, and fallback for compression unsupported case
			bool accept_padding = ((search_flags & FormatSearchFlags::AllowPaddingChannels) != 0);
			bool requires_alpha = ((search_flags & FormatSearchFlags::RequireAlpha) != 0);
			if ((search_flags & FormatSearchFlags::FloatingPoint))
			{
				// Floating point formats ONLY!
				bool accept_half = ((search_flags & FormatSearchFlags::HalfPrecisionFloat) != 0);
				bool accept_full = ((search_flags & FormatSearchFlags::FullPrecisionFloat) != 0);

				// How many channels?
				if ((search_flags & FormatSearchFlags::FourChannels))
				{
					if (accept_full && is_format_supported(type_flags, TextureFormat::RGBA32F))
						return TextureFormat::RGBA32F;
					else if (accept_half && is_format_supported(type_flags, TextureFormat::RGBA16F))
						return TextureFormat::RGBA16F;

				} // End if FourChannel
				else if ((search_flags & FormatSearchFlags::TwoChannels))
				{
					if (!requires_alpha)
					{
						if (accept_full && is_format_supported(type_flags, TextureFormat::RG32F))
							return TextureFormat::RG32F;
						else if (accept_half && is_format_supported(type_flags, TextureFormat::RG16F))
							return TextureFormat::RG16F;
						else if (accept_padding && accept_half && is_format_supported(type_flags, TextureFormat::RGBA16F))
							return TextureFormat::RGBA16F;
						else if (accept_padding && accept_full && is_format_supported(type_flags, TextureFormat::RGBA32F))
							return TextureFormat::RGBA32F;

					} // End if !requires_alpha
					else
					{
						if (accept_padding && accept_half && is_format_supported(type_flags, TextureFormat::RGBA16F))
							return TextureFormat::RGBA16F;
						else if (accept_padding && accept_full && is_format_supported(type_flags, TextureFormat::RGBA32F))
							return TextureFormat::RGBA32F;

					} // End if requires_alpha

				} // End if TwoChannel
				else if ((search_flags & FormatSearchFlags::OneChannel))
				{
					if (!requires_alpha)
					{
						if (accept_full && is_format_supported(type_flags, TextureFormat::R32F))
							return TextureFormat::R32F;
						else if (accept_half && is_format_supported(type_flags, TextureFormat::R16F))
							return TextureFormat::R16F;
						else if (accept_padding && accept_half && is_format_supported(type_flags, TextureFormat::RG16F))
							return TextureFormat::RG16F;
						else if (accept_padding && accept_full && is_format_supported(type_flags, TextureFormat::RG32F))
							return TextureFormat::RG32F;
						else if (accept_padding && accept_half && is_format_supported(type_flags, TextureFormat::RGBA16F))
							return TextureFormat::RGBA16F;
						else if (accept_padding && accept_full && is_format_supported(type_flags, TextureFormat::RGBA32F))
							return TextureFormat::RGBA32F;

					} // End if !requires_alpha
					else
					{
						if (accept_padding && accept_half && is_format_supported(type_flags, TextureFormat::RGBA16F))
							return TextureFormat::RGBA16F;
						else if (accept_padding && accept_full && is_format_supported(type_flags, TextureFormat::RGBA32F))
							return TextureFormat::RGBA32F;

					} // End if requires_alpha

				} // End if OneChannel

			} // End if float
			else
			{
				// How many channels?
				if ((search_flags & FormatSearchFlags::FourChannels))
				{
					if (!requires_alpha)
					{
						if (is_format_supported(type_flags, TextureFormat::RGBA8))
							return TextureFormat::RGBA8;
						else if (is_format_supported(type_flags, TextureFormat::BGRA8))
							return TextureFormat::BGRA8;
						else if (is_format_supported(type_flags, TextureFormat::RGB10A2))
							return TextureFormat::RGB10A2;
						else if (is_format_supported(type_flags, TextureFormat::RGBA16))
							return TextureFormat::RGBA16;
						else if (is_format_supported(type_flags, TextureFormat::R5G6B5))
							return TextureFormat::R5G6B5;
						else if (is_format_supported(type_flags, TextureFormat::RGB5A1))
							return TextureFormat::RGB5A1;

					} // End if !requires_alpha
					else
					{
						if (is_format_supported(type_flags, TextureFormat::RGBA8))
							return TextureFormat::RGBA8;
						else if (is_format_supported(type_flags, TextureFormat::RGBA16))
							return TextureFormat::RGBA16;
						else if (is_format_supported(type_flags, TextureFormat::RGB10A2))
							return TextureFormat::RGB10A2;
						else if (is_format_supported(type_flags, TextureFormat::RGB5A1))
							return TextureFormat::RGB5A1;

					} // End if requires_alpha

				} // End if FourChannel
				else if ((search_flags & FormatSearchFlags::TwoChannels))
				{
					if (!requires_alpha)
					{
						if (is_format_supported(type_flags, TextureFormat::RG16))
							return TextureFormat::RG16;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGB8))
							return TextureFormat::RGB8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::BGRA8))
							return TextureFormat::BGRA8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGBA8))
							return TextureFormat::RGBA8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGB10A2))
							return TextureFormat::RGB10A2;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGBA16))
							return TextureFormat::RGBA16;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::R5G6B5))
							return TextureFormat::R5G6B5;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGB5A1))
							return TextureFormat::RGB5A1;

					} // End if !requires_alpha
					else
					{
						if (accept_padding && is_format_supported(type_flags, TextureFormat::BGRA8))
							return TextureFormat::BGRA8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGBA8))
							return TextureFormat::RGBA8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGBA16))
							return TextureFormat::RGBA16;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGB10A2))
							return TextureFormat::RGB10A2;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGB5A1))
							return TextureFormat::RGB5A1;

					} // End if requires_alpha

				} // End if TwoChannel
				else if ((search_flags & FormatSearchFlags::OneChannel))
				{
					if (!requires_alpha)
					{
						if (is_format_supported(type_flags, TextureFormat::R8))
							return TextureFormat::R8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RG16))
							return TextureFormat::RG16;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGB8))
							return TextureFormat::RGB8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::BGRA8))
							return TextureFormat::BGRA8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGBA8))
							return TextureFormat::RGBA8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGB10A2))
							return TextureFormat::RGB10A2;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGBA16))
							return TextureFormat::RGBA16;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::R5G6B5))
							return TextureFormat::R5G6B5;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGB5A1))
							return TextureFormat::RGB5A1;

					} // End if !requires_alpha
					else
					{
						if (is_format_supported(type_flags, TextureFormat::A8))
							return TextureFormat::A8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::BGRA8))
							return TextureFormat::BGRA8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGBA8))
							return TextureFormat::RGBA8;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGBA16))
							return TextureFormat::RGBA16;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGB10A2))
							return TextureFormat::RGB10A2;
						else if (accept_padding && is_format_supported(type_flags, TextureFormat::RGB5A1))
							return TextureFormat::RGB5A1;

					} // End if requires_alpha

				} // End if OneChannel

			} // End if !float

		} // End if color formats
		else
		{
			bool requires_stencil = ((search_flags & FormatSearchFlags::RequireStencil) != 0);
			if ((search_flags & FormatSearchFlags::FloatingPoint))
			{
				// Floating point formats ONLY!
				bool accept_half = ((search_flags & FormatSearchFlags::HalfPrecisionFloat) != 0);
				bool accept_full = ((search_flags & FormatSearchFlags::FullPrecisionFloat) != 0);
				if (!requires_stencil)
				{
					if (accept_full && is_format_supported(type_flags, TextureFormat::D32F))
						return TextureFormat::D32F;
					else if (accept_full && is_format_supported(type_flags, TextureFormat::D24F))
						return TextureFormat::D24F;

				} // End if !requires_stencil
				else
				{
					// no floating point that supports stencil
					//if (accept_full && is_format_supported(type_flags, TextureFormat::D24S8))
					//	return TextureFormat::D24S8;

				} // End if requires_stencil

			} // End if float
			else
			{
				if (!requires_stencil)
				{
					if (is_format_supported(type_flags, TextureFormat::D32))
						return TextureFormat::D32;
					else if (is_format_supported(type_flags, TextureFormat::D24))
						return TextureFormat::D24;
					else if (is_format_supported(type_flags, TextureFormat::D16))
						return TextureFormat::D16;

				} // End if !requires_stencil
				else
				{
					if (is_format_supported(type_flags, TextureFormat::D24S8))
						return TextureFormat::D24S8;

				} // End if requires_stencil

			} // End if !float

		} // End if depth formats

		// Unsupported format.
		return TextureFormat::Unknown;
	}
	static bool initted = false;

	void shutdown()
	{
		if (initted)
			bgfx::shutdown();
	}

	bool init(RendererType::Enum _type /*= RendererType::Count */, uint16_t _vendorId /*= BGFX_PCI_ID_NONE */, uint16_t _deviceId /*= 0 */, CallbackI* _callback /*= NULL */, bx::AllocatorI* _reallocator /*= NULL */)
	{
		initted = bgfx::init(_type, _vendorId, _deviceId, _callback, _reallocator);

		if (initted)
		{
			PosTexCoord0Vertex::init();
			MeshVertex::init();
		}

		return initted;
	}

	bool is_initted()
	{
		return initted;
	}

	std::uint64_t screen_quad(float dest_width, float dest_height, float depth, float width, float height)
	{
		float texture_half = get_half_texel();
		bool origin_bottom_left = is_origin_bottom_left();

		if (3 == getAvailTransientVertexBuffer(3, PosTexCoord0Vertex::decl))
		{
			TransientVertexBuffer vb;
			allocTransientVertexBuffer(&vb, 3, PosTexCoord0Vertex::decl);
			PosTexCoord0Vertex* vertex = (PosTexCoord0Vertex*)vb.data;

			const float minx = -width;
			const float maxx = width;
			const float miny = 0.0f;
			const float maxy = height*2.0f;

			const float texel_half_w = texture_half / dest_width;
			const float texel_half_h = texture_half / dest_height;
			const float minu = -1.0f + texel_half_w;
			const float maxu = 1.0f + texel_half_h;

			const float zz = depth;

			float minv = texel_half_h;
			float maxv = 2.0f + texel_half_h;

			if (origin_bottom_left)
			{
				float temp = minv;
				minv = maxv;
				maxv = temp;

				minv -= 1.0f;
				maxv -= 1.0f;
			}

			vertex[0].x = minx;
			vertex[0].y = miny;
			vertex[0].z = zz;
			vertex[0].u = minu;
			vertex[0].v = minv;

			vertex[1].x = maxx;
			vertex[1].y = miny;
			vertex[1].z = zz;
			vertex[1].u = maxu;
			vertex[1].v = minv;

			vertex[2].x = maxx;
			vertex[2].y = maxy;
			vertex[2].z = zz;
			vertex[2].u = maxu;
			vertex[2].v = maxv;

			setVertexBuffer(&vb);
		}

		return 0;
	}

	std::uint64_t clip_quad(float depth, float width, float height)
	{
		float texture_half = get_half_texel();
		bool origin_bottom_left = is_origin_bottom_left();

		if (4 == getAvailTransientVertexBuffer(4, PosTexCoord0Vertex::decl))
		{
			TransientVertexBuffer vb;
			allocTransientVertexBuffer(&vb, 4, PosTexCoord0Vertex::decl);
			PosTexCoord0Vertex* vertex = (PosTexCoord0Vertex*)vb.data;

			const float minx = -width;
			const float maxx = width;
			const float miny = -height;
			const float maxy = height;

			const float texel_half_w = texture_half;
			const float texel_half_h = texture_half;
			const float minu = 0.0f;
			const float maxu = 1.0f;

			const float zz = depth;

			float minv = 1.0f;
			float maxv = 0.0f;

			if (origin_bottom_left)
			{
				minv = 1.0f - minv;
				maxv = 1.0f - maxv;
			}

			vertex[0].x = minx;
			vertex[0].y = maxy;
			vertex[0].z = zz;
			vertex[0].u = minu;
			vertex[0].v = maxv;

			vertex[1].x = maxx;
			vertex[1].y = maxy;
			vertex[1].z = zz;
			vertex[1].u = maxu;
			vertex[1].v = maxv;

			vertex[2].x = minx;
			vertex[2].y = miny;
			vertex[2].z = zz;
			vertex[2].u = minu;
			vertex[2].v = minv;

			vertex[3].x = maxx;
			vertex[3].y = miny;
			vertex[3].z = zz;
			vertex[3].u = maxu;
			vertex[3].v = minv;

			setVertexBuffer(&vb);
		}

		return BGFX_STATE_PT_TRISTRIP;
	}

	

	void get_size_from_ratio(BackbufferRatio::Enum _ratio, uint16_t& _width, uint16_t& _height)
	{
		auto stats = gfx::getStats();
		_width = stats->width;
		_height = stats->height;
		switch (_ratio)
		{
		case BackbufferRatio::Half:      _width /= 2; _height /= 2; break;
		case BackbufferRatio::Quarter:   _width /= 4; _height /= 4; break;
		case BackbufferRatio::Eighth:    _width /= 8; _height /= 8; break;
		case BackbufferRatio::Sixteenth: _width /= 16; _height /= 16; break;
		case BackbufferRatio::Double:    _width *= 2; _height *= 2; break;

		default:
			break;
		}

		_width = bx::uint16_max(1, _width);
		_height = bx::uint16_max(1, _height);
		
	}

}