#ifndef MML_WINDOWIMPLX11_HPP
#define MML_WINDOWIMPLX11_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/event.hpp>
#include <mml/window/window_impl.hpp>
#include <mml/window/window_style.hpp>
#include <X11/Xlib.h>
#include <deque>


namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Linux (X11) implementation of WindowImpl
///
////////////////////////////////////////////////////////////
class window_impl_x11 : public window_impl
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Construct the window implementation from an existing control
    ///
    /// \param handle Platform-specific handle of the control
    ///
    ////////////////////////////////////////////////////////////
    window_impl_x11(window_handle handle);

    ////////////////////////////////////////////////////////////
    /// \brief Create the window implementation
    ///
    /// \param mode  Video mode to use
    /// \param title Title of the window
    /// \param style window style (resizable, fixed, or fullscren)
    /// \param settings Additional settings for the underlying OpenGL context
    ///
    ////////////////////////////////////////////////////////////
    window_impl_x11(video_mode mode, const std::string& title, unsigned long style);

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    ////////////////////////////////////////////////////////////
    ~window_impl_x11();

    ////////////////////////////////////////////////////////////
    /// \brief Get the OS-specific handle of the window
    ///
    /// \return Handle of the window
    ///
    ////////////////////////////////////////////////////////////
    virtual window_handle get_system_handle() const;

    ////////////////////////////////////////////////////////////
    /// \brief Get the position of the window
    ///
    /// \return Position of the window, in pixels
    ///
    ////////////////////////////////////////////////////////////
    virtual std::array<std::int32_t, 2> get_position() const;

    ////////////////////////////////////////////////////////////
    /// \brief Change the position of the window on screen
    ///
    /// \param position New position of the window, in pixels
    ///
    ////////////////////////////////////////////////////////////
    virtual void set_position(const std::array<std::int32_t, 2>& position);

    ////////////////////////////////////////////////////////////
    /// \brief Get the client size of the window
    ///
    /// \return Size of the window, in pixels
    ///
    ////////////////////////////////////////////////////////////
    virtual std::array<std::uint32_t, 2> get_size() const;

    ////////////////////////////////////////////////////////////
    /// \brief Change the size of the rendering region of the window
    ///
    /// \param size New size, in pixels
    ///
    ////////////////////////////////////////////////////////////
    virtual void set_size(const std::array<std::uint32_t, 2>& size);

    ////////////////////////////////////////////////////////////
    /// \brief Change the title of the window
    ///
    /// \param title New title
    ///
    ////////////////////////////////////////////////////////////
    virtual void set_title(const std::string& title);

    ////////////////////////////////////////////////////////////
    /// \brief Change the window's icon
    ///
    /// \param width  Icon's width, in pixels
    /// \param height Icon's height, in pixels
    /// \param pixels Pointer to the pixels in memory, format must be RGBA 32 bits
    ///
    ////////////////////////////////////////////////////////////
    virtual void set_icon(unsigned int width, unsigned int height, const std::uint8_t* pixels);

    ////////////////////////////////////////////////////////////
    /// \brief Show or hide the window
    ///
    /// \param visible True to show, false to hide
    ///
    ////////////////////////////////////////////////////////////
    virtual void set_visible(bool visible);

    virtual void maximize();
    virtual void minimize();O
    virtual void restore();
    virtual void set_alpha(float alpha);

    ////////////////////////////////////////////////////////////
    /// \brief Show or hide the mouse cursor
    ///
    /// \param visible True to show, false to hide
    ///
    ////////////////////////////////////////////////////////////
    virtual void set_mouse_cursor_visible(bool visible);

    ////////////////////////////////////////////////////////////
    /// \brief Grab or release the mouse cursor
    ///
    /// \param grabbed True to enable, false to disable
    ///
    ////////////////////////////////////////////////////////////
    virtual void set_mouse_cursor_grabbed(bool grabbed);

    ////////////////////////////////////////////////////////////
    /// \brief Set the displayed cursor to a native system cursor
    ///
    /// \param cursor Native system cursor type to display
    ///
    ////////////////////////////////////////////////////////////
    virtual void set_mouse_cursor(const cursor_impl& cursor);

    ////////////////////////////////////////////////////////////
    /// \brief Enable or disable automatic key-repeat
    ///
    /// \param enabled True to enable, false to disable
    ///
    ////////////////////////////////////////////////////////////
    virtual void set_key_repeat_enabled(bool enabled);

    ////////////////////////////////////////////////////////////
    /// \brief Request the current window to be made the active
    ///        foreground window
    ///
    ////////////////////////////////////////////////////////////
    virtual void request_focus();

    ////////////////////////////////////////////////////////////
    /// \brief Check whether the window has the input focus
    ///
    /// \return True if window has focus, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    virtual bool has_focus() const;

