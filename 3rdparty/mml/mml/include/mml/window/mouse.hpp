#ifndef MML_MOUSE_HPP
#define MML_MOUSE_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/export.hpp>
#include <array>
#include <cstdint>

namespace mml
{
class window;

////////////////////////////////////////////////////////////
/// \brief Give access to the real-time state of the mouse
///
////////////////////////////////////////////////////////////
class MML_WINDOW_API mouse
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief mouse buttons
    ///
    ////////////////////////////////////////////////////////////
    enum button
    {
        left,       ///< The left mouse button
        right,      ///< The right mouse button
        middle,     ///< The middle (wheel) mouse button
        x_button1,   ///< The first extra mouse button
        x_button2,   ///< The second extra mouse button

        button_count ///< Keep last -- the total number of mouse buttons
    };

    ////////////////////////////////////////////////////////////
    /// \brief mouse wheels
    ///
    ////////////////////////////////////////////////////////////
    enum wheel
    {
        vertical_wheel,  ///< The vertical mouse wheel
        horizontal_wheel ///< The horizontal mouse wheel
    };

    ////////////////////////////////////////////////////////////
    /// \brief Check if a mouse button is pressed
    ///
    /// \param button button to check
    ///
    /// \return True if the button is pressed, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool is_button_pressed(button button);

    ////////////////////////////////////////////////////////////
    /// \brief Get the current position of the mouse in desktop coordinates
    ///
    /// This function returns the global position of the mouse
    /// cursor on the desktop.
    ///
    /// \return Current position of the mouse
    ///
    ////////////////////////////////////////////////////////////
    static std::array<std::int32_t, 2> get_position();

    ////////////////////////////////////////////////////////////
    /// \brief Get the current position of the mouse in window coordinates
    ///
    /// This function returns the current position of the mouse
    /// cursor, relative to the given window.
    ///
    /// \param relativeTo Reference window
    ///
    /// \return Current position of the mouse
    ///
    ////////////////////////////////////////////////////////////
    static std::array<std::int32_t, 2> get_position(const window& relativeTo);

    ////////////////////////////////////////////////////////////
    /// \brief Set the current position of the mouse in desktop coordinates
    ///
    /// This function sets the global position of the mouse
    /// cursor on the desktop.
    ///
    /// \param position New position of the mouse
    ///
    ////////////////////////////////////////////////////////////
    static void set_position(const std::array<std::int32_t, 2>& position);

    ////////////////////////////////////////////////////////////
    /// \brief Set the current position of the mouse in window coordinates
    ///
    /// This function sets the current position of the mouse
    /// cursor, relative to the given window.
    ///
    /// \param position New position of the mouse
    /// \param relativeTo Reference window
    ///
    ////////////////////////////////////////////////////////////
    static void set_position(const std::array<std::int32_t, 2>& position, const window& relativeTo);
};

} // namespace mml


#endif // MML_MOUSE_HPP


////////////////////////////////////////////////////////////
/// \class mml::mouse
/// \ingroup window
///
/// mml::mouse provides an interface to the state of the
/// mouse. It only contains static functions (a single
/// mouse is assumed), so it's not meant to be instantiated.
///
/// This class allows users to query the mouse state at any
/// time and directly, without having to deal with a window and
/// its events. Compared to the mouse_moved, mouse_button_pressed
/// and mouse_button_released events, mml::mouse can retrieve the
/// state of the cursor and the buttons at any time
/// (you don't need to store and update a boolean on your side
/// in order to know if a button is pressed or released), and you
/// always get the real state of the mouse, even if it is
/// moved, pressed or released when your window is out of focus
/// and no event is triggered.
///
/// The set_position and get_position functions can be used to change
/// or retrieve the current position of the mouse pointer. There are
/// two versions: one that operates in global coordinates (relative
/// to the desktop) and one that operates in window coordinates
/// (relative to a specific window).
///
/// Usage example:
/// \code
/// if (mml::mouse::is_button_pressed(mml::mouse::Left))
/// {
///     // left click...
/// }
///
/// // get global mouse position
/// mml::std::array<std::int32_t, 2> position = mml::mouse::get_position();
///
/// // set mouse position relative to a window
/// mml::mouse::set_position(mml::std::array<std::int32_t, 2>(100, 200), window);
/// \endcode
///
/// \see mml::joystick, mml::keyboard, mml::touch
///
////////////////////////////////////////////////////////////
