#ifndef MML_EVENT_HPP
#define MML_EVENT_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <cstdint>
#include <mml/window/joystick.hpp>
#include <mml/window/keyboard.hpp>
#include <mml/window/mouse.hpp>
#include <mml/window/sensor.hpp>


namespace mml
{
////////////////////////////////////////////////////////////
/// \brief Defines a system event and its parameters
///
////////////////////////////////////////////////////////////
class platform_event
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Size events parameters (resized)
    ///
    ////////////////////////////////////////////////////////////
    struct size_event
    {
        unsigned int width;  ///< New width, in pixels
        unsigned int height; ///< New height, in pixels
    };

    ////////////////////////////////////////////////////////////
    /// \brief keyboard event parameters (key_pressed, key_released)
    ///
    ////////////////////////////////////////////////////////////
    struct key_event
    {
        keyboard::key code;    ///< Code of the key that has been pressed
        bool          alt;     ///< Is the Alt key pressed?
        bool          control; ///< Is the Control key pressed?
        bool          shift;   ///< Is the Shift key pressed?
        bool          system;  ///< Is the System key pressed?
    };

    ////////////////////////////////////////////////////////////
    /// \brief Text event parameters (text_entered)
    ///
    ////////////////////////////////////////////////////////////
    struct text_event
    {
		std::uint32_t unicode; ///< UTF-32 Unicode value of the character
    };

    ////////////////////////////////////////////////////////////
    /// \brief mouse move event parameters (mouse_moved)
    ///
    ////////////////////////////////////////////////////////////
    struct mouse_move_event
    {
        int x; ///< X position of the mouse pointer, relative to the left of the owner window
        int y; ///< Y position of the mouse pointer, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief mouse buttons events parameters
    ///        (mouse_button_pressed, mouse_button_released)
    ///
    ////////////////////////////////////////////////////////////
    struct mouse_button_event
    {
        mouse::button button; ///< Code of the button that has been pressed
        int           x;      ///< X position of the mouse pointer, relative to the left of the owner window
        int           y;      ///< Y position of the mouse pointer, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief mouse wheel events parameters (mouse_wheel_moved)
    ///
    /// \deprecated This event is deprecated and potentially inaccurate.
    ///             Use mouse_wheel_scroll_event instead.
    ///
    ////////////////////////////////////////////////////////////
    struct mouse_wheel_event
    {
        int delta; ///< Number of ticks the wheel has moved (positive is up, negative is down)
        int x;     ///< X position of the mouse pointer, relative to the left of the owner window
        int y;     ///< Y position of the mouse pointer, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief mouse wheel events parameters (mouse_wheel_scrolled)
    ///
    ////////////////////////////////////////////////////////////
    struct mouse_wheel_scroll_event
    {
        mouse::wheel wheel; ///< Which wheel (for mice with multiple ones)
        float        delta; ///< wheel offset (positive is up/left, negative is down/right). High-precision mice may use non-integral offsets.
        int          x;     ///< X position of the mouse pointer, relative to the left of the owner window
        int          y;     ///< Y position of the mouse pointer, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief joystick connection events parameters
    ///        (joystick_connected, joystick_disconnected)
    ///
    ////////////////////////////////////////////////////////////
    struct joystick_connect_event
    {
        unsigned int joystick_id; ///< Index of the joystick (in range [0 .. joystick::Count - 1])
    };

    ////////////////////////////////////////////////////////////
    /// \brief joystick axis move event parameters (joystick_moved)
    ///
    ////////////////////////////////////////////////////////////
    struct joystick_move_event
    {
        unsigned int   joystick_id; ///< Index of the joystick (in range [0 .. joystick::Count - 1])
        joystick::axis axis;       ///< axis on which the joystick moved
        float          position;   ///< New position on the axis (in range [-100 .. 100])
    };

    ////////////////////////////////////////////////////////////
    /// \brief joystick buttons events parameters
    ///        (joystick_button_pressed, joystick_button_released)
    ///
    ////////////////////////////////////////////////////////////
    struct joystick_button_event
    {
        unsigned int joystick_id; ///< Index of the joystick (in range [0 .. joystick::Count - 1])
        unsigned int button;     ///< Index of the button that has been pressed (in range [0 .. joystick::ButtonCount - 1])
    };

    ////////////////////////////////////////////////////////////
    /// \brief touch events parameters (touch_began, touch_moved, touch_ended)
    ///
    ////////////////////////////////////////////////////////////
    struct touch_event
    {
        unsigned int finger; ///< Index of the finger in case of multi-touch events
        int x;               ///< X position of the touch, relative to the left of the owner window
        int y;               ///< Y position of the touch, relative to the top of the owner window
    };

