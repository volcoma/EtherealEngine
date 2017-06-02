#ifndef MML_SHAREDDISPLAY_HPP
#define MML_SHAREDDISPLAY_HPP

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <X11/Xlib.h>
#include <string>


namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
/// \brief Get the shared Display
///
/// This function increments the reference count of the display,
/// it must be matched with a call to close_display.
///
/// \return Pointer to the shared display
///
////////////////////////////////////////////////////////////
Display* open_display();

////////////////////////////////////////////////////////////
/// \brief Release a reference to the shared display
///
/// \param display Display to release
///
////////////////////////////////////////////////////////////
void close_display(Display* display);

////////////////////////////////////////////////////////////
/// \brief Get the atom with the specified name
///
/// \param name         Name of the atom
/// \param onlyIfExists Don't try to create the atom if it doesn't already exist
///
/// \return Atom if it exists or None (0) if it doesn't
///
////////////////////////////////////////////////////////////
Atom get_atom(const std::string& name, bool onlyIfExists = false);

} // namespace priv

} // namespace mml


#endif // MML_SHAREDDISPLAY_HPP
