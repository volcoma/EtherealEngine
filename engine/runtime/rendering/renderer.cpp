#include "renderer.h"
#include "../system/events.h"
#include "core/common/assert.hpp"
#include "core/graphics/graphics.h"
#include "core/graphics/render_pass.h"
#include "core/logging/logging.h"
#include "core/string_utils/string_utils.h"
#include <cstdarg>

namespace runtime
{
bool renderer::initialize()
{
	on_platform_events.connect(this, &renderer::platform_events);
	on_frame_end.connect(this, &renderer::frame_end);

	if(!init_backend())
	{
		APPLOG_ERROR("Could not initialize rendering backend!");
		return false;
	}

	mml::video_mode desktop = mml::video_mode::get_desktop_mode();
	desktop.width = 1280;
	desktop.height = 720;
	auto window = std::make_unique<render_window>(desktop, "App", mml::style::standard);
	window->request_focus();
	register_window(std::move(window));
	process_pending_windows();

	return true;
}

void renderer::dispose()
{
	on_platform_events.disconnect(this, &renderer::platform_events);
	on_frame_end.disconnect(this, &renderer::frame_end);
	_windows.clear();
	_windows_pending_addition.clear();
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
	_windows_pending_addition.emplace_back(std::move(window));
}

const std::vector<std::unique_ptr<render_window>>& renderer::get_windows() const
{
	return _windows;
}

const std::unique_ptr<render_window>& renderer::get_window(uint32_t id) const
{
	auto it = std::find_if(std::begin(_windows), std::end(_windows),
						   [id](const auto& window) { return window->get_id() == id; });

	ensures(it != std::end(_windows));

	return *it;
}

const std::unique_ptr<render_window>& renderer::get_main_window() const
{
	expects(_windows.size() > 0);

	return _windows.front();
}

void renderer::hide_all_secondary_windows()
{
	std::size_t i = 0;
	for(auto& window : _windows)
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
	for(auto& window : _windows)
	{
		if(i++ != 0)
		{
			window->set_visible(true);
		}
	}
}

void renderer::process_pending_windows()
{
	for(auto& window : _windows_pending_addition)
	{
		_windows.emplace_back(std::move(window));
	}
	_windows_pending_addition.clear();
}

void renderer::platform_events(const std::pair<std::uint32_t, bool>& info,
							   const std::vector<mml::platform_event>& events)
{
	for(const auto& e : events)
	{
		if(e.type == mml::platform_event::closed)
		{
			_windows.erase(std::remove_if(std::begin(_windows), std::end(_windows),
										  [window_id = info.first](const auto& window) {
											  return window->get_id() == window_id;
										  }),
						   std::end(_windows));
			return;
		}
	}
}

bool renderer::init_backend()
{

	mml::video_mode desktop = mml::video_mode::get_desktop_mode();
	desktop.width = 100;
	desktop.height = 100;
	_init_window = std::make_unique<mml::window>(desktop, "App", mml::style::none);
	_init_window->set_visible(false);
	gfx::platform_data pd{
		reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(_init_window->get_system_handle_specific())),
		reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(_init_window->get_system_handle())),
		nullptr,
		nullptr,
		nullptr,
		nullptr,
	};

	gfx::set_platform_data(pd);

	// auto detect
	const auto preferred_renderer_type = gfx::renderer_type::Count;
	if(!gfx::init(preferred_renderer_type))
		return false;

	if(gfx::get_renderer_type() == gfx::renderer_type::Direct3D9)
	{
		APPLOG_ERROR("Does not support dx9. Minimum supported is dx11.");
		return false;
	}
	const auto sz = _init_window->get_size();
	gfx::reset(sz[0], sz[1], BGFX_RESET_VSYNC);

	return true;
}

void renderer::frame_end(std::chrono::duration<float>)
{
	gfx::render_pass pass("init_bb_update");
	pass.bind();
	pass.clear();

	_render_frame = gfx::frame();

	gfx::render_pass::reset();
}
}
