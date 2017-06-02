#ifndef MML_VIDEOMODE_HPP
#define MML_VIDEOMODE_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/export.hpp>
#include <vector>


namespace mml
{
////////////////////////////////////////////////////////////
/// \brief video_mode defines a video mode (width, height, bpp)
///
////////////////////////////////////////////////////////////
class MML_WINDOW_API video_mode
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// This constructors initializes all members to 0.
    ///
    ////////////////////////////////////////////////////////////
    video_mode();

    ////////////////////////////////////////////////////////////
    /// \brief Construct the video mode with its attributes
    ///
    /// \param modeWidth        Width in pixels
    /// \param modeHeight       Height in pixels
    /// \param modeBitsPerPixel Pixel depths in bits per pixel
    ///
    ////////////////////////////////////////////////////////////
    video_mode(unsigned int modeWidth, unsigned int modeHeight, unsigned int modeBitsPerPixel = 32);

    ////////////////////////////////////////////////////////////
    /// \brief Get the current desktop video mode
    ///
    /// \return Current desktop video mode
    ///
    ////////////////////////////////////////////////////////////
    static video_mode get_desktop_mode();

    ////////////////////////////////////////////////////////////
    /// \brief Retrieve all the video modes supported in fullscreen mode
    ///
    /// When creating a fullscreen window, the video mode is restricted
    /// to be compatible with what the graphics driver and monitor
    /// support. This function returns the complete list of all video
    /// modes that can be used in fullscreen mode.
    /// The returned array is sorted from best to worst, so that
    /// the first element will always give the best mode (higher
    /// width, height and bits-per-pixel).
    ///
    /// \return Array containing all the supported fullscreen modes
    ///
    ////////////////////////////////////////////////////////////
    static const std::vector<video_mode>& get_fullscreen_modes();

    ////////////////////////////////////////////////////////////
    /// \brief Tell whether or not the video mode is valid
    ///
    /// The validity of video modes is only relevant when using
    /// fullscreen windows; otherwise any video mode can be used
    /// with no restriction.
    ///
    /// \return True if the video mode is valid for fullscreen mode
    ///
    ////////////////////////////////////////////////////////////
    bool is_valid() const;

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    unsigned int width;        ///< Video mode width, in pixels
    unsigned int height;       ///< Video mode height, in pixels
    unsigned int bits_per_pixel; ///< Video mode pixel depth, in bits per pixels
};

////////////////////////////////////////////////////////////
/// \relates video_mode
/// \brief Overload of == operator to compare two video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if modes are equal
///
////////////////////////////////////////////////////////////
MML_WINDOW_API bool operator ==(const video_mode& left, const video_mode& right);

////////////////////////////////////////////////////////////
/// \relates video_mode
/// \brief Overload of != operator to compare two video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if modes are different
///
////////////////////////////////////////////////////////////
MML_WINDOW_API bool operator !=(const video_mode& left, const video_mode& right);

////////////////////////////////////////////////////////////
/// \relates video_mode
/// \brief Overload of < operator to compare video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if \a left is lesser than \a right
///
////////////////////////////////////////////////////////////
MML_WINDOW_API bool operator <(const video_mode& left, const video_mode& right);

////////////////////////////////////////////////////////////
/// \relates video_mode
/// \brief Overload of > operator to compare video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if \a left is greater than \a right
///
////////////////////////////////////////////////////////////
MML_WINDOW_API bool operator >(const video_mode& left, const video_mode& right);

////////////////////////////////////////////////////////////
/// \relates video_mode
/// \brief Overload of <= operator to compare video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if \a left is lesser or equal than \a right
///
////////////////////////////////////////////////////////////
MML_WINDOW_API bool operator <=(const video_mode& left, const video_mode& right);

////////////////////////////////////////////////////////////
/// \relates video_mode
/// \brief Overload of >= operator to compare video modes
///
/// \param left  Left operand (a video mode)
/// \param right Right operand (a video mode)
///
/// \return True if \a left is greater or equal than \a right
///
////////////////////////////////////////////////////////////
MML_WINDOW_API bool operator >=(const video_mode& left, const video_mode& right);

} // namespace mml


#endif // MML_VIDEOMODE_HPP


////////////////////////////////////////////////////////////
/// \class mml::video_mode
/// \ingroup window
///
/// A video mode is defined by a width and a height (in pixels)
/// and a depth (in bits per pixel). Video modes are used to
/// setup windows (mml::window) at creation time.
///
/// The main usage of video modes is for fullscreen mode:
/// indeed you must use one of the valid video modes
/// allowed by the OS (which are defined by what the monitor
/// and the graphics card support), otherwise your window
/// creation will just fail.
///
/// mml::video_mode provides a static function for retrieving
/// the list of all the video modes supported by the system:
/// get_fullscreen_modes().
///
/// A custom video mode can also be checked directly for
/// fullscreen compatibility with its is_valid() function.
///
/// Additionally, mml::video_mode provides a static function
/// to get the mode currently used by the desktop: get_desktop_mode().
/// This allows to build windows with the same size or pixel
/// depth as the current resolution.
///
/// Usage example:
/// \code
/// // Display the list of all the video modes available for fullscreen
/// std::vector<mml::video_mode> modes = mml::video_mode::get_fullscreen_modes();
/// for (std::size_t i = 0; i < modes.size(); ++i)
/// {
///     mml::video_mode mode = modes[i];
///     std::cout << "Mode #" << i << ": "
///               << mode.width << "x" << mode.height << " - "
///               << mode.bits_per_pixel << " bpp" << std::endl;
/// }
///
/// // Create a window with the same pixel depth as the desktop
/// mml::video_mode desktop = mml::video_mode::get_desktop_mode();
/// window.create(mml::video_mode(1024, 768, desktop.bits_per_pixel), "mml window");
/// \endcode
///
////////////////////////////////////////////////////////////
