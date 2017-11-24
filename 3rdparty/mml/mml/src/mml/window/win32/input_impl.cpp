////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#ifdef _WIN32_WINDOWS
    #undef _WIN32_WINDOWS
#endif
#ifdef _WIN32_WINNT
    #undef _WIN32_WINNT
#endif
#define _WIN32_WINDOWS 0x0501
#define _WIN32_WINNT   0x0501
#include <mml/window/window.hpp>
#include <mml/window/win32/input_impl.hpp>
#include <windows.h>


namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
bool input_impl::is_key_pressed(keyboard::key key)
{
    int vkey = 0;
    switch (key)
    {
        default:                   vkey = 0;             break;
        case keyboard::A:          vkey = 'A';           break;
        case keyboard::B:          vkey = 'B';           break;
        case keyboard::C:          vkey = 'C';           break;
        case keyboard::D:          vkey = 'D';           break;
        case keyboard::E:          vkey = 'E';           break;
        case keyboard::F:          vkey = 'F';           break;
        case keyboard::G:          vkey = 'G';           break;
        case keyboard::H:          vkey = 'H';           break;
        case keyboard::I:          vkey = 'I';           break;
        case keyboard::J:          vkey = 'J';           break;
        case keyboard::K:          vkey = 'K';           break;
        case keyboard::L:          vkey = 'L';           break;
        case keyboard::M:          vkey = 'M';           break;
        case keyboard::N:          vkey = 'N';           break;
        case keyboard::O:          vkey = 'O';           break;
        case keyboard::P:          vkey = 'P';           break;
        case keyboard::Q:          vkey = 'Q';           break;
        case keyboard::R:          vkey = 'R';           break;
        case keyboard::S:          vkey = 'S';           break;
        case keyboard::T:          vkey = 'T';           break;
        case keyboard::U:          vkey = 'U';           break;
        case keyboard::V:          vkey = 'V';           break;
        case keyboard::W:          vkey = 'W';           break;
        case keyboard::X:          vkey = 'X';           break;
        case keyboard::Y:          vkey = 'Y';           break;
        case keyboard::Z:          vkey = 'Z';           break;
        case keyboard::Num0:       vkey = '0';           break;
        case keyboard::Num1:       vkey = '1';           break;
        case keyboard::Num2:       vkey = '2';           break;
        case keyboard::Num3:       vkey = '3';           break;
        case keyboard::Num4:       vkey = '4';           break;
        case keyboard::Num5:       vkey = '5';           break;
        case keyboard::Num6:       vkey = '6';           break;
        case keyboard::Num7:       vkey = '7';           break;
        case keyboard::Num8:       vkey = '8';           break;
        case keyboard::Num9:       vkey = '9';           break;
        case keyboard::Escape:     vkey = VK_ESCAPE;     break;
        case keyboard::LControl:   vkey = VK_LCONTROL;   break;
        case keyboard::LShift:     vkey = VK_LSHIFT;     break;
        case keyboard::LAlt:       vkey = VK_LMENU;      break;
        case keyboard::LSystem:    vkey = VK_LWIN;       break;
        case keyboard::RControl:   vkey = VK_RCONTROL;   break;
        case keyboard::RShift:     vkey = VK_RSHIFT;     break;
        case keyboard::RAlt:       vkey = VK_RMENU;      break;
        case keyboard::RSystem:    vkey = VK_RWIN;       break;
        case keyboard::Menu:       vkey = VK_APPS;       break;
        case keyboard::LBracket:   vkey = VK_OEM_4;      break;
        case keyboard::RBracket:   vkey = VK_OEM_6;      break;
        case keyboard::SemiColon:  vkey = VK_OEM_1;      break;
        case keyboard::Comma:      vkey = VK_OEM_COMMA;  break;
        case keyboard::Period:     vkey = VK_OEM_PERIOD; break;
        case keyboard::Quote:      vkey = VK_OEM_7;      break;
        case keyboard::Slash:      vkey = VK_OEM_2;      break;
        case keyboard::BackSlash:  vkey = VK_OEM_5;      break;
        case keyboard::Tilde:      vkey = VK_OEM_3;      break;
        case keyboard::Equal:      vkey = VK_OEM_PLUS;   break;
        case keyboard::Dash:       vkey = VK_OEM_MINUS;  break;
        case keyboard::Space:      vkey = VK_SPACE;      break;
        case keyboard::Return:     vkey = VK_RETURN;     break;
        case keyboard::BackSpace:  vkey = VK_BACK;       break;
        case keyboard::Tab:        vkey = VK_TAB;        break;
        case keyboard::PageUp:     vkey = VK_PRIOR;      break;
        case keyboard::PageDown:   vkey = VK_NEXT;       break;
        case keyboard::End:        vkey = VK_END;        break;
        case keyboard::Home:       vkey = VK_HOME;       break;
        case keyboard::Insert:     vkey = VK_INSERT;     break;
        case keyboard::Delete:     vkey = VK_DELETE;     break;
        case keyboard::Add:        vkey = VK_ADD;        break;
        case keyboard::Subtract:   vkey = VK_SUBTRACT;   break;
        case keyboard::Multiply:   vkey = VK_MULTIPLY;   break;
        case keyboard::Divide:     vkey = VK_DIVIDE;     break;
        case keyboard::Left:       vkey = VK_LEFT;       break;
        case keyboard::Right:      vkey = VK_RIGHT;      break;
        case keyboard::Up:         vkey = VK_UP;         break;
        case keyboard::Down:       vkey = VK_DOWN;       break;
        case keyboard::Numpad0:    vkey = VK_NUMPAD0;    break;
        case keyboard::Numpad1:    vkey = VK_NUMPAD1;    break;
        case keyboard::Numpad2:    vkey = VK_NUMPAD2;    break;
        case keyboard::Numpad3:    vkey = VK_NUMPAD3;    break;
        case keyboard::Numpad4:    vkey = VK_NUMPAD4;    break;
        case keyboard::Numpad5:    vkey = VK_NUMPAD5;    break;
        case keyboard::Numpad6:    vkey = VK_NUMPAD6;    break;
        case keyboard::Numpad7:    vkey = VK_NUMPAD7;    break;
        case keyboard::Numpad8:    vkey = VK_NUMPAD8;    break;
        case keyboard::Numpad9:    vkey = VK_NUMPAD9;    break;
        case keyboard::F1:         vkey = VK_F1;         break;
        case keyboard::F2:         vkey = VK_F2;         break;
        case keyboard::F3:         vkey = VK_F3;         break;
        case keyboard::F4:         vkey = VK_F4;         break;
        case keyboard::F5:         vkey = VK_F5;         break;
        case keyboard::F6:         vkey = VK_F6;         break;
        case keyboard::F7:         vkey = VK_F7;         break;
        case keyboard::F8:         vkey = VK_F8;         break;
        case keyboard::F9:         vkey = VK_F9;         break;
        case keyboard::F10:        vkey = VK_F10;        break;
        case keyboard::F11:        vkey = VK_F11;        break;
        case keyboard::F12:        vkey = VK_F12;        break;
        case keyboard::F13:        vkey = VK_F13;        break;
        case keyboard::F14:        vkey = VK_F14;        break;
        case keyboard::F15:        vkey = VK_F15;        break;
        case keyboard::Pause:      vkey = VK_PAUSE;      break;
    }

    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}


