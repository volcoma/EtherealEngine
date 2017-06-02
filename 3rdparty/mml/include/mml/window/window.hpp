#ifndef MML_WINDOW_HPP
#define MML_WINDOW_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/export.hpp>
#include <mml/window/cursor.hpp>
#include <mml/window/video_mode.hpp>
#include <mml/window/window_handle.hpp>
#include <mml/window/window_style.hpp>
#include <mml/window/event.hpp>
#include <mml/system/non_copyable.hpp>
#include <string>
#include <array>

namespace mml
{
namespace priv
{
    class window_impl;
}

class platform_event;

////////////////////////////////////////////////////////////
/// \brief window that serves as a target for OpenGL rendering
///
////////////////////////////////////////////////////////////
class MML_WINDOW_API window : non_copyable
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// This constructor doesn't actually create the window,
    /// use the other constructors or call create() to do so.
    ///
    ////////////////////////////////////////////////////////////
    window();

    ////////////////////////////////////////////////////////////
    /// \brief Construct a new window
    ///
    /// This constructor creates the window with the size and pixel
    /// depth defined in \a mode. An optional style can be passed to
    /// customize the look and behavior of the window (borders,
    /// title bar, resizable, closable, ...). If \a style contains
    /// style::Fullscreen, then \a mode must be a valid video mode.
    ///
    /// The fourth parameter is an optional structure specifying
    /// advanced OpenGL context settings such as antialiasing,
    /// depth-buffer bits, etc.
    ///
    /// \param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)
    /// \param title    Title of the window
    /// \param style    %window style, a bitwise OR combination of mml::style enumerators
    ///
    ////////////////////////////////////////////////////////////
    window(video_mode mode, const std::string& title, std::uint32_t style = style::Default);

    ////////////////////////////////////////////////////////////
    /// \brief Construct the window from an existing control
    ///
    /// Use this constructor if you want to create an OpenGL
    /// rendering area into an already existing control.
    ///
    /// The second parameter is an optional structure specifying
    /// advanced OpenGL context settings such as antialiasing,
    /// depth-buffer bits, etc.
    ///
    /// \param handle   Platform-specific handle of the control
    ///
    ////////////////////////////////////////////////////////////
    explicit window(window_handle handle);

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    /// Closes the window and frees all the resources attached to it.
    ///
    ////////////////////////////////////////////////////////////
    virtual ~window();

    ////////////////////////////////////////////////////////////
    /// \brief Create (or recreate) the window
    ///
    /// If the window was already created, it closes it first.
    /// If \a style contains style::Fullscreen, then \a mode
    /// must be a valid video mode.
    ///
    /// The fourth parameter is an optional structure specifying
    /// advanced OpenGL context settings such as antialiasing,
    /// depth-buffer bits, etc.
    ///
    /// \param mode     Video mode to use (defines the width, height and depth of the rendering area of the window)
    /// \param title    Title of the window
    /// \param style    %window style, a bitwise OR combination of mml::style enumerators
    /// \param settings Additional settings for the underlying OpenGL context
    ///
    ////////////////////////////////////////////////////////////
    void create(video_mode mode, const std::string& title, std::uint32_t style = style::Default);

    ////////////////////////////////////////////////////////////
    /// \brief Create (or recreate) the window from an existing control
    ///
    /// Use this function if you want to create an OpenGL
    /// rendering area into an already existing control.
    /// If the window was already created, it closes it first.
    ///
    /// The second parameter is an optional structure specifying
    /// advanced OpenGL context settings such as antialiasing,
    /// depth-buffer bits, etc.
    ///
    /// \param handle   Platform-specific handle of the control
    /// \param settings Additional settings for the underlying OpenGL context
    ///
    ////////////////////////////////////////////////////////////
    void create(window_handle handle);

    ////////////////////////////////////////////////////////////
    /// \brief Dispose the window and destroy all the attached resources
    ///
    /// After calling this function, the mml::window instance remains
    /// valid and you can call create() to recreate the window.
    /// All other functions such as poll_event() or display() will
    /// still work (i.e. you don't have to test is_open() every time),
    /// and will have no effect on disposed windows.
    ///
    ////////////////////////////////////////////////////////////
    void dispose();

