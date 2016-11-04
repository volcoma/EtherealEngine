////////////////////////////////////////////////////////////
//
// SFML - Simple and Fast Multimedia Library
// Copyright (C) 2007-2015 Laurent Gomila (laurent@sfml-dev.org)
//
// This software is provided 'as-is', without any express or implied warranty.
// In no event will the authors be held liable for any damages arising from the use of this software.
//
// Permission is granted to anyone to use this software for any purpose,
// including commercial applications, and to alter it and redistribute it freely,
// subject to the following restrictions:
//
// 1. The origin of this software must not be misrepresented;
//    you must not claim that you wrote the original software.
//    If you use this software in a product, an acknowledgment
//    in the product documentation would be appreciated but is not required.
//
// 2. Altered source versions must be plainly marked as such,
//    and must not be misrepresented as being the original software.
//
// 3. This notice may not be removed or altered from any source distribution.
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "Window.hpp"
#include "WindowImpl.hpp"
#include "../System/Err.hpp"


namespace
{
    const sf::Window* fullscreenWindow = nullptr;
}


namespace sf
{
////////////////////////////////////////////////////////////
Window::Window() :
m_impl          (nullptr),
m_size          (0, 0)
{

}


////////////////////////////////////////////////////////////
Window::Window(VideoMode mode, const std::string& title, std::uint32_t style) :
m_impl          (nullptr),
m_size          (0, 0)
{
    create(mode, title, style);
}


////////////////////////////////////////////////////////////
Window::Window(WindowHandle handle) :
m_impl          (nullptr),
m_size          (0, 0)
{
    create(handle);
}


////////////////////////////////////////////////////////////
Window::~Window()
{
    close();
}


////////////////////////////////////////////////////////////
void Window::create(VideoMode mode, const std::string& title, std::uint32_t style)
{
    // Destroy the previous window implementation
    close();

    // Fullscreen style requires some tests
    if (style & Style::Fullscreen)
    {
        // Make sure there's not already a fullscreen window (only one is allowed)
        if (fullscreenWindow)
        {
            err() << "Creating two fullscreen windows is not allowed, switching to windowed mode" << std::endl;
            style &= ~Style::Fullscreen;
        }
        else
        {
            // Make sure that the chosen video mode is compatible
            if (!mode.isValid())
            {
                err() << "The requested video mode is not available, switching to a valid mode" << std::endl;
                mode = VideoMode::getFullscreenModes()[0];
            }

            // Update the fullscreen window
            fullscreenWindow = this;
        }
    }

    // Check validity of style according to the underlying platform
    #if defined(SFML_SYSTEM_IOS) || defined(SFML_SYSTEM_ANDROID)
        if (style & Style::Fullscreen)
            style &= ~Style::Titlebar;
        else
            style |= Style::Titlebar;
    #else
        if ((style & Style::Close) || (style & Style::Resize))
            style |= Style::Titlebar;
    #endif

    // Recreate the window implementation
    m_impl = priv::WindowImpl::create(mode, title, style);

    // Perform common initializations
    initialize();
}


////////////////////////////////////////////////////////////
void Window::create(WindowHandle handle)
{
    // Destroy the previous window implementation
    close();

    // Recreate the window implementation
    m_impl = priv::WindowImpl::create(handle);

    // Perform common initializations
    initialize();
}


////////////////////////////////////////////////////////////
void Window::close()
{	
	onClose();
    // Delete the window implementation
    delete m_impl;
    m_impl = NULL;

    // Update the fullscreen window
    if (this == fullscreenWindow)
        fullscreenWindow = NULL;	
}


////////////////////////////////////////////////////////////
bool Window::isOpen() const
{
    return m_impl != NULL;
}

////////////////////////////////////////////////////////////
bool Window::pollEvent(Event& event)
{
    if (m_impl && m_impl->popEvent(event, false))
    {
        return filterEvent(event);
    }
    else
    {
        return false;
    }
}


////////////////////////////////////////////////////////////
bool Window::waitEvent(Event& event)
{
    if (m_impl && m_impl->popEvent(event, true))
    {
        return filterEvent(event);
    }
    else
    {
        return false;
    }
}


////////////////////////////////////////////////////////////
iPoint Window::getPosition() const
{
    return m_impl ? m_impl->getPosition() : iPoint();
}


////////////////////////////////////////////////////////////
void Window::setPosition(const iPoint& position)
{
    if (m_impl)
        m_impl->setPosition(position);
}


////////////////////////////////////////////////////////////
uSize Window::getSize() const
{
    return m_size;
}


////////////////////////////////////////////////////////////
void Window::setSize(const uSize& size)
{
    if (m_impl)
    {
        m_impl->setSize(size);

        // Cache the new size
        m_size = size;

        // Notify the derived class
        onResize();
    }
}


////////////////////////////////////////////////////////////
void Window::setTitle(const std::string& title)
{
    if (m_impl)
        m_impl->setTitle(title);
}


////////////////////////////////////////////////////////////
void Window::setIcon(unsigned int width, unsigned int height, const std::uint8_t* pixels)
{
    if (m_impl)
        m_impl->setIcon(width, height, pixels);
}


////////////////////////////////////////////////////////////
void Window::setVisible(bool visible)
{
    if (m_impl)
        m_impl->setVisible(visible);
}
void Window::maximize()
{
	if (m_impl)
		m_impl->maximize();
}
void Window::minimize()
{
	if (m_impl)
		m_impl->minimize();
}

void Window::setAlpha(float alpha01)
{
	if (m_impl)
		m_impl->setAlpha(alpha01);
}
////////////////////////////////////////////////////////////
void Window::setMouseCursorVisible(bool visible)
{
    if (m_impl)
        m_impl->setMouseCursorVisible(visible);
}

////////////////////////////////////////////////////////////
void Window::setMouseCursorGrabbed(bool grabbed)
{
	if (m_impl)
		m_impl->setMouseCursorGrabbed(grabbed);
}

////////////////////////////////////////////////////////////
void Window::setMouseCursor(Cursor cursor)
{
	if (m_impl)
		m_impl->setMouseCursor(cursor);
}


////////////////////////////////////////////////////////////
void Window::setMouseCursor(const std::uint8_t* pixels, unsigned int width, unsigned int height, unsigned int hotspotX, unsigned int hotspotY)
{
	if (m_impl && pixels && width && height)
		m_impl->setMouseCursor(pixels, width, height, hotspotX, hotspotY);
}


////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
void Window::setKeyRepeatEnabled(bool enabled)
{
    if (m_impl)
        m_impl->setKeyRepeatEnabled(enabled);
}

////////////////////////////////////////////////////////////
void Window::setJoystickThreshold(float threshold)
{
    if (m_impl)
        m_impl->setJoystickThreshold(threshold);
}


////////////////////////////////////////////////////////////
void Window::requestFocus()
{
    if (m_impl)
        m_impl->requestFocus();
}


////////////////////////////////////////////////////////////
bool Window::hasFocus() const
{
    return m_impl && m_impl->hasFocus();
}


////////////////////////////////////////////////////////////
WindowHandle Window::getSystemHandle() const
{
    return m_impl ? m_impl->getSystemHandle() : 0;
}

////////////////////////////////////////////////////////////
void Window::onResize()
{
    // Nothing by default
}

////////////////////////////////////////////////////////////
void Window::onClose()
{
	// Nothing by default
}
////////////////////////////////////////////////////////////
bool Window::filterEvent(const Event& event)
{
    // Notify resize events to the derived class
    if (event.type == Event::Resized)
    {
        // Cache the new size
        m_size.width = event.size.width;
        m_size.height = event.size.height;

        // Notify the derived class
        onResize();
    }

	if (event.type == Event::Closed)
	{
		// Notify the derived class
		close();
	}

    return true;
}


////////////////////////////////////////////////////////////
void Window::initialize()
{
    // Setup default behaviors (to get a consistent behavior across different implementations)
    setVisible(true);
    setMouseCursorVisible(true);
    setKeyRepeatEnabled(true);

    // Get and cache the initial size of the window
    m_size = m_impl->getSize();
}

} // namespace sf