////////////////////////////////////////////////////////////
void input_impl::set_virtual_keyboard_visible(bool visible)
{
    // Not applicable
}


////////////////////////////////////////////////////////////
bool input_impl::is_mouse_button_pressed(mouse::button button)
{
    int vkey = 0;
    switch (button)
    {
        case mouse::left:     vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_RBUTTON : VK_LBUTTON; break;
        case mouse::right:    vkey = GetSystemMetrics(SM_SWAPBUTTON) ? VK_LBUTTON : VK_RBUTTON; break;
        case mouse::middle:   vkey = VK_MBUTTON;  break;
        case mouse::x_button1: vkey = VK_XBUTTON1; break;
        case mouse::x_button2: vkey = VK_XBUTTON2; break;
        default:              vkey = 0;           break;
    }

    return (GetAsyncKeyState(vkey) & 0x8000) != 0;
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> input_impl::get_mouse_position()
{
    POINT point;
    GetCursorPos(&point);
	return std::array<std::int32_t, 2>({ point.x, point.y });
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> input_impl::get_mouse_position(const window& relativeTo)
{
    window_handle handle = relativeTo.get_system_handle();
    if (handle)
    {
        POINT point;
        GetCursorPos(&point);
        ScreenToClient(handle, &point);
		return std::array<std::int32_t, 2>({ point.x, point.y });
    }
    else
    {
		return std::array<std::int32_t, 2>({ 0, 0 });
    }
}


////////////////////////////////////////////////////////////
void input_impl::set_mouse_position(const std::array<std::int32_t, 2>& position)
{
    SetCursorPos(position[0], position[1]);
}


////////////////////////////////////////////////////////////
void input_impl::set_mouse_position(const std::array<std::int32_t, 2>& position, const window& relativeTo)
{
    window_handle handle = relativeTo.get_system_handle();
    if (handle)
    {
        POINT point = {position[0], position[1]};
        ClientToScreen(handle, &point);
        SetCursorPos(point.x, point.y);
    }
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
	return std::array<std::int32_t, 2>({ 0, 0 });
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> input_impl::get_touch_position(unsigned int /*finger*/, const window& /*relativeTo*/)
{
    // Not applicable
	return std::array<std::int32_t, 2>({ 0, 0 });
}

} // namespace priv

} // namespace mml
