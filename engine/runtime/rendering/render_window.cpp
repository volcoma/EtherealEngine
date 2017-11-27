////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "render_window.h"
#include "core/logging/logging.h"
#include "core/graphics/render_pass.h"
static std::uint32_t s_next_id = 0;

void render_window::on_resize()
{
	prepare_surface();
}

render_window::render_window()
{
	_id = s_next_id++;
	_surface = std::make_shared<frame_buffer>();
	prepare_surface();
}

render_window::render_window(mml::video_mode mode, const std::string& title,
							 std::uint32_t style /*= mml::style::default*/)
	: mml::window(mode, title, style)
{
	_id = s_next_id++;
	_surface = std::make_shared<frame_buffer>();
	prepare_surface();
}

render_window::~render_window()
{
	destroy_surface();
}

std::shared_ptr<frame_buffer> render_window::get_surface()
{
	return _surface;
}

std::shared_ptr<frame_buffer> render_window::get_surface() const
{
	return _surface;
}

void render_window::destroy_surface()
{
	// force internal handle destruction
	if(_surface)
	{
		_surface->dispose();
		_surface.reset();

		// this is a must to flush the destruction
		// of the frame buffer
		gfx::frame();
		gfx::frame();
	}
}

std::uint8_t render_window::begin_present_pass()
{
	render_pass pass("present_to_window_pass");
	pass.bind(_surface.get());
	pass.clear();
	return pass.id;
}

uint32_t render_window::get_id() const
{
	return _id;
}

void render_window::prepare_surface()
{
	auto size = get_size();

	_surface->populate(reinterpret_cast<void*>(reinterpret_cast<uintptr_t>(get_system_handle())),
					   static_cast<std::uint16_t>(size[0]), static_cast<std::uint16_t>(size[1]));
}
