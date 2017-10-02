#include "graphics.h"
#include <algorithm>
#include <map>
namespace gfx
{
static std::map<std::string, std::function<void(const std::string& log_msg)>> loggers;
void set_info_logger(std::function<void(const std::string& log_msg)> logger)
{
	loggers["info"] = logger;
}
static std::function<void(const std::string& log_msg)> warning_logger;
void set_warning_logger(std::function<void(const std::string& log_msg)> logger)
{
	loggers["warning"] = logger;
}
static std::function<void(const std::string& log_msg)> error_logger;
void set_error_logger(std::function<void(const std::string& log_msg)> logger)
{
	loggers["error"] = logger;
}
void log(const std::string& category, const std::string& log_msg)
{
	if(loggers["category"])
		loggers["category"](log_msg);
}


struct gfx_callback : public gfx::CallbackI
{
	virtual ~gfx_callback()
	{
	}

	virtual void traceVargs(const char* /*_filePath*/, std::uint16_t /*_line*/, const char* /*_format*/,
							va_list /*_argList*/)
	{
        //log("info", _str);
	}

	virtual void fatal(gfx::Fatal::Enum /*_code*/, const char* _str)
	{
		log("error", _str);
	}

	virtual uint32_t cacheReadSize(uint64_t /*_id*/)
	{
		return 0;
	}

	virtual bool cacheRead(uint64_t /*_id*/, void* /*_data*/, uint32_t /*_size*/)
	{
		return false;
	}

	virtual void cacheWrite(uint64_t /*_id*/, const void* /*_data*/, uint32_t /*_size*/)
	{
	}

	virtual void screenShot(const char* /*_filePath*/, uint32_t /*_width*/, uint32_t /*_height*/,
							uint32_t /*_pitch*/, const void* /*_data*/, uint32_t /*_size*/, bool /*_yflip*/)
	{
	}

	virtual void captureBegin(uint32_t /*_width*/, uint32_t /*_height*/, uint32_t /*_pitch*/,
							  gfx::TextureFormat::Enum /*_format*/, bool /*_yflip*/)
	{
	}

	virtual void captureEnd()
	{
	}

