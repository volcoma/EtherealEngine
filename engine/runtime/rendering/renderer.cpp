#include "renderer.h"
#include "core/graphics/graphics.h"
#include "core/logging/logging.h"
#include "core/common/string.h"
#include <cstdarg>
#include "render_pass.h"
#include "../system/engine.h"

struct GfxCallback : public gfx::CallbackI
{
	virtual ~GfxCallback()
	{
	}

	virtual void traceVargs(const char* _filePath, std::uint16_t _line, const char* _format, std::va_list _argList)
	{
		//APPLOG_TRACE(string_utils::format(_format, _argList).c_str());
	}

	virtual void fatal(gfx::Fatal::Enum _code, const char* _str)
	{
		APPLOG_ERROR(_str);
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

	virtual void screenShot(const char* _filePath, uint32_t _width, uint32_t _height, uint32_t _pitch, const void* _data, uint32_t _size, bool _yflip)
	{
	}

	virtual void captureBegin(uint32_t /*_width*/, uint32_t /*_height*/, uint32_t /*_pitch*/, gfx::TextureFormat::Enum /*_format*/, bool /*_yflip*/)
	{
	}

	virtual void captureEnd()
	{
	}

	virtual void captureFrame(const void* /*_data*/, uint32_t /*_size*/)
	{
	}

};

namespace runtime
{
	bool renderer::initialize()
	{	
		on_frame_end.connect(this, &renderer::frame_end);

		return true;
	}

	void renderer::dispose()
	{
		on_frame_end.disconnect(this, &renderer::frame_end);
		gfx::shutdown();
	}

	bool renderer::init_backend(mml::window& main_window)
	{
		gfx::PlatformData pd
		{
			(void*)(uintptr_t)main_window.get_system_handle_specific(),
            (void*)(uintptr_t)main_window.get_system_handle(),
			nullptr,
			nullptr,
			nullptr
		};

		gfx::setPlatformData(pd);

		static GfxCallback callback;
		if (!gfx::init(gfx::RendererType::Count, 0, 0, &callback))
			return false;

		if (gfx::getRendererType() == gfx::RendererType::Direct3D9)
		{
			APPLOG_ERROR("Does not support dx9. Minimum supported is dx11.");
			return false;
		}
		return true;
	}

	void renderer::frame_end(std::chrono::duration<float>)
	{
		_render_frame = gfx::frame();
		render_pass::reset();
	}

}


