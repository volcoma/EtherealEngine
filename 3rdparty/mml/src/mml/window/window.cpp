////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/window.hpp>
#include <mml/window/window_impl.hpp>
#include <mml/system/err.hpp>
#include <cstring>

namespace
{
    const mml::window* fullscreenWindow = NULL;
}


namespace mml
{
////////////////////////////////////////////////////////////
window::window() :
_impl          (NULL),
_size          ({0, 0})
{

}


////////////////////////////////////////////////////////////
window::window(video_mode mode, const std::string& title, std::uint32_t style) :
_impl          (NULL),
_size          ({0, 0})
{
    create(mode, title, style);
}


////////////////////////////////////////////////////////////
window::window(window_handle handle) :
_impl          (NULL),
_size          ({0, 0})
{
    create(handle);
}


////////////////////////////////////////////////////////////
window::~window()
{
    dispose();
}


////////////////////////////////////////////////////////////
void window::create(video_mode mode, const std::string& title, std::uint32_t style)
{
    // Destroy the previous window implementation
    dispose();

    // Fullscreen style requires some tests
    if (style & style::Fullscreen)
    {
        // Make sure there's not already a fullscreen window (only one is allowed)
        if (fullscreenWindow)
        {
            err() << "Creating two fullscreen windows is not allowed, switching to windowed mode" << std::endl;
            style &= ~style::Fullscreen;
        }
        else
        {
            // Make sure that the chosen video mode is compatible
            if (!mode.is_valid())
            {
                err() << "The requested video mode is not available, switching to a valid mode" << std::endl;
                mode = video_mode::get_fullscreen_modes()[0];
            }

            // Update the fullscreen window
            fullscreenWindow = this;
        }
    }

    // Check validity of style according to the underlying platform
    #if defined(MML_SYSTEM_IOS) || defined(MML_SYSTEM_ANDROID)
        if (style & style::Fullscreen)
            style &= ~style::Titlebar;
        else
            style |= style::Titlebar;
    #else
        if ((style & style::Close) || (style & style::Resize))
            style |= style::Titlebar;
    #endif

    // Recreate the window implementation
    _impl = priv::window_impl::create(mode, title, style);

    // Perform common initializations
    initialize();
}


////////////////////////////////////////////////////////////
void window::create(window_handle handle)
{
    // Destroy the previous window implementation
    dispose();

    // Recreate the window implementation
    _impl = priv::window_impl::create(handle);

    // Perform common initializations
    initialize();
}


////////////////////////////////////////////////////////////
void window::dispose()
{
    // Delete the window implementation
    delete _impl;
    _impl = NULL;

    // Update the fullscreen window
    if (this == fullscreenWindow)
        fullscreenWindow = NULL;
}

////////////////////////////////////////////////////////////
void window::request_close()
{
	if (_impl)
	{
		platform_event e;
		std::memset(&e, 0, sizeof(e));
		e.type = platform_event::closed;
		_impl->push_event(e);
	}
}
////////////////////////////////////////////////////////////
bool window::is_open() const
{
    return _impl != NULL;
}

////////////////////////////////////////////////////////////
bool window::poll_event(platform_event& event)
{
    if (_impl && _impl->pop_event(event, false))
    {
        return filter_event(event);
    }
    else
    {
        return false;
    }
}


////////////////////////////////////////////////////////////
bool window::wait_event(platform_event& event)
{
    if (_impl && _impl->pop_event(event, true))
    {
        return filter_event(event);
    }
    else
    {
        return false;
    }
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> window::get_position() const
{
    return _impl ? _impl->get_position() : std::array<std::int32_t, 2>();
}


////////////////////////////////////////////////////////////
void window::set_position(const std::array<std::int32_t, 2>& position)
{
    if (_impl)
        _impl->set_position(position);
}


////////////////////////////////////////////////////////////
std::array<std::uint32_t, 2> window::get_size() const
{
    return _size;
}


////////////////////////////////////////////////////////////
void window::set_size(const std::array<std::uint32_t, 2>& size)
{
    if (_impl)
    {
        _impl->set_size(size);

        // Cache the new size
        _size = size;

        // Notify the derived class
        on_resize();
    }
}


////////////////////////////////////////////////////////////
void window::set_title(const std::string& title)
{
    if (_impl)
        _impl->set_title(title);
}


////////////////////////////////////////////////////////////
void window::set_icon(unsigned int width, unsigned int height, const std::uint8_t* pixels)
{
    if (_impl)
        _impl->set_icon(width, height, pixels);
}


////////////////////////////////////////////////////////////
void window::set_visible(bool visible)
{
    if (_impl)
        _impl->set_visible(visible);
}

////////////////////////////////////////////////////////////
void window::maximize()
{
	if (_impl)
		_impl->maximize();
}

////////////////////////////////////////////////////////////
void window::minimize()
{
	if (_impl)
		_impl->minimize();
}

////////////////////////////////////////////////////////////
void window::restore()
{
	if (_impl)
		_impl->restore();
}

////////////////////////////////////////////////////////////
void window::set_alpha(float alpha)
{
	if (_impl)
		_impl->set_alpha(alpha);
}

////////////////////////////////////////////////////////////
void window::set_mouse_cursor_visible(bool visible)
{
    if (_impl)
        _impl->set_mouse_cursor_visible(visible);
}


////////////////////////////////////////////////////////////
void window::set_mouse_cursor_grabbed(bool grabbed)
{
    if (_impl)
        _impl->set_mouse_cursor_grabbed(grabbed);
}


////////////////////////////////////////////////////////////
void window::set_mouse_cursor(const cursor& cursor)
{
    if (_impl)
        _impl->set_mouse_cursor(cursor.get_impl());
}


////////////////////////////////////////////////////////////
void window::set_key_repeat_enabled(bool enabled)
{
    if (_impl)
        _impl->set_key_repeat_enabled(enabled);
}

////////////////////////////////////////////////////////////
void window::set_joystick_threshold(float threshold)
{
    if (_impl)
        _impl->set_joystick_threshold(threshold);
}


////////////////////////////////////////////////////////////
void window::request_focus()
{
    if (_impl)
        _impl->request_focus();
}


////////////////////////////////////////////////////////////
bool window::has_focus() const
{
    return _impl && _impl->has_focus();
}


////////////////////////////////////////////////////////////
window_handle window::get_system_handle() const
{
    return _impl ? _impl->get_system_handle() : 0;
}


////////////////////////////////////////////////////////////
void window::on_create()
{
    // Nothing by default
}


////////////////////////////////////////////////////////////
void window::on_resize()
{
    // Nothing by default
}


////////////////////////////////////////////////////////////
bool window::filter_event(const platform_event& event)
{
    // Notify resize events to the derived class
    if (event.type == platform_event::resized)
    {
        // Cache the new size
		_size = { event.size.width, event.size.height };

        // Notify the derived class
        on_resize();
    }

    return true;
}


////////////////////////////////////////////////////////////
void window::initialize()
{
    // Setup default behaviors (to get a consistent behavior across different implementations)
    set_visible(true);
    set_mouse_cursor_visible(true);
    set_key_repeat_enabled(true);

    // Get and cache the initial size of the window
    _size = _impl->get_size();

    // Notify the derived class
    on_create();
}

} // namespace mml