protected:

    ////////////////////////////////////////////////////////////
    /// \brief Process incoming events from the operating system
    ///
    ////////////////////////////////////////////////////////////
    virtual void process_events();

private:

    ////////////////////////////////////////////////////////////
    /// \brief Request the WM to make the current window active
    ///
    ////////////////////////////////////////////////////////////
    void grab_focus();

    ////////////////////////////////////////////////////////////
    /// \brief Set fullscreen video mode
    ///
    /// \param Mode video mode to switch to
    ///
    ////////////////////////////////////////////////////////////
    void set_video_mode(const video_mode& mode);

    ////////////////////////////////////////////////////////////
    /// \brief Reset to desktop video mode
    ///
    ////////////////////////////////////////////////////////////
    void reset_video_mode();

    ////////////////////////////////////////////////////////////
    /// \brief Switch to fullscreen mode
    ///
    ////////////////////////////////////////////////////////////
    void switch_to_fullscreen();

    ////////////////////////////////////////////////////////////
    /// \brief Set the WM protocols we support
    ///
    ////////////////////////////////////////////////////////////
    void set_protocols();

    ////////////////////////////////////////////////////////////
    /// \brief Update the last time we received user input
    ///
    /// \param time Last time we received user input
    ///
    ////////////////////////////////////////////////////////////
    void update_last_input_time(::Time time);

    ////////////////////////////////////////////////////////////
    /// \brief Do some common initializations after the window has been created
    ///
    ////////////////////////////////////////////////////////////
    void initialize();

    ////////////////////////////////////////////////////////////
    /// \brief Create a transparent mouse cursor
    ///
    ////////////////////////////////////////////////////////////
    void create_hidden_cursor();

    ////////////////////////////////////////////////////////////
    /// \brief Cleanup graphical resources attached to the window
    ///
    ////////////////////////////////////////////////////////////
    void cleanup();

    ////////////////////////////////////////////////////////////
    /// \brief Process an incoming event from the window
    ///
    /// \param windowEvent platform_event which has been received
    ///
    /// \return True if the event was processed, false if it was discarded
    ///
    ////////////////////////////////////////////////////////////
    bool process_event(XEvent& windowEvent);
	//static void set_window_maximized(window_impl_x11& window, bool maximized);
    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    ::Window   _window;         ///< X identifier defining our window
    ::Display* _display;        ///< Pointer to the display
    int        _screen;         ///< Screen identifier
    XIM        _input_method;    ///< Input method linked to the X display
    XIC        _input_context;   ///< Input context used to get unicode input in our window
    bool       _is_external;     ///< Tell whether the window has been created externally or by mml
    int        _old_video_mode;   ///< Video mode in use before we switch to fullscreen
    ::Cursor   _hidden_cursor;   ///< As X11 doesn't provide cursor hiding, we must create a transparent one
    ::Cursor   _last_cursor;     ///< Last cursor used -- this data is not owned by the window and is required to be always valid
    bool       _key_repeat;      ///< Is the KeyRepeat feature enabled?
    std::array<std::int32_t, 2>   _previous_size;   ///< Previous size of the window, to find if a ConfigureNotify event is a resize event (could be a move event only)
    bool       _use_size_hints;   ///< Is the size of the window fixed with size hints?
    bool       _fullscreen;     ///< Is the window in fullscreen?
    bool       _cursor_grabbed;  ///< Is the mouse cursor trapped?
    bool       _window_mapped;   ///< Has the window been mapped by the window manager?
    Pixmap     _icon_pixmap;     ///< The current icon pixmap if in use
    Pixmap     _icon_mask_pixmap; ///< The current icon mask pixmap if in use
    ::Time     _last_input_time;  ///< Last time we received user input
};

} // namespace priv

} // namespace mml


#endif // MML_WINDOWIMPLX11_HPP
