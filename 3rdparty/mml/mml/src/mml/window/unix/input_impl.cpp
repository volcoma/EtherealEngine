////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/window.hpp>
#include <mml/window/input_impl.hpp>
#include <mml/window/unix/display.hpp>
#include <mml/system/err.hpp>
#include <X11/Xlib.h>
#include <X11/keysym.h>


namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
bool input_impl::is_key_pressed(keyboard::key key)
{
    // Get the corresponding X11 keysym
    KeySym keysym = 0;
    switch (key)
    {
        case keyboard::LShift:     keysym = XK_Shift_L;      break;
        case keyboard::RShift:     keysym = XK_Shift_R;      break;
        case keyboard::LControl:   keysym = XK_Control_L;    break;
        case keyboard::RControl:   keysym = XK_Control_R;    break;
        case keyboard::LAlt:       keysym = XK_Alt_L;        break;
        case keyboard::RAlt:       keysym = XK_Alt_R;        break;
        case keyboard::LSystem:    keysym = XK_Super_L;      break;
        case keyboard::RSystem:    keysym = XK_Super_R;      break;
        case keyboard::Menu:       keysym = XK_Menu;         break;
        case keyboard::Escape:     keysym = XK_Escape;       break;
        case keyboard::Semicolon:  keysym = XK_semicolon;    break;
        case keyboard::Slash:      keysym = XK_slash;        break;
        case keyboard::Equal:      keysym = XK_equal;        break;
        case keyboard::Hyphen:     keysym = XK_minus;        break;
        case keyboard::LBracket:   keysym = XK_bracketleft;  break;
        case keyboard::RBracket:   keysym = XK_bracketright; break;
        case keyboard::Comma:      keysym = XK_comma;        break;
        case keyboard::Period:     keysym = XK_period;       break;
        case keyboard::Quote:      keysym = XK_apostrophe;   break;
        case keyboard::Backslash:  keysym = XK_backslash;    break;
        case keyboard::Tilde:      keysym = XK_grave;        break;
        case keyboard::Space:      keysym = XK_space;        break;
        case keyboard::Enter:      keysym = XK_Return;       break;
        case keyboard::Backspace:  keysym = XK_BackSpace;    break;
        case keyboard::Tab:        keysym = XK_Tab;          break;
        case keyboard::PageUp:     keysym = XK_Prior;        break;
        case keyboard::PageDown:   keysym = XK_Next;         break;
        case keyboard::End:        keysym = XK_End;          break;
        case keyboard::Home:       keysym = XK_Home;         break;
        case keyboard::Insert:     keysym = XK_Insert;       break;
        case keyboard::Delete:     keysym = XK_Delete;       break;
        case keyboard::Add:        keysym = XK_KP_Add;       break;
        case keyboard::Subtract:   keysym = XK_KP_Subtract;  break;
        case keyboard::Multiply:   keysym = XK_KP_Multiply;  break;
        case keyboard::Divide:     keysym = XK_KP_Divide;    break;
        case keyboard::Pause:      keysym = XK_Pause;        break;
        case keyboard::F1:         keysym = XK_F1;           break;
        case keyboard::F2:         keysym = XK_F2;           break;
        case keyboard::F3:         keysym = XK_F3;           break;
        case keyboard::F4:         keysym = XK_F4;           break;
        case keyboard::F5:         keysym = XK_F5;           break;
        case keyboard::F6:         keysym = XK_F6;           break;
        case keyboard::F7:         keysym = XK_F7;           break;
        case keyboard::F8:         keysym = XK_F8;           break;
        case keyboard::F9:         keysym = XK_F9;           break;
        case keyboard::F10:        keysym = XK_F10;          break;
        case keyboard::F11:        keysym = XK_F11;          break;
        case keyboard::F12:        keysym = XK_F12;          break;
        case keyboard::F13:        keysym = XK_F13;          break;
        case keyboard::F14:        keysym = XK_F14;          break;
        case keyboard::F15:        keysym = XK_F15;          break;
        case keyboard::Left:       keysym = XK_Left;         break;
        case keyboard::Right:      keysym = XK_Right;        break;
        case keyboard::Up:         keysym = XK_Up;           break;
        case keyboard::Down:       keysym = XK_Down;         break;
        case keyboard::Numpad0:    keysym = XK_KP_Insert;    break;
        case keyboard::Numpad1:    keysym = XK_KP_End;       break;
        case keyboard::Numpad2:    keysym = XK_KP_Down;      break;
        case keyboard::Numpad3:    keysym = XK_KP_Page_Down; break;
        case keyboard::Numpad4:    keysym = XK_KP_Left;      break;
        case keyboard::Numpad5:    keysym = XK_KP_Begin;     break;
        case keyboard::Numpad6:    keysym = XK_KP_Right;     break;
        case keyboard::Numpad7:    keysym = XK_KP_Home;      break;
        case keyboard::Numpad8:    keysym = XK_KP_Up;        break;
        case keyboard::Numpad9:    keysym = XK_KP_Page_Up;   break;
        case keyboard::A:          keysym = XK_a;            break;
        case keyboard::B:          keysym = XK_b;            break;
        case keyboard::C:          keysym = XK_c;            break;
        case keyboard::D:          keysym = XK_d;            break;
        case keyboard::E:          keysym = XK_e;            break;
        case keyboard::F:          keysym = XK_f;            break;
        case keyboard::G:          keysym = XK_g;            break;
        case keyboard::H:          keysym = XK_h;            break;
        case keyboard::I:          keysym = XK_i;            break;
        case keyboard::J:          keysym = XK_j;            break;
        case keyboard::K:          keysym = XK_k;            break;
        case keyboard::L:          keysym = XK_l;            break;
        case keyboard::M:          keysym = XK_m;            break;
        case keyboard::N:          keysym = XK_n;            break;
        case keyboard::O:          keysym = XK_o;            break;
        case keyboard::P:          keysym = XK_p;            break;
        case keyboard::Q:          keysym = XK_q;            break;
        case keyboard::R:          keysym = XK_r;            break;
        case keyboard::S:          keysym = XK_s;            break;
        case keyboard::T:          keysym = XK_t;            break;
        case keyboard::U:          keysym = XK_u;            break;
        case keyboard::V:          keysym = XK_v;            break;
        case keyboard::W:          keysym = XK_w;            break;
        case keyboard::X:          keysym = XK_x;            break;
        case keyboard::Y:          keysym = XK_y;            break;
        case keyboard::Z:          keysym = XK_z;            break;
        case keyboard::Num0:       keysym = XK_0;            break;
        case keyboard::Num1:       keysym = XK_1;            break;
        case keyboard::Num2:       keysym = XK_2;            break;
        case keyboard::Num3:       keysym = XK_3;            break;
        case keyboard::Num4:       keysym = XK_4;            break;
        case keyboard::Num5:       keysym = XK_5;            break;
        case keyboard::Num6:       keysym = XK_6;            break;
        case keyboard::Num7:       keysym = XK_7;            break;
        case keyboard::Num8:       keysym = XK_8;            break;
        case keyboard::Num9:       keysym = XK_9;            break;
        default:                   keysym = 0;               break;
    }

    // Sanity checks
    if (key < 0 || key >= mml::keyboard::KeyCount)
        return false;

    // Open a connection with the X server
    Display* display = open_display();

    // Convert to keycode
    KeyCode keycode = XKeysymToKeycode(display, keysym);
    if (keycode != 0)
    {
        // Get the whole keyboard state
        char keys[32];
        XQueryKeymap(display, keys);

        // Close the connection with the X server
        close_display(display);

        // Check our keycode
        return (keys[keycode / 8] & (1 << (keycode % 8))) != 0;
    }
    else
    {
        // Close the connection with the X server
        close_display(display);

        return false;
    }
}


