////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/joystick_impl.hpp>
#include <mml/system/err.hpp>
#include <mml/system/utf.hpp>
#include <windows.h>
#include <tchar.h>
#include <regstr.h>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <string>
#include <chrono>

namespace
{
    struct connection_cache
    {
        connection_cache() : connected(false) {}
        bool connected;
		std::chrono::system_clock::time_point time;
    };
	const std::chrono::milliseconds connectionRefreshDelay = std::chrono::milliseconds(500);

    connection_cache connectionCache[mml::joystick::count];

    // If true, will only update when WM_DEVICECHANGE message is received
    bool lazyUpdates = false;

    // Get a system error string from an error code
    std::string get_error_string(DWORD error)
    {
        PTCHAR buffer;

        if (FormatMessage(FORMAT_MESSAGE_MAX_WIDTH_MASK | FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, error, 0, reinterpret_cast<PTCHAR>(&buffer), 0, NULL) == 0)
            return "Unknown error.";

		std::string message;
		std::size_t length = std::wcslen(buffer);
		if (length > 0)
		{
			message.reserve(length + 1);
			mml::utf32::from_wide(buffer, buffer + length, std::back_inserter(message));
		}
	
        LocalFree(buffer);
        return message;
    }

    // Get the joystick's name
    std::string get_device_name(unsigned int index, JOYCAPS caps)
    {
        // Give the joystick a default name
		std::string joystickDescription = "Unknown joystick";

        LONG result;
        HKEY rootKey;
        HKEY currentKey;
        std::basic_string<TCHAR> subkey;

        subkey  = REGSTR_PATH_JOYCONFIG;
        subkey += TEXT('\\');
        subkey += caps.szRegKey;
        subkey += TEXT('\\');
        subkey += REGSTR_KEY_JOYCURR;

        rootKey = HKEY_CURRENT_USER;
        result  = RegOpenKeyEx(rootKey, subkey.c_str(), 0, KEY_READ, &currentKey);

        if (result != ERROR_SUCCESS)
        {
            rootKey = HKEY_LOCAL_MACHINE;
            result  = RegOpenKeyEx(rootKey, subkey.c_str(), 0, KEY_READ, &currentKey);

            if (result != ERROR_SUCCESS)
            {
                mml::err() << "Unable to open registry for joystick at index " << index << ": " << get_error_string(result) << std::endl;
                return joystickDescription;
            }
        }

        std::basic_ostringstream<TCHAR> indexString;
        indexString << index + 1;

        subkey  = TEXT("joystick");
        subkey += indexString.str();
        subkey += REGSTR_VAL_JOYOEMNAME;

        TCHAR keyData[256];
        DWORD keyDataSize = sizeof(keyData);

        result = RegQueryValueEx(currentKey, subkey.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(keyData), &keyDataSize);
        RegCloseKey(currentKey);

        if (result != ERROR_SUCCESS)
        {
            mml::err() << "Unable to query registry key for joystick at index " << index << ": " << get_error_string(result) << std::endl;
            return joystickDescription;
        }

        subkey  = REGSTR_PATH_JOYOEM;
        subkey += TEXT('\\');
        subkey.append(keyData, keyDataSize / sizeof(TCHAR));

        result = RegOpenKeyEx(rootKey, subkey.c_str(), 0, KEY_READ, &currentKey);

        if (result != ERROR_SUCCESS)
        {
            mml::err() << "Unable to open registry key for joystick at index " << index << ": " << get_error_string(result) << std::endl;
            return joystickDescription;
        }

        keyDataSize = sizeof(keyData);

        result = RegQueryValueEx(currentKey, REGSTR_VAL_JOYOEMNAME, NULL, NULL, reinterpret_cast<LPBYTE>(keyData), &keyDataSize);
        RegCloseKey(currentKey);

        if (result != ERROR_SUCCESS)
        {
			mml::err() << "Unable to query name for joystick at index " << index << ": " << get_error_string(result) << std::endl;
            return joystickDescription;
        }

        keyData[255] = TEXT('\0'); // Ensure null terminator in case the data is too long.
        
		std::string data;
		std::size_t length = std::wcslen(keyData);
		if (length > 0)
		{
			data.reserve(length + 1);
			mml::utf32::from_wide(&keyData[0], &keyData[0] + length, std::back_inserter(data));
		}

		joystickDescription = data;

        return joystickDescription;
    }
}

namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
void joystick_impl::initialize()
{
    // Perform the initial scan and populate the connection cache
    update_connections();
}


////////////////////////////////////////////////////////////
void joystick_impl::cleanup()
{
    // Nothing to do
}


////////////////////////////////////////////////////////////
bool joystick_impl::is_connected(unsigned int index)
{
    connection_cache& cache = connectionCache[index];
	auto now = std::chrono::system_clock::now();
	std::chrono::milliseconds delay = std::chrono::duration_cast<std::chrono::milliseconds>(now - cache.time);
    if (!lazyUpdates && delay > connectionRefreshDelay)
    {
        JOYINFOEX joyInfo;
        joyInfo.dwSize = sizeof(joyInfo);
        joyInfo.dwFlags = 0;
        cache.connected = joyGetPosEx(JOYSTICKID1 + index, &joyInfo) == JOYERR_NOERROR;

		cache.time = now;
    }
    return cache.connected;
}