	////////////////////////////////////////////////////////////
	/// \brief  Request the current window to be closed
	///
	/// 
	/// 
	////////////////////////////////////////////////////////////
	void request_close();

    ////////////////////////////////////////////////////////////
    /// \brief Tell whether or not the window is open
    ///
    /// This function returns whether or not the window exists.
    /// Note that a hidden window (set_visible(false)) is open
    /// (therefore this function would return true).
    ///
    /// \return True if the window is open, false if it has been closed
    ///
    ////////////////////////////////////////////////////////////
    bool is_open() const;

    ////////////////////////////////////////////////////////////
    /// \brief Pop the event on top of the event queue, if any, and return it
    ///
    /// This function is not blocking: if there's no pending event then
    /// it will return false and leave \a event unmodified.
    /// Note that more than one event may be present in the event queue,
    /// thus you should always call this function in a loop
    /// to make sure that you process every pending event.
    /// \code
    /// mml::platform_event event;
    /// while (window.poll_event(event))
    /// {
    ///    // process event...
    /// }
    /// \endcode
    ///
    /// \param event platform_event to be returned
    ///
    /// \return True if an event was returned, or false if the event queue was empty
    ///
    /// \see wait_event
    ///
    ////////////////////////////////////////////////////////////
    bool poll_event(platform_event& event);

    ////////////////////////////////////////////////////////////
    /// \brief wait for an event and return it
    ///
    /// This function is blocking: if there's no pending event then
    /// it will wait until an event is received.
    /// After this function returns (and no error occurred),
    /// the \a event object is always valid and filled properly.
    /// This function is typically used when you have a thread that
    /// is dedicated to events handling: you want to make this thread
    /// sleep as long as no new event is received.
    /// \code
    /// mml::platform_event event;
    /// if (window.wait_event(event))
    /// {
    ///    // process event...
    /// }
    /// \endcode
    ///
    /// \param event platform_event to be returned
    ///
    /// \return False if any error occurred
    ///
    /// \see poll_event
    ///
    ////////////////////////////////////////////////////////////
    bool wait_event(platform_event& event);

    ////////////////////////////////////////////////////////////
    /// \brief Get the position of the window
    ///
    /// \return Position of the window, in pixels
    ///
    /// \see set_position
    ///
    ////////////////////////////////////////////////////////////
    std::array<std::int32_t, 2> get_position() const;

    ////////////////////////////////////////////////////////////
    /// \brief Change the position of the window on screen
    ///
    /// This function only works for top-level windows
    /// (i.e. it will be ignored for windows created from
    /// the handle of a child window/control).
    ///
    /// \param position New position, in pixels
    ///
    /// \see get_position
    ///
    ////////////////////////////////////////////////////////////
    void set_position(const std::array<std::int32_t, 2>& position);

    ////////////////////////////////////////////////////////////
    /// \brief Get the size of the rendering region of the window
    ///
    /// The size doesn't include the titlebar and borders
    /// of the window.
    ///
    /// \return Size in pixels
    ///
    /// \see set_size
    ///
    ////////////////////////////////////////////////////////////
    std::array<std::uint32_t, 2> get_size() const;

    ////////////////////////////////////////////////////////////
    /// \brief Change the size of the rendering region of the window
    ///
    /// \param size New size, in pixels
    ///
    /// \see get_size
    ///
    ////////////////////////////////////////////////////////////
    void set_size(const std::array<std::uint32_t, 2>& size);

    ////////////////////////////////////////////////////////////
    /// \brief Change the title of the window
    ///
    /// \param title New title
    ///
    /// \see set_icon
    ///
    ////////////////////////////////////////////////////////////
    void set_title(const std::string& title);

