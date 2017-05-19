////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "render_window.h"
#include "render_pass.h"

void render_window::onResize()
{
	prepare_surface();
	auto size = getSize();
	on_resized(*this, size);
}

void render_window::onClose()
{
	on_closed(*this);
}

render_window::render_window()
{
	_surface = std::make_shared<frame_buffer>();
}

render_window::render_window(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/) : sf::Window(mode, title, style)
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
	render_pass pass("RenderWindowPass");
	pass.bind(_surface.get());
	pass.clear();
}

void render_window::prepare_surface()
{
	if (!gfx::is_initted())
		return;

	auto size = getSize();
	if (_is_main)
	{		
		gfx::reset(size.width, size.height, 0);
	}
	else
	{
		_surface->populate(getSystemHandle(), size.width, size.height);
	}

}