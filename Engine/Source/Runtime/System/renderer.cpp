#include "renderer.h"
#include "graphics/graphics.h"
#include "core/logging/logging.h"
#include "core/common/string_utils.h"
#include <cstdarg>
#include "rendering/render_pass.h"

struct GfxCallback : public gfx::CallbackI
{
	virtual ~GfxCallback()
	{
	}

	virtual void traceVargs(const char* _filePath, std::uint16_t _line, const char* _format, std::va_list _argList) BX_OVERRIDE
	{
		auto logger = logging::get("Log");
		logger->trace() << string_utils::format(_format, _argList).c_str();
	}

	virtual void fatal(gfx::Fatal::Enum _code, const char* _str) BX_OVERRIDE
	{
		auto logger = logging::get("Log");

		logger->error() << _str;
	}

	virtual uint32_t cacheReadSize(uint64_t /*_id*/) BX_OVERRIDE
	{
		return 0;
	}

	virtual bool cacheRead(uint64_t /*_id*/, void* /*_data*/, uint32_t /*_size*/) BX_OVERRIDE
	{
		return false;
	}

	virtual void cacheWrite(uint64_t /*_id*/, const void* /*_data*/, uint32_t /*_size*/) BX_OVERRIDE
	{
	}

	virtual void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip) BX_OVERRIDE
	{
		BX_UNUSED(_filePath, _width, _height, _pitch, _data, _size, _yflip);
	}

	virtual void captureBegin(uint32_t /*_width*/, uint32_t /*_height*/, uint32_t /*_pitch*/, gfx::TextureFormat::Enum /*_format*/, bool /*_yflip*/) BX_OVERRIDE
	{
		BX_TRACE("Warning: using capture without callback (a.k.a. pointless).");
	}

	virtual void captureEnd() BX_OVERRIDE
	{
	}

	virtual void captureFrame(const void* /*_data*/, uint32_t /*_size*/) BX_OVERRIDE
	{
	}

};
static GfxCallback sGfxCallback;



namespace runtime
{
	bool Renderer::initialize()
	{	
		return true;
	}

	void Renderer::dispose()
	{
		gfx::shutdown();
	}
	bool Renderer::init_backend(RenderWindow& main_window)
	{
		gfx::PlatformData pd
		{
			nullptr,
			main_window.getSystemHandle(),
			nullptr,
			nullptr,
			nullptr
		};

		gfx::setPlatformData(pd);

		if (!gfx::init(gfx::RendererType::Count, 0, 0, &sGfxCallback))
			return false;

		main_window.setMain(true);

		return true;
	}

	void Renderer::frame_end()
	{
		_render_frame = gfx::frame();
		RenderPass::reset();
	}

}