    ////////////////////////////////////////////////////////////
    /// \brief Change the window's icon
    ///
    /// \a pixels must be an array of \a width x \a height pixels
    /// in 32-bits RGBA format.
    ///
    /// The OS default icon is used by default.
    ///
    /// \param width  Icon's width, in pixels
    /// \param height Icon's height, in pixels
    /// \param pixels Pointer to the array of pixels in memory. The
    ///               pixels are copied, so you need not keep the
    ///               source alive after calling this function.
    ///
    /// \see set_title
    ///
    ////////////////////////////////////////////////////////////
    void set_icon(unsigned int width, unsigned int height, const std::uint8_t* pixels);

    ////////////////////////////////////////////////////////////
    /// \brief Show or hide the window
    ///
    /// The window is shown by default.
    ///
    /// \param visible True to show the window, false to hide it
    ///
    ////////////////////////////////////////////////////////////
    void set_visible(bool visible);

	////////////////////////////////////////////////////////////
	/// \brief Request to maxiize the window
	////////////////////////////////////////////////////////////
	void maximize();

	////////////////////////////////////////////////////////////
	/// \brief Request to minimize the window
	////////////////////////////////////////////////////////////
	void minimize();

	////////////////////////////////////////////////////////////
	/// \brief Request to restore the window
	////////////////////////////////////////////////////////////
	void restore();

	////////////////////////////////////////////////////////////
	/// \brief Request to change window transparency
	////////////////////////////////////////////////////////////
	void set_alpha(float alpha);
   
    ////////////////////////////////////////////////////////////
    /// \brief Show or hide the mouse cursor
    ///
    /// The mouse cursor is visible by default.
    ///
    /// \param visible True to show the mouse cursor, false to hide it
    ///
    ////////////////////////////////////////////////////////////
    void set_mouse_cursor_visible(bool visible);

    ////////////////////////////////////////////////////////////
    /// \brief Grab or release the mouse cursor
    ///
    /// If set, grabs the mouse cursor inside this window's client
    /// area so it may no longer be moved outside its bounds.
    /// Note that grabbing is only active while the window has
    /// focus.
    ///
    /// \param grabbed True to enable, false to disable
    ///
    ////////////////////////////////////////////////////////////
    void set_mouse_cursor_grabbed(bool grabbed);

    ////////////////////////////////////////////////////////////
    /// \brief Set the displayed cursor to a native system cursor
    ///
    /// Upon window creation, the arrow cursor is used by default.
    ///
    /// \warning The cursor must not be destroyed while in use by
    ///          the window.
    ///
    /// \warning Features related to cursor are not supported on
    ///          iOS and Android.
    ///
    /// \param cursor Native system cursor type to display
    ///
    /// \see mml::cursor::load_from_system
    /// \see mml::cursor::load_from_pixels
    ///
    ////////////////////////////////////////////////////////////
    void set_mouse_cursor(const cursor& cursor);

    ////////////////////////////////////////////////////////////
    /// \brief Enable or disable automatic key-repeat
    ///
    /// If key repeat is enabled, you will receive repeated
    /// key_pressed events while keeping a key pressed. If it is disabled,
    /// you will only get a single event when the key is pressed.
    ///
    /// key repeat is enabled by default.
    ///
    /// \param enabled True to enable, false to disable
    ///
    ////////////////////////////////////////////////////////////
    void set_key_repeat_enabled(bool enabled);

    ////////////////////////////////////////////////////////////
    /// \brief Change the joystick threshold
    ///
    /// The joystick threshold is the value below which
    /// no joystick_moved event will be generated.
    ///
    /// The threshold value is 0.1 by default.
    ///
    /// \param threshold New threshold, in the range [0, 100]
    ///
    ////////////////////////////////////////////////////////////
    void set_joystick_threshold(float threshold);

    ////////////////////////////////////////////////////////////
    /// \brief Request the current window to be made the active
    ///        foreground window
    ///
    /// At any given time, only one window may have the input focus
    /// to receive input events such as keystrokes or mouse events.
    /// If a window requests focus, it only hints to the operating
    /// system, that it would like to be focused. The operating system
    /// is free to deny the request.
    ///
    /// \see has_focus
    ///
    ////////////////////////////////////////////////////////////
    void request_focus();

