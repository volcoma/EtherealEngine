#ifndef MML_WINDOWSTYLE_HPP
#define MML_WINDOWSTYLE_HPP


namespace mml
{
namespace style
{
    ////////////////////////////////////////////////////////////
    /// \ingroup window
    /// \brief Enumeration of the window styles
    ///
    ////////////////////////////////////////////////////////////
    enum
    {
        none       = 0,      ///< No border / title bar (this flag and all others are mutually exclusive)
        titlebar   = 1 << 0, ///< Title bar + fixed border
        resize     = 1 << 1, ///< Title bar + resizable border + maximize button
        close      = 1 << 2, ///< Title bar + close button
        fullscreen = 1 << 3, ///< Fullscreen mode (this flag and all others are mutually exclusive)

        standard = titlebar | resize | close ///< standard window style
    };
}

} // namespace mml


#endif // MML_WINDOWSTYLE_HPP