	virtual void captureFrame(const void* /*_data*/, uint32_t /*_size*/)
	{
	}
};


VertexDecl pos_texcoord0_vertex::decl;
VertexDecl mesh_vertex::decl;

TextureFormat::Enum get_best_format(std::uint16_t type_flags, std::uint32_t search_flags)
{
	//( "DX11", "Go back over the list and find good formats for DX11" )
	bool is_depth = ((search_flags & format_search_flags::RequireDepth) != 0);
	// Get best format for the target case.

	if(!is_depth)
	{
		//( "Try some of the new compressed formats (two channel, one channel,
		// floating point, etc.)" )

		// Does the user prefer compressed textures in this case?
		// We will select compressed formats only in the
		// four channel non-floating point cases.
		if((search_flags & format_search_flags::PreferCompressed) &&
		   (search_flags & format_search_flags::FourChannels) &&
		   !(search_flags & format_search_flags::FloatingPoint))
		{
			// Alpha is required?
			if((search_flags & format_search_flags::RequireAlpha))
			{
				if(is_format_supported(type_flags, TextureFormat::BC2))
					return TextureFormat::BC2;
				if(is_format_supported(type_flags, TextureFormat::BC3))
					return TextureFormat::BC3;

			} // End if alpha required
			else
			{
				if(is_format_supported(type_flags, TextureFormat::BC1))
					return TextureFormat::BC1;

			} // End if no alpha required

		} // End if prefer compressed formats

		// Standard formats, and fallback for compression unsupported case
		bool accept_padding = ((search_flags & format_search_flags::AllowPaddingChannels) != 0);
		bool requires_alpha = ((search_flags & format_search_flags::RequireAlpha) != 0);
		if((search_flags & format_search_flags::FloatingPoint))
		{
			// Floating point formats ONLY!
			bool accept_half = ((search_flags & format_search_flags::HalfPrecisionFloat) != 0);
			bool accept_full = ((search_flags & format_search_flags::FullPrecisionFloat) != 0);

			// How many channels?
			if((search_flags & format_search_flags::FourChannels))
			{
				if(accept_full && is_format_supported(type_flags, TextureFormat::RGBA32F))
					return TextureFormat::RGBA32F;
				else if(accept_half && is_format_supported(type_flags, TextureFormat::RGBA16F))
					return TextureFormat::RGBA16F;

			} // End if FourChannel
			else if((search_flags & format_search_flags::TwoChannels))
			{
				if(!requires_alpha)
				{
					if(accept_full && is_format_supported(type_flags, TextureFormat::RG32F))
						return TextureFormat::RG32F;
					else if(accept_half && is_format_supported(type_flags, TextureFormat::RG16F))
						return TextureFormat::RG16F;
					else if(accept_padding && accept_half &&
							is_format_supported(type_flags, TextureFormat::RGBA16F))
						return TextureFormat::RGBA16F;
					else if(accept_padding && accept_full &&
							is_format_supported(type_flags, TextureFormat::RGBA32F))
						return TextureFormat::RGBA32F;

				} // End if !requires_alpha
				else
				{
					if(accept_padding && accept_half &&
					   is_format_supported(type_flags, TextureFormat::RGBA16F))
						return TextureFormat::RGBA16F;
					else if(accept_padding && accept_full &&
							is_format_supported(type_flags, TextureFormat::RGBA32F))
						return TextureFormat::RGBA32F;

				} // End if requires_alpha

			} // End if TwoChannel
			else if((search_flags & format_search_flags::OneChannel))
			{
				if(!requires_alpha)
				{
					if(accept_full && is_format_supported(type_flags, TextureFormat::R32F))
						return TextureFormat::R32F;
					else if(accept_half && is_format_supported(type_flags, TextureFormat::R16F))
						return TextureFormat::R16F;
					else if(accept_padding && accept_half &&
							is_format_supported(type_flags, TextureFormat::RG16F))
						return TextureFormat::RG16F;
					else if(accept_padding && accept_full &&
							is_format_supported(type_flags, TextureFormat::RG32F))
						return TextureFormat::RG32F;
					else if(accept_padding && accept_half &&
							is_format_supported(type_flags, TextureFormat::RGBA16F))
						return TextureFormat::RGBA16F;
					else if(accept_padding && accept_full &&
							is_format_supported(type_flags, TextureFormat::RGBA32F))
						return TextureFormat::RGBA32F;

				} // End if !requires_alpha
				else
				{
					if(accept_padding && accept_half &&
					   is_format_supported(type_flags, TextureFormat::RGBA16F))
						return TextureFormat::RGBA16F;
					else if(accept_padding && accept_full &&
							is_format_supported(type_flags, TextureFormat::RGBA32F))
						return TextureFormat::RGBA32F;

				} // End if requires_alpha

			} // End if OneChannel

		} // End if float
		else
		{
			// How many channels?
			if((search_flags & format_search_flags::FourChannels))
			{
				if(!requires_alpha)
				{
					if(is_format_supported(type_flags, TextureFormat::RGBA8))
						return TextureFormat::RGBA8;
					else if(is_format_supported(type_flags, TextureFormat::BGRA8))
						return TextureFormat::BGRA8;
					else if(is_format_supported(type_flags, TextureFormat::RGB10A2))
						return TextureFormat::RGB10A2;
					else if(is_format_supported(type_flags, TextureFormat::RGBA16))
						return TextureFormat::RGBA16;
					else if(is_format_supported(type_flags, TextureFormat::R5G6B5))
						return TextureFormat::R5G6B5;
					else if(is_format_supported(type_flags, TextureFormat::RGB5A1))
						return TextureFormat::RGB5A1;

				} // End if !requires_alpha
				else
				{
					if(is_format_supported(type_flags, TextureFormat::RGBA8))
						return TextureFormat::RGBA8;
					else if(is_format_supported(type_flags, TextureFormat::RGBA16))
						return TextureFormat::RGBA16;
					else if(is_format_supported(type_flags, TextureFormat::RGB10A2))
						return TextureFormat::RGB10A2;
					else if(is_format_supported(type_flags, TextureFormat::RGB5A1))
						return TextureFormat::RGB5A1;

				} // End if requires_alpha

			} // End if FourChannel
			else if((search_flags & format_search_flags::TwoChannels))
			{
				if(!requires_alpha)
				{
					if(is_format_supported(type_flags, TextureFormat::RG16))
						return TextureFormat::RG16;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGB8))
						return TextureFormat::RGB8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::BGRA8))
						return TextureFormat::BGRA8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGBA8))
						return TextureFormat::RGBA8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGB10A2))
						return TextureFormat::RGB10A2;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGBA16))
						return TextureFormat::RGBA16;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::R5G6B5))
						return TextureFormat::R5G6B5;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGB5A1))
						return TextureFormat::RGB5A1;

				} // End if !requires_alpha
				else
				{
					if(accept_padding && is_format_supported(type_flags, TextureFormat::BGRA8))
						return TextureFormat::BGRA8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGBA8))
						return TextureFormat::RGBA8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGBA16))
						return TextureFormat::RGBA16;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGB10A2))
						return TextureFormat::RGB10A2;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGB5A1))
						return TextureFormat::RGB5A1;

				} // End if requires_alpha

			} // End if TwoChannel
			else if((search_flags & format_search_flags::OneChannel))
			{
				if(!requires_alpha)
				{
					if(is_format_supported(type_flags, TextureFormat::R8))
						return TextureFormat::R8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RG16))
						return TextureFormat::RG16;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGB8))
						return TextureFormat::RGB8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::BGRA8))
						return TextureFormat::BGRA8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGBA8))
						return TextureFormat::RGBA8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGB10A2))
						return TextureFormat::RGB10A2;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGBA16))
						return TextureFormat::RGBA16;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::R5G6B5))
						return TextureFormat::R5G6B5;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGB5A1))
						return TextureFormat::RGB5A1;

				} // End if !requires_alpha
				else
				{
					if(is_format_supported(type_flags, TextureFormat::A8))
						return TextureFormat::A8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::BGRA8))
						return TextureFormat::BGRA8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGBA8))
						return TextureFormat::RGBA8;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGBA16))
						return TextureFormat::RGBA16;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGB10A2))
						return TextureFormat::RGB10A2;
					else if(accept_padding && is_format_supported(type_flags, TextureFormat::RGB5A1))
						return TextureFormat::RGB5A1;

				} // End if requires_alpha

			} // End if OneChannel

		} // End if !float

	} // End if color formats
	else
	{
		bool requires_stencil = ((search_flags & format_search_flags::RequireStencil) != 0);
		if((search_flags & format_search_flags::FloatingPoint))
		{
			// Floating point formats ONLY!
			// bool accept_half = ((search_flags &
			// format_search_flags::HalfPrecisionFloat) != 0);
			bool accept_full = ((search_flags & format_search_flags::FullPrecisionFloat) != 0);
			if(!requires_stencil)
			{
				if(accept_full && is_format_supported(type_flags, TextureFormat::D32F))
					return TextureFormat::D32F;
				else if(accept_full && is_format_supported(type_flags, TextureFormat::D24F))
					return TextureFormat::D24F;

			} // End if !requires_stencil
			else
			{
				// no floating point that supports stencil
				// if (accept_full && is_format_supported(type_flags,
				// TextureFormat::D24S8))
				//	return TextureFormat::D24S8;

			} // End if requires_stencil

		} // End if float
		else
		{
			if(!requires_stencil)
			{
				if(is_format_supported(type_flags, TextureFormat::D32))
					return TextureFormat::D32;
				else if(is_format_supported(type_flags, TextureFormat::D24))
					return TextureFormat::D24;
				else if(is_format_supported(type_flags, TextureFormat::D16))
					return TextureFormat::D16;

			} // End if !requires_stencil
			else
			{
				if(is_format_supported(type_flags, TextureFormat::D24S8))
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
	if(initted)
		bgfx::shutdown();
}

bool init(RendererType::Enum _type /*= RendererType::Count */, uint16_t _vendorId /*= BGFX_PCI_ID_NONE */,
		  uint16_t _deviceId /*= 0 */,
		  bx::AllocatorI* _reallocator /*= NULL */)
{
    static gfx_callback callback;
    
	initted = bgfx::init(_type, _vendorId, _deviceId, &callback, _reallocator);

	if(initted)
	{
		pos_texcoord0_vertex::init();
		mesh_vertex::init();
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

	if(3 == getAvailTransientVertexBuffer(3, pos_texcoord0_vertex::decl))
	{
		TransientVertexBuffer vb;
		allocTransientVertexBuffer(&vb, 3, pos_texcoord0_vertex::decl);
		pos_texcoord0_vertex* vertex = (pos_texcoord0_vertex*)vb.data;

		const float minx = -width;
		const float maxx = width;
		const float miny = 0.0f;
		const float maxy = height * 2.0f;

		const float texel_half_w = texture_half / dest_width;
		const float texel_half_h = texture_half / dest_height;
		const float minu = -1.0f + texel_half_w;
		const float maxu = 1.0f + texel_half_h;

		const float zz = depth;

		float minv = texel_half_h;
		float maxv = 2.0f + texel_half_h;

		if(origin_bottom_left)
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

		setVertexBuffer(0, &vb);
	}

	return 0;
}

std::uint64_t clip_quad(float depth, float width, float height)
{
	// float texture_half = get_half_texel();
	bool origin_bottom_left = is_origin_bottom_left();

	if(4 == getAvailTransientVertexBuffer(4, pos_texcoord0_vertex::decl))
	{
		TransientVertexBuffer vb;
		allocTransientVertexBuffer(&vb, 4, pos_texcoord0_vertex::decl);
		pos_texcoord0_vertex* vertex = (pos_texcoord0_vertex*)vb.data;

		const float minx = -width;
		const float maxx = width;
		const float miny = -height;
		const float maxy = height;

		// const float texel_half_w = texture_half;
		// const float texel_half_h = texture_half;
		const float minu = 0.0f;
		const float maxu = 1.0f;

		const float zz = depth;

		float minv = 1.0f;
		float maxv = 0.0f;

		if(origin_bottom_left)
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

		setVertexBuffer(0, &vb);
	}

	return BGFX_STATE_PT_TRISTRIP;
}

void get_size_from_ratio(BackbufferRatio::Enum _ratio, uint16_t& _width, uint16_t& _height)
{
	auto stats = gfx::getStats();
	_width = stats->width;
	_height = stats->height;
	switch(_ratio)
	{
		case BackbufferRatio::Half:
			_width /= 2;
			_height /= 2;
			break;
		case BackbufferRatio::Quarter:
			_width /= 4;
			_height /= 4;
			break;
		case BackbufferRatio::Eighth:
			_width /= 8;
			_height /= 8;
			break;
		case BackbufferRatio::Sixteenth:
			_width /= 16;
			_height /= 16;
			break;
		case BackbufferRatio::Double:
			_width *= 2;
			_height *= 2;
			break;

		default:
			break;
	}

	_width = std::max<uint16_t>(1, _width);
	_height = std::max<uint16_t>(1, _height);
}

const std::string& get_renderer_filename_extension()
{
	static std::map<gfx::RendererType::Enum, std::string> types = {
		{RendererType::Direct3D9, ".dx9"},   {RendererType::Direct3D11, ".dx11"},
		{RendererType::Direct3D12, ".dx12"}, {RendererType::Gnm, ".gnm"},
		{RendererType::Metal, ".metal"},	 {RendererType::OpenGL, ".gl"},
		{RendererType::OpenGLES, ".gles"},   {RendererType::Noop, ".noop"}};

	return types[getRendererType()];
}
}
