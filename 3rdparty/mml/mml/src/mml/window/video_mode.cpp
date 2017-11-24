////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/video_mode.hpp>
#include <mml/window/video_mode_impl.hpp>
#include <algorithm>
#include <functional>


namespace mml
{
////////////////////////////////////////////////////////////
video_mode::video_mode() :
width       (0),
height      (0),
bits_per_pixel(0)
{

}


////////////////////////////////////////////////////////////
video_mode::video_mode(unsigned int modeWidth, unsigned int modeHeight, unsigned int modeBitsPerPixel) :
width       (modeWidth),
height      (modeHeight),
bits_per_pixel(modeBitsPerPixel)
{

}


////////////////////////////////////////////////////////////
video_mode video_mode::get_desktop_mode()
{
    // Directly forward to the OS-specific implementation
    return priv::video_mode_impl::get_desktop_mode();
}


////////////////////////////////////////////////////////////
const std::vector<video_mode>& video_mode::get_fullscreen_modes()
{
    static std::vector<video_mode> modes;

    // Populate the array on first call
    if (modes.empty())
    {
        modes = priv::video_mode_impl::get_fullscreen_modes();
        std::sort(modes.begin(), modes.end(), std::greater<video_mode>());
    }

    return modes;
}


////////////////////////////////////////////////////////////
bool video_mode::is_valid() const
{
    const std::vector<video_mode>& modes = get_fullscreen_modes();

    return std::find(modes.begin(), modes.end(), *this) != modes.end();
}


////////////////////////////////////////////////////////////
bool operator ==(const video_mode& left, const video_mode& right)
{
    return (left.width        == right.width)        &&
           (left.height       == right.height)       &&
           (left.bits_per_pixel == right.bits_per_pixel);
}


////////////////////////////////////////////////////////////
bool operator !=(const video_mode& left, const video_mode& right)
{
    return !(left == right);
}


////////////////////////////////////////////////////////////
bool operator <(const video_mode& left, const video_mode& right)
{
    if (left.bits_per_pixel == right.bits_per_pixel)
    {
        if (left.width == right.width)
        {
            return left.height < right.height;
        }
        else
        {
            return left.width < right.width;
        }
    }
    else
    {
        return left.bits_per_pixel < right.bits_per_pixel;
    }
}


////////////////////////////////////////////////////////////
bool operator >(const video_mode& left, const video_mode& right)
{
    return right < left;
}


////////////////////////////////////////////////////////////
bool operator <=(const video_mode& left, const video_mode& right)
{
    return !(right < left);
}


////////////////////////////////////////////////////////////
bool operator >=(const video_mode& left, const video_mode& right)
{
    return !(left < right);
}

} // namespace mml
