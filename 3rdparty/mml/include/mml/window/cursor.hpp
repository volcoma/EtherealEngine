#ifndef MML_CURSOR_HPP
#define MML_CURSOR_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/export.hpp>
#include <mml/system/non_copyable.hpp>
#include <array>
#include <cstdint>

namespace mml
{
namespace priv
{
    class cursor_impl;
}

////////////////////////////////////////////////////////////
/// \brief cursor defines the appearance of a system cursor
///
////////////////////////////////////////////////////////////
class MML_WINDOW_API cursor : non_copyable
{
public:

    ////////////////////////////////////////////////////////////
    /// \brief Enumeration of the native system cursor types
    ///
    /// Refer to the following table to determine which cursor
    /// is available on which platform.
    ///
    ///  Type								| Linux | Mac OS X | Windows
    /// ------------------------------------|:-----:|:--------:|:--------:
    ///  cursor::arrow						|  yes  |    yes   |   yes
    ///  cursor::arrow_wait					|  no   |    no    |   yes
    ///  cursor::wait						|  yes  |    no    |   yes
    ///  cursor::text						|  yes  |    yes   |   yes
    ///  cursor::hand						|  yes  |    yes   |   yes
    ///  cursor::size_horizontal			|  yes  |    yes   |   yes
    ///  cursor::size_vertical				|  yes  |    yes   |   yes
    ///  cursor::size_top_left_bottom_right |  no   |    no    |   yes
    ///  cursor::size_bottom_left_top_right |  no   |    no    |   yes
    ///  cursor::size_all					|  yes  |    no    |   yes
    ///  cursor::cross						|  yes  |    yes   |   yes
    ///  cursor::help						|  yes  |    no    |   yes
    ///  cursor::not_allowed				|  yes  |    yes   |   yes
    ///
    ////////////////////////////////////////////////////////////
    enum type
    {
        arrow,                  ///< arrow cursor (default)
        arrow_wait,              ///< Busy arrow cursor
        wait,                   ///< Busy cursor
        text,                   ///< I-beam, cursor when hovering over a field allowing text entry
        hand,                   ///< Pointing hand cursor
        size_horizontal,         ///< Horizontal double arrow cursor
        size_vertical,           ///< Vertical double arrow cursor
        size_top_left_bottom_right, ///< Double arrow cursor going from top-left to bottom-right
        size_bottom_left_top_right, ///< Double arrow cursor going from bottom-left to top-right
        size_all,                ///< Combination of SizeHorizontal and SizeVertical
        cross,                  ///< Crosshair cursor
        help,                   ///< Help cursor
        not_allowed              ///< Action not allowed cursor
    };

public:

    ////////////////////////////////////////////////////////////
    /// \brief Default constructor
    ///
    /// This constructor doesn't actually create the cursor;
    /// initially the new instance is invalid and must not be
    /// used until either load_from_pixels() or load_from_system()
    /// is called and successfully created a cursor.
    ///
    ////////////////////////////////////////////////////////////
    cursor();

    ////////////////////////////////////////////////////////////
    /// \brief Destructor
    ///
    /// This destructor releases the system resources
    /// associated with this cursor, if any.
    ///
    ////////////////////////////////////////////////////////////
    ~cursor();

    ////////////////////////////////////////////////////////////
    /// \brief Create a cursor with the provided image
    ///
    /// \a pixels must be an array of \a width by \a height pixels
    /// in 32-bit RGBA format. If not, this will cause undefined behavior.
    ///
    /// If \a pixels is null or either \a width or \a height are 0,
    /// the current cursor is left unchanged and the function will
    /// return false.
    ///
    /// In addition to specifying the pixel data, you can also
    /// specify the location of the hotspot of the cursor. The
    /// hotspot is the pixel coordinate within the cursor image
    /// which will be located exactly where the mouse pointer
    /// position is. Any mouse actions that are performed will
    /// return the window/screen location of the hotspot.
    ///
    /// \warning On Unix, the pixels are mapped into a monochrome
    ///          bitmap: pixels with an alpha channel to 0 are
    ///          transparent, black if the RGB channel are close
    ///          to zero, and white otherwise.
    ///
    /// \param pixels   Array of pixels of the image
    /// \param size     Width and height of the image
    /// \param hotspot  (x,y) location of the hotspot
    /// \return true if the cursor was successfully loaded;
    ///         false otherwise
    ///
    ////////////////////////////////////////////////////////////
    bool load_from_pixels(const std::uint8_t* pixels, const std::array<std::uint32_t, 2>& size, const std::array<std::uint32_t, 2>& hotspot);

    ////////////////////////////////////////////////////////////
    /// \brief Create a native system cursor
    ///
    /// Refer to the list of cursor available on each system
    /// (see mml::cursor::Type) to know whether a given cursor is
    /// expected to load successfully or is not supported by
    /// the operating system.
    ///
    /// \param type Native system cursor type
    /// \return true if and only if the corresponding cursor is
    ///         natively supported by the operating system;
    ///         false otherwise
    ///
    ////////////////////////////////////////////////////////////
    bool load_from_system(type t);

private:

    friend class window;

    ////////////////////////////////////////////////////////////
    /// \brief Get access to the underlying implementation
    ///
    /// This is primarily designed for mml::window::set_mouse_cursor,
    /// hence the friendship.
    ///
    /// \return a reference to the OS-specific implementation
    ///
    ////////////////////////////////////////////////////////////
    const priv::cursor_impl& get_impl() const;

private:

    ////////////////////////////////////////////////////////////
    // Member data
    ////////////////////////////////////////////////////////////
    priv::cursor_impl* _impl; ///< Platform-specific implementation of the cursor
};

} // namespace mml


#endif // MML_CURSOR_HPP


////////////////////////////////////////////////////////////
/// \class mml::cursor
/// \ingroup window
///
/// \warning Features related to cursor are not supported on
///          iOS and Android.
///
/// This class abstracts the operating system resources
/// associated with either a native system cursor or a custom
/// cursor.
///
/// After loading the cursor the graphical appearance
/// with either load_from_pixels() or load_from_system(), the
/// cursor can be changed with mml::window::set_mouse_cursor().
///
/// The behaviour is undefined if the cursor is destroyed while
/// in use by the window.
///
/// Usage example:
/// \code
/// mml::window window;
///
/// // ... create window as usual ...
///
/// mml::cursor cursor;
/// if (cursor.load_from_system(mml::cursor::Hand))
///     window.set_mouse_cursor(cursor);
/// \endcode
///
/// \see mml::window::set_mouse_cursor
///
////////////////////////////////////////////////////////////
