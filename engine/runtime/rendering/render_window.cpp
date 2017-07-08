////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "render_window.h"
#include "render_pass.h"

void render_window::on_resize()
{
	prepare_surface();
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
	destroy_surface();
}

void render_window::destroy_surface()
{
	// force internal handle destruction
	if (_surface)
	{
		_surface->dispose();
		_surface.reset();

		//this is a must to flush the destruction
		//of the frame buffer
		gfx::frame();
		gfx::frame();
	}
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
        _surface->populate((void*)(uintptr_t)get_system_handle(), size[0], size[1]);
	}

}
