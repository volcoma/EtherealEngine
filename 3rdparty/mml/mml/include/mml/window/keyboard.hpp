#ifndef MML_KEYBOARD_HPP
#define MML_KEYBOARD_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/export.hpp>


namespace mml
{
////////////////////////////////////////////////////////////
/// \brief Give access to the real-time state of the keyboard
///
////////////////////////////////////////////////////////////
class MML_WINDOW_API keyboard
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief key codes
    ///
    ////////////////////////////////////////////////////////////
    enum key
    {
        Unknown = -1, ///< Unhandled key
        A = 0,        ///< The A key
        B,            ///< The B key
        C,            ///< The C key
        D,            ///< The D key
        E,            ///< The E key
        F,            ///< The F key
        G,            ///< The G key
        H,            ///< The H key
        I,            ///< The I key
        J,            ///< The J key
        K,            ///< The K key
        L,            ///< The L key
        M,            ///< The M key
        N,            ///< The N key
        O,            ///< The O key
        P,            ///< The P key
        Q,            ///< The Q key
        R,            ///< The R key
        S,            ///< The S key
        T,            ///< The T key
        U,            ///< The U key
        V,            ///< The V key
        W,            ///< The W key
        X,            ///< The X key
        Y,            ///< The Y key
        Z,            ///< The Z key
        Num0,         ///< The 0 key
        Num1,         ///< The 1 key
        Num2,         ///< The 2 key
        Num3,         ///< The 3 key
        Num4,         ///< The 4 key
        Num5,         ///< The 5 key
        Num6,         ///< The 6 key
        Num7,         ///< The 7 key
        Num8,         ///< The 8 key
        Num9,         ///< The 9 key
        Escape,       ///< The Escape key
        LControl,     ///< The left Control key
        LShift,       ///< The left Shift key
        LAlt,         ///< The left Alt key
        LSystem,      ///< The left OS specific key: window (Windows and Linux), apple (MacOS X), ...
        RControl,     ///< The right Control key
        RShift,       ///< The right Shift key
        RAlt,         ///< The right Alt key
        RSystem,      ///< The right OS specific key: window (Windows and Linux), apple (MacOS X), ...
        Menu,         ///< The Menu key
        LBracket,     ///< The [ key
        RBracket,     ///< The ] key
        Semicolon,    ///< The ; key
        Comma,        ///< The , key
        Period,       ///< The . key
        Quote,        ///< The ' key
        Slash,        ///< The / key
        Backslash,    ///< The \ key
        Tilde,        ///< The ~ key
        Equal,        ///< The = key
        Hyphen,       ///< The - key (hyphen)
        Space,        ///< The Space key
        Enter,        ///< The Enter/Return key
        Backspace,    ///< The Backspace key
        Tab,          ///< The Tabulation key
        PageUp,       ///< The Page up key
        PageDown,     ///< The Page down key
        End,          ///< The End key
        Home,         ///< The Home key
        Insert,       ///< The Insert key
        Delete,       ///< The Delete key
        Add,          ///< The + key
        Subtract,     ///< The - key (minus, usually from numpad)
        Multiply,     ///< The * key
        Divide,       ///< The / key
        Left,         ///< Left arrow
        Right,        ///< Right arrow
        Up,           ///< Up arrow
        Down,         ///< Down arrow
        Numpad0,      ///< The numpad 0 key
        Numpad1,      ///< The numpad 1 key
        Numpad2,      ///< The numpad 2 key
        Numpad3,      ///< The numpad 3 key
        Numpad4,      ///< The numpad 4 key
        Numpad5,      ///< The numpad 5 key
        Numpad6,      ///< The numpad 6 key
        Numpad7,      ///< The numpad 7 key
        Numpad8,      ///< The numpad 8 key
        Numpad9,      ///< The numpad 9 key
        F1,           ///< The F1 key
        F2,           ///< The F2 key
        F3,           ///< The F3 key
        F4,           ///< The F4 key
        F5,           ///< The F5 key
        F6,           ///< The F6 key
        F7,           ///< The F7 key
        F8,           ///< The F8 key
        F9,           ///< The F9 key
        F10,          ///< The F10 key
        F11,          ///< The F11 key
        F12,          ///< The F12 key
        F13,          ///< The F13 key
        F14,          ///< The F14 key
        F15,          ///< The F15 key
        Pause,        ///< The Pause key

        KeyCount      ///< Keep last -- the total number of keyboard keys
    };

    ////////////////////////////////////////////////////////////
    /// \brief Check if a key is pressed
    ///
    /// \param key key to check
    ///
    /// \return True if the key is pressed, false otherwise
    ///
    ////////////////////////////////////////////////////////////
    static bool is_key_pressed(key k);

    ////////////////////////////////////////////////////////////
    /// \brief Show or hide the virtual keyboard
    ///
    /// Warning: the virtual keyboard is not supported on all
    /// systems. It will typically be implemented on mobile OSes
    /// (Android, iOS) but not on desktop OSes (Windows, Linux, ...).
    ///
    /// If the virtual keyboard is not available, this function does
    /// nothing.
    ///
    /// \param visible True to show, false to hide
    ///
    ////////////////////////////////////////////////////////////
    static void set_virtual_keyboard_visible(bool visible);
};

} // namespace mml


#endif // MML_KEYBOARD_HPP


////////////////////////////////////////////////////////////
/// \class mml::keyboard
/// \ingroup window
///
/// mml::keyboard provides an interface to the state of the
/// keyboard. It only contains static functions (a single
/// keyboard is assumed), so it's not meant to be instantiated.
///
/// This class allows users to query the keyboard state at any
/// time and directly, without having to deal with a window and
/// its events. Compared to the key_pressed and key_released events,
/// mml::keyboard can retrieve the state of a key at any time
/// (you don't need to store and update a boolean on your side
/// in order to know if a key is pressed or released), and you
/// always get the real state of the keyboard, even if keys are
/// pressed or released when your window is out of focus and no
/// event is triggered.
///
/// Usage example:
/// \code
/// if (mml::keyboard::is_key_pressed(mml::keyboard::Left))
/// {
///     // move left...
/// }
/// else if (mml::keyboard::is_key_pressed(mml::keyboard::Right))
/// {
///     // move right...
/// }
/// else if (mml::keyboard::is_key_pressed(mml::keyboard::Escape))
/// {
///     // quit...
/// }
/// \endcode
///
/// \see mml::joystick, mml::mouse, mml::touch
///
////////////////////////////////////////////////////////////
