////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/video_mode_impl.hpp>
#include <windows.h>
#include <algorithm>


namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
std::vector<video_mode> video_mode_impl::get_fullscreen_modes()
{
    std::vector<video_mode> modes;

    // Enumerate all available video modes for the primary display adapter
    DEVMODE win32Mode;
    win32Mode.dmSize = sizeof(win32Mode);
    for (int count = 0; EnumDisplaySettings(NULL, count, &win32Mode); ++count)
    {
        // Convert to mml::video_mode
        video_mode mode(win32Mode.dmPelsWidth, win32Mode.dmPelsHeight, win32Mode.dmBitsPerPel);

        // Add it only if it is not already in the array
        if (std::find(modes.begin(), modes.end(), mode) == modes.end())
            modes.push_back(mode);
    }

    return modes;
}


////////////////////////////////////////////////////////////
video_mode video_mode_impl::get_desktop_mode()
{
    DEVMODE win32Mode;
    win32Mode.dmSize = sizeof(win32Mode);
    EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &win32Mode);

    return video_mode(win32Mode.dmPelsWidth, win32Mode.dmPelsHeight, win32Mode.dmBitsPerPel);
}

} // namespace priv

} // namespace mml