////////////////////////////////////////////////////////////
void input_impl::set_virtual_keyboard_visible(bool /*visible*/)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
bool input_impl::is_mouse_button_pressed(mouse::button button)
{
    // Open a connection with the X server
    Display* display = open_display();

    // we don't care about these but they are required
    ::Window root, child;
    int wx, wy;
    int gx, gy;

    unsigned int buttons = 0;
    XQueryPointer(display, DefaultRootWindow(display), &root, &child, &gx, &gy, &wx, &wy, &buttons);

    // Close the connection with the X server
    close_display(display);

    switch (button)
    {
        case mouse::left:     return buttons & Button1Mask;
        case mouse::right:    return buttons & Button3Mask;
        case mouse::middle:   return buttons & Button2Mask;
		case mouse::x_button1: return false; // not supported by X
		case mouse::x_button2: return false; // not supported by X
        default:              return false;
    }

    return false;
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> input_impl::get_mouse_position()
{
    // Open a connection with the X server
    Display* display = open_display();

    // we don't care about these but they are required
    ::Window root, child;
    int x, y;
    unsigned int buttons;

    int gx = 0;
    int gy = 0;
    XQueryPointer(display, DefaultRootWindow(display), &root, &child, &gx, &gy, &x, &y, &buttons);

    // Close the connection with the X server
    close_display(display);

	return std::array<std::int32_t, 2>({ gx, gy });
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> input_impl::get_mouse_position(const window& relativeTo)
{
    window_handle handle = relativeTo.native_handle();
    if (handle)
    {
        // Open a connection with the X server
        Display* display = open_display();

        // we don't care about these but they are required
        ::Window root, child;
        int gx, gy;
        unsigned int buttons;

        int x = 0;
        int y = 0;
        XQueryPointer(display, handle, &root, &child, &gx, &gy, &x, &y, &buttons);

        // Close the connection with the X server
        close_display(display);

		return std::array<std::int32_t, 2>({ x, y });
    }
    else
    {
		return std::array<std::int32_t, 2>({0, 0});
    }
}


////////////////////////////////////////////////////////////
void input_impl::set_mouse_position(const std::array<std::int32_t, 2>& position)
{
    // Open a connection with the X server
    Display* display = open_display();

    XWarpPointer(display, None, DefaultRootWindow(display), 0, 0, 0, 0, position[0], position[1]);
    XFlush(display);

    // Close the connection with the X server
    close_display(display);
}


////////////////////////////////////////////////////////////
void input_impl::set_mouse_position(const std::array<std::int32_t, 2>& position, const window& relativeTo)
{
    // Open a connection with the X server
    Display* display = open_display();

    window_handle handle = relativeTo.native_handle();
    if (handle)
    {
        XWarpPointer(display, None, handle, 0, 0, 0, 0, position[0], position[1]);
        XFlush(display);
    }

    // Close the connection with the X server
    close_display(display);
}


////////////////////////////////////////////////////////////
bool input_impl::is_touch_down(unsigned int /*finger*/)
{
    // Not applicable
    return false;
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> input_impl::get_touch_position(unsigned int /*finger*/)
{
    // Not applicable
    return std::array<std::int32_t, 2>();
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> input_impl::get_touch_position(unsigned int /*finger*/, const window& /*relativeTo*/)
{
    // Not applicable
    return std::array<std::int32_t, 2>();
}

} // namespace priv

} // namespace mml