////////////////////////////////////////////////////////////
void joystick_impl::set_lazy_updates(bool status)
{
    lazyUpdates = status;
}

////////////////////////////////////////////////////////////
void joystick_impl::update_connections()
{
    for (unsigned int i = 0; i < joystick::count; ++i)
    {
        JOYINFOEX joyInfo;
        joyInfo.dwSize = sizeof(joyInfo);
        joyInfo.dwFlags = 0;
        connection_cache& cache = connectionCache[i];
        cache.connected = joyGetPosEx(JOYSTICKID1 + i, &joyInfo) == JOYERR_NOERROR;

		cache.time = std::chrono::system_clock::now();
    }
}

////////////////////////////////////////////////////////////
bool joystick_impl::open(unsigned int index)
{
    // No explicit "open" action is required
    _index = JOYSTICKID1 + index;

    // Store the joystick capabilities
    bool success = joyGetDevCaps(_index, &_caps, sizeof(_caps)) == JOYERR_NOERROR;

    if (success)
    {
        _identification.name      = get_device_name(_index, _caps);
        _identification.product_id = _caps.wPid;
        _identification.vendor_id  = _caps.wMid;
    }

    return success;
}


////////////////////////////////////////////////////////////
void joystick_impl::close()
{
    // Nothing to do
}

////////////////////////////////////////////////////////////
joystick_caps joystick_impl::get_capabilities() const
{
    joystick_caps caps;

    caps.button_count = _caps.wNumButtons;
    if (caps.button_count > joystick::button_count)
        caps.button_count = joystick::button_count;

    caps.axes[joystick::X]    = true;
    caps.axes[joystick::Y]    = true;
    caps.axes[joystick::Z]    = (_caps.wCaps & JOYCAPS_HASZ) != 0;
    caps.axes[joystick::R]    = (_caps.wCaps & JOYCAPS_HASR) != 0;
    caps.axes[joystick::U]    = (_caps.wCaps & JOYCAPS_HASU) != 0;
    caps.axes[joystick::V]    = (_caps.wCaps & JOYCAPS_HASV) != 0;
    caps.axes[joystick::PovX] = (_caps.wCaps & JOYCAPS_HASPOV) != 0;
    caps.axes[joystick::PovY] = (_caps.wCaps & JOYCAPS_HASPOV) != 0;

    return caps;
}


////////////////////////////////////////////////////////////
joystick::identification joystick_impl::get_identification() const
{
    return _identification;
}


////////////////////////////////////////////////////////////
joystick_state joystick_impl::update()
{
    joystick_state state;

    // Get the current joystick state
    JOYINFOEX pos;
    pos.dwFlags  = JOY_RETURNX | JOY_RETURNY | JOY_RETURNZ | JOY_RETURNR | JOY_RETURNU | JOY_RETURNV | JOY_RETURNBUTTONS;
    pos.dwFlags |= (_caps.wCaps & JOYCAPS_POVCTS) ? JOY_RETURNPOVCTS : JOY_RETURNPOV;
    pos.dwSize   = sizeof(JOYINFOEX);
    if (joyGetPosEx(_index, &pos) == JOYERR_NOERROR)
    {
        // The joystick is connected
        state.connected = true;

        // Axes
        state.axes[joystick::X] = (pos.dwXpos - (_caps.wXmax + _caps.wXmin) / 2.f) * 200.f / (_caps.wXmax - _caps.wXmin);
        state.axes[joystick::Y] = (pos.dwYpos - (_caps.wYmax + _caps.wYmin) / 2.f) * 200.f / (_caps.wYmax - _caps.wYmin);
        state.axes[joystick::Z] = (pos.dwZpos - (_caps.wZmax + _caps.wZmin) / 2.f) * 200.f / (_caps.wZmax - _caps.wZmin);
        state.axes[joystick::R] = (pos.dwRpos - (_caps.wRmax + _caps.wRmin) / 2.f) * 200.f / (_caps.wRmax - _caps.wRmin);
        state.axes[joystick::U] = (pos.dwUpos - (_caps.wUmax + _caps.wUmin) / 2.f) * 200.f / (_caps.wUmax - _caps.wUmin);
        state.axes[joystick::V] = (pos.dwVpos - (_caps.wVmax + _caps.wVmin) / 2.f) * 200.f / (_caps.wVmax - _caps.wVmin);

        // Special case for POV, it is given as an angle
        if (pos.dwPOV != 0xFFFF)
        {
            float angle = pos.dwPOV / 18000.f * 3.141592654f;
            state.axes[joystick::PovX] = std::sin(angle) * 100;
            state.axes[joystick::PovY] = std::cos(angle) * 100;
        }
        else
        {
            state.axes[joystick::PovX] = 0;
            state.axes[joystick::PovY] = 0;
        }

        // Buttons
        for (unsigned int i = 0; i < joystick::button_count; ++i)
            state.buttons[i] = (pos.dwButtons & (1 << i)) != 0;
    }

    return state;
}

} // namespace priv

} // namespace mml
