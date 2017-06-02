////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "render_window.h"
#include "render_pass.h"

void render_window::on_resize()
{
	prepare_surface();
	auto size = get_size();
	on_resized(*this, { size[0], size[1] });
}

bool render_window::filter_event(const mml::platform_event& event)
{
	if (event.type == mml::platform_event::closed)
	{
		on_closed(*this);
		dispose();
	}

	return mml::window::filter_event(event);
}

render_window::render_window()
{
	_surface = std::make_shared<frame_buffer>();
}

render_window::render_window(mml::video_mode mode, const std::string& title, std::uint32_t style /*= mml::style::default*/) : mml::window(mode, title, style)
{
	_surface = std::make_shared<frame_buffer>();
}

render_window::~render_window()
{
	// force internal handle destruction
	_surface->dispose();
	_surface.reset();
}

void render_window::frame_end()
{
	render_pass pass("present_to_window_pass");
	pass.bind(_surface.get());
	pass.clear();
}

void render_window::prepare_surface()
{
	if (!gfx::is_initted())
		return;

	auto size = get_size();
	if (_is_main)
	{		
		gfx::reset(size[0], size[1], 0);
	}
	else
	{
		_surface->populate(get_system_handle(), size[0], size[1]);
	}

}