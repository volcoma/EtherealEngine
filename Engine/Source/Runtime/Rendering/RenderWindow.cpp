////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "RenderWindow.h"
#include "RenderView.h"

void RenderWindow::onResize()
{
	prepareView();
	auto size = getSize();
	onResized(*this, size);
}

void RenderWindow::onClose()
{
	onClosed(*this);
}

RenderWindow::RenderWindow()
{
	mView = std::make_shared<RenderView>();
}

RenderWindow::RenderWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/) : sf::Window(mode, title, style)
{
	mView = std::make_shared<RenderView>();
}

RenderWindow::~RenderWindow()
{
}

bool RenderWindow::filterEvent(const sf::Event& event)
{
	mInput.handleEvent(event);
	return sf::Window::filterEvent(event);;
}

void RenderWindow::frameBegin()
{
	auto view = getRenderView();
	RenderView::pushView(view);
	view->clear();
	mInput.update();
}

void RenderWindow::frameUpdate(float dt)
{
}

void RenderWindow::frameEnd()
{
	RenderView::popView();
}

void RenderWindow::frameRender()
{
}

void RenderWindow::prepareView()
{
	auto size = getSize();
	auto& surface = mView->getRenderSurface();
	surface.populate(mView->getId(), getSystemHandle(), size.width, size.height);
}

void RenderWindow::destroyView()
{
	mView.reset();
}

void RenderWindow::delayedClose()
{
	destroyView();
	onClose();
}
