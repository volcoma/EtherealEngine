#ifndef MML_JOYSTICK_HPP
#define MML_JOYSTICK_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/export.hpp>
#include <string>

namespace mml
{
////////////////////////////////////////////////////////////
/// \brief Give access to the real-time state of the joysticks
///
////////////////////////////////////////////////////////////
class MML_WINDOW_API joystick
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Constants related to joysticks capabilities
    ///
    ////////////////////////////////////////////////////////////
    enum
    {
        count       = 8,  ///< Maximum number of supported joysticks
        button_count = 32, ///< Maximum number of supported buttons
        axis_count   = 8   ///< Maximum number of supported axes
    };

    ////////////////////////////////////////////////////////////
    /// \brief Axes supported by mml joysticks
    ///
    ////////////////////////////////////////////////////////////
    enum axis
    {
        X,    ///< The X axis
        Y,    ///< The Y axis
        Z,    ///< The Z axis
        R,    ///< The R axis
        U,    ///< The U axis
        V,    ///< The V axis
        PovX, ///< The X axis of the point-of-view hat
        PovY  ///< The Y axis of the point-of-view hat
    };

    ////////////////////////////////////////////////////////////
    /// \brief Structure holding a joystick's identification
    ///
    ////////////////////////////////////////////////////////////
    struct MML_WINDOW_API identification
    {
        identification();

        std::string  name;      ///< Name of the joystick
        unsigned int vendor_id;  ///< Manufacturer identifier
        unsigned int product_id; ///< Product identifier
    };

    ////////////////////////////////////////////////////////////
    /// \brief Check if a joystick is connected
    ///
    /// \param joystick Index of the joystick to check
    ///
    /// \return True if the joystick is connected, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool is_connected(unsigned int joystick);

    ////////////////////////////////////////////////////////////
    /// \brief Return the number of buttons supported by a joystick
    ///
    /// If the joystick is not connected, this function returns 0.
    ///
    /// \param joystick Index of the joystick
    ///
    /// \return Number of buttons supported by the joystick
    ///
    ////////////////////////////////////////////////////////////
    static unsigned int get_button_count(unsigned int joystick);

    ////////////////////////////////////////////////////////////
    /// \brief Check if a joystick supports a given axis
    ///
    /// If the joystick is not connected, this function returns false.
    ///
    /// \param joystick Index of the joystick
    /// \param axis     axis to check
    ///
    /// \return True if the joystick supports the axis, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool has_axis(unsigned int joystick, axis axis);

    ////////////////////////////////////////////////////////////
    /// \brief Check if a joystick button is pressed
    ///
    /// If the joystick is not connected, this function returns false.
    ///
    /// \param joystick Index of the joystick
    /// \param button   button to check
    ///
    /// \return True if the button is pressed, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool is_button_pressed(unsigned int joystick, unsigned int button);

    ////////////////////////////////////////////////////////////
    /// \brief Get the current position of a joystick axis
    ///
    /// If the joystick is not connected, this function returns 0.
    ///
    /// \param joystick Index of the joystick
    /// \param axis     axis to check
    ///
    /// \return Current position of the axis, in range [-100 .. 100]
    ///
    ////////////////////////////////////////////////////////////
    static float get_axis_position(unsigned int joystick, axis axis);

    ////////////////////////////////////////////////////////////
    /// \brief Get the joystick information
    ///
    /// \param joystick Index of the joystick
    ///
    /// \return Structure containing joystick information.
    ///
    ////////////////////////////////////////////////////////////
    static identification get_identification(unsigned int joystick);

    ////////////////////////////////////////////////////////////
    /// \brief Update the states of all joysticks
    ///
    /// This function is used internally by mml, so you normally
    /// don't have to call it explicitly. However, you may need to
    /// call it if you have no window yet (or no window at all):
    /// in this case the joystick states are not updated automatically.
    ///
    ////////////////////////////////////////////////////////////
    static void update();
};

} // namespace mml


#endif // MML_JOYSTICK_HPP


////////////////////////////////////////////////////////////
/// \class mml::joystick
/// \ingroup window
///
/// mml::joystick provides an interface to the state of the
/// joysticks. It only contains static functions, so it's not
/// meant to be instantiated. Instead, each joystick is identified
/// by an index that is passed to the functions of this class.
///
/// This class allows users to query the state of joysticks at any
/// time and directly, without having to deal with a window and
/// its events. Compared to the joystick_moved, joystick_button_pressed
/// and joystick_button_released events, mml::joystick can retrieve the
/// state of axes and buttons of joysticks at any time
/// (you don't need to store and update a boolean on your side
/// in order to know if a button is pressed or released), and you
/// always get the real state of joysticks, even if they are
/// moved, pressed or released when your window is out of focus
/// and no event is triggered.
///
/// mml supports:
/// \li 8 joysticks (mml::joystick::Count)
/// \li 32 buttons per joystick (mml::joystick::ButtonCount)
/// \li 8 axes per joystick (mml::joystick::AxisCount)
///
/// Unlike the keyboard or mouse, the state of joysticks is sometimes
/// not directly available (depending on the OS), therefore an update()
/// function must be called in order to update the current state of
/// joysticks. When you have a window with event handling, this is done
/// automatically, you don't need to call anything. But if you have no
/// window, or if you want to check joysticks state before creating one,
/// you must call mml::joystick::update explicitly.
///
/// Usage example:
/// \code
/// // Is joystick #0 connected?
/// bool connected = mml::joystick::is_connected(0);
///
/// // How many buttons does joystick #0 support?
/// unsigned int buttons = mml::joystick::get_button_count(0);
///
/// // Does joystick #0 define a X axis?
/// bool hasX = mml::joystick::has_axis(0, mml::joystick::X);
///
/// // Is button #2 pressed on joystick #0?
/// bool pressed = mml::joystick::is_button_pressed(0, 2);
///
/// // What's the current position of the Y axis on joystick #0?
/// float position = mml::joystick::get_axis_position(0, mml::joystick::Y);
/// \endcode
///
/// \see mml::keyboard, mml::mouse
///
////////////////////////////////////////////////////////////