    ////////////////////////////////////////////////////////////
    /// \brief Check whether the window has the input focus
    ///
    /// At any given time, only one window may have the input focus
    /// to receive input events such as keystrokes or most mouse
    /// events.
    ///
    /// \return True if window has focus, false otherwise
    /// \see request_focus
    ///
    ////////////////////////////////////////////////////////////
    bool has_focus() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the OS-specific handle of the window
    ///
    /// The type of the returned handle is mml::window_handle,
    /// which is a typedef to the handle type defined by the OS.
    /// You shouldn't need to use this function, unless you have
    /// very specific stuff to implement that mml doesn't support,
    /// or implement a temporary workaround until a bug is fixed.
    ///
    /// \return System handle of the window
    ///
    ////////////////////////////////////////////////////////////
    window_handle get_system_handle() const;

protected:

    ////////////////////////////////////////////////////////////
    /// \brief Function called after the window has been created
    ///
    /// This function is called so that derived classes can
    /// perform their own specific initialization as soon as
    /// the window is created.
    ///
    ////////////////////////////////////////////////////////////
    virtual void on_create();

    ////////////////////////////////////////////////////////////
    /// \brief Function called after the window has been resized
    ///
    /// This function is called so that derived classes can
    /// perform custom actions when the size of the window changes.
    ///
    ////////////////////////////////////////////////////////////
    virtual void on_resize();

    ////////////////////////////////////////////////////////////
    /// \brief Processes an event before it is sent to the user
    ///
    /// This function is called every time an event is received
    /// from the internal window (through poll_event or wait_event).
    /// It filters out unwanted events, and performs whatever internal
    /// stuff the window needs before the event is returned to the
    /// user.
    ///
    /// \param event platform_event to filter
    ///
    ////////////////////////////////////////////////////////////
    virtual bool filter_event(const platform_event& event);

private:
    ////////////////////////////////////////////////////////////
    /// \brief Perform some common internal initializations
    ///
    ////////////////////////////////////////////////////////////
    void initialize();

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    priv::window_impl* _impl;           ///< Platform-specific implementation of the window
    std::array<std::uint32_t, 2> _size;           ///< Current size of the window
};

} // namespace mml


#endif // MML_WINDOW_HPP


////////////////////////////////////////////////////////////
/// \class mml::window
/// \ingroup window
///
/// mml::window is the main class of the window module. It defines
/// an OS window that is able to receive an OpenGL rendering.
///
/// A mml::window can create its own new window, or be embedded into
/// an already existing control using the create(handle) function.
/// This can be useful for embedding an OpenGL rendering area into
/// a view which is part of a bigger GUI with existing windows,
/// controls, etc. It can also serve as embedding an OpenGL rendering
/// area into a window created by another (probably richer) GUI library
/// like Qt or wxWidgets.
///
/// The mml::window class provides a simple interface for manipulating
/// the window: move, resize, show/hide, control mouse cursor, etc.
/// It also provides event handling through its poll_event() and wait_event()
/// functions.
///
/// Note that OpenGL experts can pass their own parameters (antialiasing
/// level, bits for the depth and stencil buffers, etc.) to the
/// OpenGL context attached to the window, with the mml::ContextSettings
/// structure which is passed as an optional argument when creating the
/// window.
///
/// Usage example:
/// \code
/// // Declare and create a new window
/// mml::window window(mml::video_mode(800, 600), "mml window");
///
/// // Limit the framerate to 60 frames per second (this step is optional)
/// window.setFramerateLimit(60);
///
/// // The main loop - ends as soon as the window is closed
/// while (window.is_open())
/// {
///    // platform_event processing
///    mml::platform_event event;
///    while (window.poll_event(event))
///    {
///        // Request for closing the window
///        if (event.type == mml::platform_event::closed)
///            window.dispose();
///    }
///
///    // Activate the window for OpenGL rendering
///    window.setActive();
///
///    // OpenGL drawing commands go here...
///
///    // End the current frame and display its contents on screen
///    window.display();
/// }
/// \endcode
///
////////////////////////////////////////////////////////////