    ////////////////////////////////////////////////////////////
    /// \brief sensor event parameters (sensor_changed)
    ///
    ////////////////////////////////////////////////////////////
    struct sensor_event
    {
        sensor::type type; ///< Type of the sensor
        float x;           ///< Current value of the sensor on X axis
        float y;           ///< Current value of the sensor on Y axis
        float z;           ///< Current value of the sensor on Z axis
    };

    ////////////////////////////////////////////////////////////
    /// \brief Enumeration of the different types of events
    ///
    ////////////////////////////////////////////////////////////
	enum event_type
	{
		///< The window requested to be closed (no data)
		closed,

		///< The window was resized (data in event.size)
		resized,

		///< The window lost the focus (no data)
		lost_focus,

		///< The window gained the focus (no data)
		gained_focus,

		///< A character was entered (data in event.text)
		text_entered,

		///< A key was pressed (data in event.key)
		key_pressed,

		///< A key was released (data in event.key)
		key_released,

		///< The mouse wheel was scrolled (data in event.mouse_wheel_scroll)
		mouse_wheel_scrolled,

		///< A mouse button was pressed (data in event.mouse_button)
		mouse_button_pressed,

		///< A mouse button was released (data in event.mouse_button)
		mouse_button_released,

		///< The mouse cursor moved (data in event.mouse_move)
		mouse_moved,

		///< The mouse cursor entered the area of the window (no data)
		mouse_entered,

		///< The mouse cursor left the area of the window (no data)
		mouse_left,

		///< A joystick button was pressed (data in event.joystick_button)
		joystick_button_pressed,

		///< A joystick button was released (data in event.joystick_button)
		joystick_button_released,

		///< The joystick moved along an axis (data in event.joystick_move)
		joystick_moved,

		///< A joystick was connected (data in event.joystick_connect)
		joystick_connected,

		///< A joystick was disconnected (data in event.joystick_connect)
		joystick_disconnected,

		///< A touch event began (data in event.touch)
		touch_began,

		///< A touch moved (data in event.touch)
		touch_moved,

		///< A touch event ended (data in event.touch)
		touch_ended,

		///< A sensor value changed (data in event.sensor)
		sensor_changed,

		///< Keep last -- the total number of event types
		count
	};

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    event_type type; ///< Type of the event

	union
	{
		///< Size event parameters (platform_event::resized)
		size_event					size;

		///< key event parameters (platform_event::key_pressed, platform_event::key_released)
		key_event					key;

		///< Text event parameters (platform_event::text_entered)
		text_event					text;

		///< mouse move event parameters (platform_event::mouse_moved)
		mouse_move_event			mouse_move;

		///< mouse button event parameters (platform_event::mouse_button_pressed, platform_event::mouse_button_released)
		mouse_button_event			mouse_button;

		///< mouse wheel event parameters (platform_event::mouse_wheel_scrolled)
		mouse_wheel_scroll_event	mouse_wheel_scroll;

		///< joystick move event parameters (platform_event::joystick_moved)
		joystick_move_event			joystick_move;

		///< joystick button event parameters (platform_event::joystick_button_pressed, platform_event::joystick_button_released)
		joystick_button_event		joystick_button;

		///< joystick (dis)connect event parameters (platform_event::joystick_connected, platform_event::joystick_disconnected)
		joystick_connect_event		joystick_connect;

		///< touch events parameters (platform_event::touch_began, platform_event::touch_moved, platform_event::touch_ended)
		touch_event					touch;

		///< sensor event parameters (platform_event::sensor_changed)
		sensor_event				sensor;
	};
};

} // namespace mml


#endif // MML_EVENT_HPP


////////////////////////////////////////////////////////////
/// \class mml::platform_event
/// \ingroup window
///
/// mml::platform_event holds all the informations about a system event
/// that just happened. Events are retrieved using the
/// mml::window::poll_event and mml::window::wait_event functions.
///
/// A mml::platform_event instance contains the type of the event
/// (mouse moved, key pressed, window closed, ...) as well
/// as the details about this particular event. Please note that
/// the event parameters are defined in a union, which means that
/// only the member matching the type of the event will be properly
/// filled; all other members will have undefined values and must not
/// be read if the type of the event doesn't match. For example,
/// if you received a key_pressed event, then you must read the
/// event.key member, all other members such as event.MouseMove
/// or event.text will have undefined values.
///
/// Usage example:
/// \code
/// mml::platform_event event;
/// while (window.poll_event(event))
/// {
///     // Request for closing the window
///     if (event.type == mml::platform_event::closed)
///         window.dispose();
///
///     // The escape key was pressed
///     if ((event.type == mml::platform_event::key_pressed) && (event.key.code == mml::keyboard::Escape))
///         window.request_close();
///
///     // The window was resized
///     if (event.type == mml::platform_event::resized)
///         doSomethingWithTheNewSize(event.size.width, event.size.height);
///
///     // etc ...
/// }
/// \endcode
///
////////////////////////////////////////////////////////////
