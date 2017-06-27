////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/unix/display.hpp>
#include <mml/system/err.hpp>
#include <X11/keysym.h>
#include <cassert>
#include <cstdlib>
#include <map>
#include <mutex>

namespace
{
    // The shared display and its reference counter
    Display* sharedDisplay = NULL;
    unsigned int referenceCount = 0;
    std::mutex mutex;

    typedef std::map<std::string, Atom> AtomMap;
    AtomMap atoms;
}

namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
Display* open_display()
{
    static bool first = true;
    if(first)
    {
        XInitThreads();
        first = false;
    }

	std::lock_guard<std::mutex> lock(mutex);

    if (referenceCount == 0)
    {
        sharedDisplay = XOpenDisplay(NULL);

        // Opening display failed: The best we can do at the moment is to output a meaningful error message
        // and cause an abnormal program termination
        if (!sharedDisplay)
        {
            err() << "Failed to open X11 display; make sure the DISPLAY environment variable is set correctly" << std::endl;
            std::abort();
        }
    }

    referenceCount++;
    return sharedDisplay;
}


////////////////////////////////////////////////////////////
void close_display(Display* display)
{
	std::lock_guard<std::mutex> lock(mutex);

    assert(display == sharedDisplay);

    referenceCount--;
    if (referenceCount == 0)
        XCloseDisplay(display);
}


////////////////////////////////////////////////////////////
Atom get_atom(const std::string& name, bool onlyIfExists)
{
    AtomMap::const_iterator iter = atoms.find(name);

    if (iter != atoms.end())
        return iter->second;

    Display* display = open_display();

    Atom atom = XInternAtom(display, name.c_str(), onlyIfExists ? True : False);

    close_display(display);

    atoms[name] = atom;

    return atom;
}

} // namespace priv

} // namespace mml
