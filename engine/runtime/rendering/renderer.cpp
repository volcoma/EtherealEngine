#include "renderer.h"

#include "../system/events.h"

#include <core/common/assert.hpp>
#include <core/graphics/graphics.h>
#include <core/graphics/render_pass.h>
#include <core/logging/logging.h>

#include <algorithm>
#include <cstdarg>

namespace runtime
{
renderer::renderer(cmd_line::parser& parser)
{
	on_platform_events.connect(this, &renderer::platform_events);
	on_frame_end.connect(this, &renderer::frame_end);

	if(!init_backend(parser))
	{
		return;
	}

	mml::video_mode desktop = mml::video_mode::get_desktop_mode();
	desktop.width = 1280;
	desktop.height = 720;
	auto window = std::make_unique<render_window>(desktop, "ETHEREAL", mml::style::standard);
	window->request_focus();
	register_window(std::move(window));
	process_pending_windows();
}

renderer::~renderer()
{
	on_platform_events.disconnect(this, &renderer::platform_events);
	on_frame_end.disconnect(this, &renderer::frame_end);
	windows_.clear();
	windows_pending_addition_.clear();
	gfx::shutdown();
}

render_window* renderer::get_focused_window() const
{
	render_window* focused_window = nullptr;

	const auto& windows = get_windows();
	auto it = std::find_if(std::begin(windows), std::end(windows),
						   [](const auto& window) { return window->has_focus(); });

	if(it != std::end(windows))
	{
		focused_window = it->get();
	}

	return focused_window;
}

void renderer::register_window(std::unique_ptr<render_window> window)
{
	windows_pending_addition_.emplace_back(std::move(window));
}

const std::vector<std::unique_ptr<render_window>>& renderer::get_windows() const
{
	return windows_;
}

const std::unique_ptr<render_window>& renderer::get_window(uint32_t id) const
{
	auto it = std::find_if(std::begin(windows_), std::end(windows_),
						   [id](const auto& window) { return window->get_id() == id; });

	ensures(it != std::end(windows_));

	return *it;
}

const std::unique_ptr<render_window>& renderer::get_main_window() const
{
	expects(!windows_.empty());

	return windows_.front();
}

void renderer::hide_all_secondary_windows()
{
	std::size_t i = 0;
	for(auto& window : windows_)
	{
		if(i++ != 0)
		{
			window->set_visible(false);
		}
	}
}

void renderer::show_all_secondary_windows()
{
	std::size_t i = 0;
	for(auto& window : windows_)
	{
		if(i++ != 0)
		{
			window->set_visible(true);
		}
	}
}

void renderer::process_pending_windows()
{
	std::move(std::begin(windows_pending_addition_), std::end(windows_pending_addition_),
			  std::back_inserter(windows_));
	windows_pending_addition_.clear();
}

void renderer::platform_events(const std::pair<std::uint32_t, bool>& info,
							   const std::vector<mml::platform_event>& events)
{
	for(const auto& e : events)
	{
		if(e.type == mml::platform_event::closed)
		{
			windows_.erase(std::remove_if(std::begin(windows_), std::end(windows_),
										  [window_id = info.first](const auto& window) {
											  return window->get_id() == window_id;
										  }),
						   std::end(windows_));
			return;
		}
	}
}

bool renderer::init_backend(cmd_line::parser& parser)
{

	mml::video_mode desktop = mml::video_mode::get_desktop_mode();
	desktop.width = 100;
	desktop.height = 100;
	init_window_ = std::make_unique<mml::window>(desktop, "App", mml::style::none);
	init_window_->set_visible(false);
	const auto sz = init_window_->get_size();

	gfx::platform_data pd;
	pd.ndt = init_window_->native_display_handle();
	pd.nwh = reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(init_window_->native_handle()));

	gfx::set_platform_data(pd);

	// auto detect
	auto preferred_renderer_type = gfx::renderer_type::Count;

	std::string preferred_renderer;
	if(parser.try_get("renderer", preferred_renderer))
	{
		if(preferred_renderer == "opengl")
		{
			preferred_renderer_type = gfx::renderer_type::OpenGL;
		}
		else if(preferred_renderer == "vulkan")
		{
			preferred_renderer_type = gfx::renderer_type::Vulkan;
		}
		else if(preferred_renderer == "directx11")
		{
			preferred_renderer_type = gfx::renderer_type::Direct3D11;
		}
		else if(preferred_renderer == "directx12")
		{
			preferred_renderer_type = gfx::renderer_type::Direct3D12;
		}
	}

	gfx::init_type init_data;
	init_data.type = preferred_renderer_type;
	init_data.resolution.width = sz[0];
	init_data.resolution.height = sz[1];
	init_data.resolution.reset = BGFX_RESET_VSYNC;

	bool novsync = false;
	parser.try_get("novsync", novsync);
	if(novsync)
	{
		init_data.resolution.reset = 0;
	}
	if(!gfx::init(init_data))
	{
		APPLOG_ERROR("Could not initialize rendering backend!");
		return false;
	}
	if(gfx::get_renderer_type() == gfx::renderer_type::Direct3D9)
	{
		APPLOG_ERROR("Does not support dx9. Minimum supported is dx11.");
		return false;
	}

	APPLOG_INFO("Using {0} rendering backend.", gfx::get_renderer_name(gfx::get_renderer_type()));

	if(gfx::get_renderer_type() == gfx::renderer_type::Direct3D12)
	{
		APPLOG_WARNING("Directx 12 support is experimental and unstable.");
	}

	return true;
}

void renderer::frame_end(delta_t /*unused*/)
{
	gfx::render_pass pass("init_bb_update");
	pass.bind();
	pass.clear();

	render_frame_ = gfx::frame();

	gfx::render_pass::reset();
}
} // namespace runtime
