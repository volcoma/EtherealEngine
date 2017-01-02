////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "render_window.h"
#include "render_pass.h"

void RenderWindow::onResize()
{
	prepare_surface();
	auto size = getSize();
	on_resized(*this, size);
}

void RenderWindow::onClose()
{
	on_closed(*this);
}

RenderWindow::RenderWindow()
{
	_surface = std::make_shared<FrameBuffer>();
}

RenderWindow::RenderWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/) : sf::Window(mode, title, style)
{
	_surface = std::make_shared<FrameBuffer>();
}

RenderWindow::~RenderWindow()
{
	// force internal handle destruction
	_surface->dispose();
	_surface.reset();
	gfx::frame();
	gfx::frame();
}

void RenderWindow::frame_end()
{
	RenderPass pass("RenderWindowPass");
	pass.bind(_surface.get());
	pass.clear();
}

void RenderWindow::prepare_surface()
{
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