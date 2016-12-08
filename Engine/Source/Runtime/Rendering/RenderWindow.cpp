////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "RenderWindow.h"
#include "RenderPass.h"

void RenderWindow::onResize()
{
	prepareSurface();
	auto size = getSize();
	onResized(*this, size);
}

void RenderWindow::onClose()
{
	onClosed(*this);
}

RenderWindow::RenderWindow()
{
	mSurface = std::make_shared<FrameBuffer>();
}

RenderWindow::RenderWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/) : sf::Window(mode, title, style)
{
	mSurface = std::make_shared<FrameBuffer>();
}

RenderWindow::~RenderWindow()
{
	// force internal handle destruction
	mSurface->dispose();
	mSurface.reset();
	gfx::frame();
	gfx::frame();
}

bool RenderWindow::filterEvent(const sf::Event& event)
{
	mInput.handleEvent(event);
	return sf::Window::filterEvent(event);;
}

void RenderWindow::frameBegin()
{

	mInput.update();
}

void RenderWindow::frameUpdate(float dt)
{
}

void RenderWindow::frameEnd()
{
	RenderPass pass("RenderWindowPass");
	pass.bind(mSurface.get());
	pass.clear();
}

void RenderWindow::frameRender()
{
}

void RenderWindow::prepareSurface()
{
	auto size = getSize();
	if (mIsMain)
	{
		gfx::reset(size.width, size.height, 0);
	}
	else
	{
		mSurface->populate(getSystemHandle(), size.width, size.height);
	}

}