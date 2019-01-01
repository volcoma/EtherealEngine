////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/window_style.hpp> // important to be included first (conflict with None)
#include <mml/window/unix/window_impl_x11.hpp>
#include <mml/window/unix/clipboard_impl.hpp>
#include <mml/window/unix/display.hpp>
#include <mml/window/unix/input_impl.hpp>
#include <mml/system/utf.hpp>
#include <mml/system/err.hpp>
#include <X11/Xlibint.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include <X11/keysym.h>
#include <X11/extensions/Xrandr.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <libgen.h>
#include <fcntl.h>
#include <algorithm>
#include <vector>
#include <string>
#include <cstring>

#undef max
#undef min
#include <thread>
#include <mutex>

#define _NET_WM_STATE_REMOVE    0l
#define _NET_WM_STATE_ADD       1l
#define _NET_WM_STATE_TOGGLE    2l
////////////////////////////////////////////////////////////
// Private data
////////////////////////////////////////////////////////////
namespace
{
    mml::priv::window_impl_x11*              fullscreenWindow = NULL;
	std::vector<mml::priv::window_impl_x11*> allWindows;
	std::mutex                             allWindowsMutex;
	std::string                            windowManagerName;
    std::string                            wmAbsPosGood[] = { "Enlightenment", "FVWM", "i3" };
	static const unsigned long            eventMask = FocusChangeMask      | ButtonPressMask     |
	                                                  ButtonReleaseMask    | ButtonMotionMask    |
	                                                  PointerMotionMask    | KeyPressMask        |
	                                                  KeyReleaseMask       | StructureNotifyMask |
	                                                  EnterWindowMask      | LeaveWindowMask     |
	                                                  VisibilityChangeMask | PropertyChangeMask;

	static const unsigned int             maxTrialsCount = 5;

    // Predicate we use to find key repeat events in processEvent
    struct KeyRepeatFinder
    {
        KeyRepeatFinder(unsigned int keycode, Time time) : keycode(keycode), time(time) {}

        // Predicate operator that checks event type, keycode and timestamp
        bool operator()(const XEvent& event)
        {
            return ((event.type == KeyPress) && (event.xkey.keycode == keycode) && (event.xkey.time - time < 2));
        }

        unsigned int keycode;
        Time time;
    };
	// Filter the events received by windows (only allow those matching a specific window)
	Bool checkEvent(::Display*, XEvent* event, XPointer userData)
	{
		// Just check if the event matches the window
		return event->xany.window == reinterpret_cast< ::Window >(userData);
	}

	// Find the name of the current executable
	std::string findExecutableName()
	{
		// We use /proc/self/cmdline to get the command line
		// the user used to invoke this instance of the application
		int file = ::open("/proc/self/cmdline", O_RDONLY | O_NONBLOCK);

		if (file < 0)
			return "sfml";

		std::vector<char> buffer(256, 0);
		std::size_t offset = 0;
		ssize_t result = 0;

		while ((result = read(file, &buffer[offset], 256)) > 0)
		{
			buffer.resize(buffer.size() + result, 0);
			offset += result;
		}

		::close(file);

		if (offset)
		{
			buffer[offset] = 0;

			// Remove the path to keep the executable name only
			return basename(&buffer[0]);
		}

		// Default fallback name
		return "mml";
	}

	// Check if Extended window Manager Hints are supported
	bool ewmhSupported()
	{
		static bool checked = false;
		static bool ewmhSupported = false;

		if (checked)
			return ewmhSupported;

		checked = true;

		Atom netSupportingWmCheck = mml::priv::get_atom("_NET_SUPPORTING_WM_CHECK", true);
		Atom netSupported = mml::priv::get_atom("_NET_SUPPORTED", true);

		if (!netSupportingWmCheck || !netSupported)
			return false;

		::Display* display = mml::priv::open_display();

		Atom actualType;
		int actualFormat;
		unsigned long numItems;
		unsigned long numBytes;
		unsigned char* data;

		int result = XGetWindowProperty(display,
		                                DefaultRootWindow(display),
		                                netSupportingWmCheck,
		                                0,
		                                1,
		                                False,
		                                XA_WINDOW,
		                                &actualType,
		                                &actualFormat,
		                                &numItems,
		                                &numBytes,
		                                &data);

		if (result != Success || actualType != XA_WINDOW || numItems != 1)
		{
			if(result == Success)
				XFree(data);

			mml::priv::close_display(display);
			return false;
		}

		::Window rootWindow = *reinterpret_cast< ::Window* >(data);

		XFree(data);

		if (!rootWindow)
		{
			mml::priv::close_display(display);
			return false;
		}

		result = XGetWindowProperty(display,
		                            rootWindow,
		                            netSupportingWmCheck,
		                            0,
		                            1,
		                            False,
		                            XA_WINDOW,
		                            &actualType,
		                            &actualFormat,
		                            &numItems,
		                            &numBytes,
		                            &data);

		if (result != Success || actualType != XA_WINDOW || numItems != 1)
		{
			if(result == Success)
				XFree(data);

			mml::priv::close_display(display);
			return false;
		}

		::Window childWindow = *reinterpret_cast< ::Window* >(data);

		XFree(data);

		if (!childWindow)
		{
			mml::priv::close_display(display);
			return false;
		}

		// Conforming window managers should return the same window for both queries
		if (rootWindow != childWindow)
		{
			mml::priv::close_display(display);
			return false;
		}

		ewmhSupported = true;

		// We try to get the name of the window manager
		// for window manager specific workarounds
		Atom netWmName = mml::priv::get_atom("_NET_WM_NAME", true);

		if (!netWmName)
		{
			mml::priv::close_display(display);
			return true;
		}

		Atom utf8StringType = mml::priv::get_atom("UTF8_STRING");

		if (!utf8StringType)
			utf8StringType = XA_STRING;

		result = XGetWindowProperty(display,
		                            rootWindow,
		                            netWmName,
		                            0,
		                            0x7fffffff,
		                            False,
		                            utf8StringType,
		                            &actualType,
		                            &actualFormat,
		                            &numItems,
		                            &numBytes,
		                            &data);

		if (actualType && numItems)
		{
			// It seems the wm name string reply is not necessarily
			// null-terminated. The work around is to get its actual
			// length to build a proper string
			const char* begin = reinterpret_cast<const char*>(data);
			//const char* end = begin + numItems;
			windowManagerName = std::string(begin, numItems);
		}

		if(result == Success)
			XFree(data);

		mml::priv::close_display(display);

		return true;
	}

	// Get the parent window.
    ::Window getParentWindow(::Display* disp, ::Window win)
    {
        ::Window root, parent;
        ::Window* children = NULL;
        unsigned int numChildren;

        XQueryTree(disp, win, &root, &parent, &children, &numChildren);

        // Children information is not used, so must be freed.
        if (children != NULL)
            XFree(children);

        return parent;
    }

    // Get the Frame Extents from EWMH WMs that support it.
    bool getEWMHFrameExtents(::Display* disp, ::Window win,
        long& xFrameExtent, long& yFrameExtent)
    {
        if (!ewmhSupported())
            return false;

        Atom frameExtents = sf::priv::get_atom("_NET_FRAME_EXTENTS", true);

        if (frameExtents == None)
            return false;

        bool gotFrameExtents = false;
        Atom actualType;
        int actualFormat;
        unsigned long numItems;
        unsigned long numBytesLeft;
        unsigned char* data = NULL;

        int result = XGetWindowProperty(disp,
                                        win,
                                        frameExtents,
                                        0,
                                        4,
                                        False,
                                        XA_CARDINAL,
                                        &actualType,
                                        &actualFormat,
                                        &numItems,
                                        &numBytesLeft,
                                        &data);

        if ((result == Success) && (actualType == XA_CARDINAL) &&
            (actualFormat == 32) && (numItems == 4) && (numBytesLeft == 0) &&
            (data != NULL))
        {
            gotFrameExtents = true;

            long* extents = (long*) data;

            xFrameExtent = extents[0]; // Left.
            yFrameExtent = extents[2]; // Top.
        }

        // Always free data.
        if (data != NULL)
            XFree(data);

        return gotFrameExtents;
    }

    // Check if the current WM is in the list of good WMs that provide
    // a correct absolute position for the window when queried.
    bool isWMAbsolutePositionGood()
    {
        // This can only work with EWMH, to get the name.
        if (!ewmhSupported())
            return false;

        for (size_t i = 0; i < (sizeof(wmAbsPosGood) / sizeof(wmAbsPosGood[0])); i++)
        {
            if (wmAbsPosGood[i] == windowManagerName)
                return true;
        }

        return false;
    }

	mml::keyboard::key keysym_to_mml(KeySym symbol)
	{
		switch (symbol)
		{
		    case XK_Shift_L:      return mml::keyboard::LShift;
		    case XK_Shift_R:      return mml::keyboard::RShift;
		    case XK_Control_L:    return mml::keyboard::LControl;
		    case XK_Control_R:    return mml::keyboard::RControl;
		    case XK_Alt_L:        return mml::keyboard::LAlt;
		    case XK_Alt_R:        return mml::keyboard::RAlt;
		    case XK_Super_L:      return mml::keyboard::LSystem;
		    case XK_Super_R:      return mml::keyboard::RSystem;
		    case XK_Menu:         return mml::keyboard::Menu;
		    case XK_Escape:       return mml::keyboard::Escape;
		    case XK_semicolon:    return mml::keyboard::Semicolon;
		    case XK_slash:        return mml::keyboard::Slash;
		    case XK_equal:        return mml::keyboard::Equal;
		    case XK_minus:        return mml::keyboard::Hyphen;
		    case XK_bracketleft:  return mml::keyboard::LBracket;
		    case XK_bracketright: return mml::keyboard::RBracket;
		    case XK_comma:        return mml::keyboard::Comma;
		    case XK_period:       return mml::keyboard::Period;
		    case XK_apostrophe:   return mml::keyboard::Quote;
		    case XK_backslash:    return mml::keyboard::Backslash;
		    case XK_grave:        return mml::keyboard::Tilde;
		    case XK_space:        return mml::keyboard::Space;
		    case XK_Return:       return mml::keyboard::Enter;
		    case XK_KP_Enter:     return mml::keyboard::Enter;
		    case XK_BackSpace:    return mml::keyboard::Backspace;
		    case XK_Tab:          return mml::keyboard::Tab;
		    case XK_Prior:        return mml::keyboard::PageUp;
		    case XK_Next:         return mml::keyboard::PageDown;
		    case XK_End:          return mml::keyboard::End;
		    case XK_Home:         return mml::keyboard::Home;
		    case XK_Insert:       return mml::keyboard::Insert;
		    case XK_Delete:       return mml::keyboard::Delete;
		    case XK_KP_Add:       return mml::keyboard::Add;
		    case XK_KP_Subtract:  return mml::keyboard::Subtract;
		    case XK_KP_Multiply:  return mml::keyboard::Multiply;
		    case XK_KP_Divide:    return mml::keyboard::Divide;
		    case XK_Pause:        return mml::keyboard::Pause;
		    case XK_F1:           return mml::keyboard::F1;
		    case XK_F2:           return mml::keyboard::F2;
		    case XK_F3:           return mml::keyboard::F3;
		    case XK_F4:           return mml::keyboard::F4;
		    case XK_F5:           return mml::keyboard::F5;
		    case XK_F6:           return mml::keyboard::F6;
		    case XK_F7:           return mml::keyboard::F7;
		    case XK_F8:           return mml::keyboard::F8;
		    case XK_F9:           return mml::keyboard::F9;
		    case XK_F10:          return mml::keyboard::F10;
		    case XK_F11:          return mml::keyboard::F11;
		    case XK_F12:          return mml::keyboard::F12;
		    case XK_F13:          return mml::keyboard::F13;
		    case XK_F14:          return mml::keyboard::F14;
		    case XK_F15:          return mml::keyboard::F15;
		    case XK_Left:         return mml::keyboard::Left;
		    case XK_Right:        return mml::keyboard::Right;
		    case XK_Up:           return mml::keyboard::Up;
		    case XK_Down:         return mml::keyboard::Down;
		    case XK_KP_Insert:    return mml::keyboard::Numpad0;
		    case XK_KP_End:       return mml::keyboard::Numpad1;
		    case XK_KP_Down:      return mml::keyboard::Numpad2;
		    case XK_KP_Page_Down: return mml::keyboard::Numpad3;
		    case XK_KP_Left:      return mml::keyboard::Numpad4;
		    case XK_KP_Begin:     return mml::keyboard::Numpad5;
		    case XK_KP_Right:     return mml::keyboard::Numpad6;
		    case XK_KP_Home:      return mml::keyboard::Numpad7;
		    case XK_KP_Up:        return mml::keyboard::Numpad8;
		    case XK_KP_Page_Up:   return mml::keyboard::Numpad9;
		    case XK_a:            return mml::keyboard::A;
		    case XK_b:            return mml::keyboard::B;
		    case XK_c:            return mml::keyboard::C;
		    case XK_d:            return mml::keyboard::D;
		    case XK_e:            return mml::keyboard::E;
		    case XK_f:            return mml::keyboard::F;
		    case XK_g:            return mml::keyboard::G;
		    case XK_h:            return mml::keyboard::H;
		    case XK_i:            return mml::keyboard::I;
		    case XK_j:            return mml::keyboard::J;
		    case XK_k:            return mml::keyboard::K;
		    case XK_l:            return mml::keyboard::L;
		    case XK_m:            return mml::keyboard::M;
		    case XK_n:            return mml::keyboard::N;
		    case XK_o:            return mml::keyboard::O;
		    case XK_p:            return mml::keyboard::P;
		    case XK_q:            return mml::keyboard::Q;
		    case XK_r:            return mml::keyboard::R;
		    case XK_s:            return mml::keyboard::S;
		    case XK_t:            return mml::keyboard::T;
		    case XK_u:            return mml::keyboard::U;
		    case XK_v:            return mml::keyboard::V;
		    case XK_w:            return mml::keyboard::W;
		    case XK_x:            return mml::keyboard::X;
		    case XK_y:            return mml::keyboard::Y;
		    case XK_z:            return mml::keyboard::Z;
		    case XK_0:            return mml::keyboard::Num0;
		    case XK_1:            return mml::keyboard::Num1;
		    case XK_2:            return mml::keyboard::Num2;
		    case XK_3:            return mml::keyboard::Num3;
		    case XK_4:            return mml::keyboard::Num4;
		    case XK_5:            return mml::keyboard::Num5;
		    case XK_6:            return mml::keyboard::Num6;
		    case XK_7:            return mml::keyboard::Num7;
		    case XK_8:            return mml::keyboard::Num8;
		    case XK_9:            return mml::keyboard::Num9;
		}

		return mml::keyboard::Unknown;
	}
}


namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
window_impl_x11::window_impl_x11(window_handle handle) :
_window         (0),
_screen         (0),
_input_method    (NULL),
_input_context   (NULL),
_is_external     (true),
_old_video_mode   (0),
_old_rrc_rtc      (0),
_hidden_cursor   (0),
_last_cursor     (None),
_key_repeat      (true),
_previous_size   {-1, -1},
_use_size_hints   (false),
_fullscreen     (false),
_cursor_grabbed  (false),
_window_mapped   (false),
_icon_pixmap     (0),
_icon_mask_pixmap (0),
_last_input_time  (0)
{
	// Open a connection with the X server
	_display = open_display();

	// Make sure to check for EWMH support before we do anything
	ewmhSupported();

	_screen = DefaultScreen(_display);

	// Save the window handle
	_window = handle;

	if (_window)
	{
		// Make sure the window is listening to all the required events
		XSetWindowAttributes attributes;
		attributes.event_mask = eventMask;

		XChangeWindowAttributes(_display, _window, CWEventMask, &attributes);

		// Set the WM protocols
		set_protocols();

		// Do some common initializations
		initialize();
	}
}


////////////////////////////////////////////////////////////
window_impl_x11::window_impl_x11(video_mode mode, const std::string& title, unsigned long style) :
_window         (0),
_screen         (0),
_input_method    (NULL),
_input_context   (NULL),
_is_external     (false),
_old_video_mode   (0),
_old_rrc_rtc      (0),
_hidden_cursor   (0),
_last_cursor     (None),
_key_repeat      (true),
_previous_size   {-1, -1},
_use_size_hints   (false),
_fullscreen     ((style & style::fullscreen) != 0),
_cursor_grabbed  (_fullscreen),
_window_mapped   (false),
_icon_pixmap     (0),
_icon_mask_pixmap (0),
_last_input_time  (0)
{
	// Open a connection with the X server
	_display = open_display();

	// Make sure to check for EWMH support before we do anything
	ewmhSupported();

	_screen = DefaultScreen(_display);

	// Compute position and size
	std::array<std::int32_t, 2> windowPosition{{0, 0}};
	if(m_fullscreen)
	{
        windowPosition = getPrimaryMonitorPosition();
	}
	else
	{
        windowPosition[0] = (DisplayWidth(m_display, m_screen)  - mode.width) / 2;
        windowPosition[1] = (DisplayWidth(m_display, m_screen)  - mode.height) / 2;
	}

	int width  = mode.width;
	int height = mode.height;

    // Choose the visual according to the context setting
    Visual* visual = DefaultVisual(_display, _screen);

    int32_t depth  = DefaultDepth(_display, _screen);
	// Define the window attributes
	XSetWindowAttributes attributes;
    attributes.colormap = XCreateColormap(_display, DefaultRootWindow(_display), visual, AllocNone);
	attributes.event_mask = eventMask;
	attributes.override_redirect = (_fullscreen && !ewmhSupported()) ? True : False;

	_window = XCreateWindow(_display,
	                         DefaultRootWindow(_display),
	                         windowPosition[0], windowPosition[1],
	                         width, height,
	                         0,
                             depth,
	                         InputOutput,
                             visual,
	                         CWEventMask | CWOverrideRedirect | CWColormap,
	                         &attributes);

	if (!_window)
	{
		err() << "Failed to create window" << std::endl;
		return;
	}

	// Set the WM protocols
	set_protocols();

	// Set the WM initial state to the normal state
	XWMHints* hints = XAllocWMHints();
	hints->flags         = StateHint;
	hints->initial_state = NormalState;
	XSetWMHints(_display, _window, hints);
	XFree(hints);

	// If not in fullscreen, set the window's style (tell the window manager to
	// change our window's decorations and functions according to the requested style)
	if (!_fullscreen)
	{
		Atom WMHintsAtom = get_atom("_MOTIF_WM_HINTS", false);
		if (WMHintsAtom)
		{
			static const unsigned long MWM_HINTS_FUNCTIONS   = 1 << 0;
			static const unsigned long MWM_HINTS_DECORATIONS = 1 << 1;

			//static const unsigned long MWM_DECOR_ALL         = 1 << 0;
			static const unsigned long MWM_DECOR_BORDER      = 1 << 1;
			static const unsigned long MWM_DECOR_RESIZEH     = 1 << 2;
			static const unsigned long MWM_DECOR_TITLE       = 1 << 3;
			static const unsigned long MWM_DECOR_MENU        = 1 << 4;
			static const unsigned long MWM_DECOR_MINIMIZE    = 1 << 5;
			static const unsigned long MWM_DECOR_MAXIMIZE    = 1 << 6;

			//static const unsigned long MWM_FUNC_ALL          = 1 << 0;
			static const unsigned long MWM_FUNC_RESIZE       = 1 << 1;
			static const unsigned long MWM_FUNC_MOVE         = 1 << 2;
			static const unsigned long MWM_FUNC_MINIMIZE     = 1 << 3;
			static const unsigned long MWM_FUNC_MAXIMIZE     = 1 << 4;
			static const unsigned long MWM_FUNC_CLOSE        = 1 << 5;

			struct WMHints
			{
				unsigned long flags;
				unsigned long functions;
				unsigned long decorations;
				long          inputMode;
				unsigned long state;
			};

			WMHints hints;
			std::memset(&hints, 0, sizeof(hints));
			hints.flags       = MWM_HINTS_FUNCTIONS | MWM_HINTS_DECORATIONS;
			hints.decorations = 0;
			hints.functions   = 0;

			if (style & style::titlebar)
			{
				hints.decorations |= MWM_DECOR_BORDER | MWM_DECOR_TITLE | MWM_DECOR_MINIMIZE | MWM_DECOR_MENU;
				hints.functions   |= MWM_FUNC_MOVE | MWM_FUNC_MINIMIZE;
			}
			if (style & style::resize)
			{
				hints.decorations |= MWM_DECOR_MAXIMIZE | MWM_DECOR_RESIZEH;
				hints.functions   |= MWM_FUNC_MAXIMIZE | MWM_FUNC_RESIZE;
			}
			if (style & style::close)
			{
				hints.decorations |= 0;
				hints.functions   |= MWM_FUNC_CLOSE;
			}

			XChangeProperty(_display,
			                _window,
			                WMHintsAtom,
			                WMHintsAtom,
			                32,
			                PropModeReplace,
			                reinterpret_cast<const unsigned char*>(&hints),
			                5);
		}
	}

	// This is a hack to force some windows managers to disable resizing
	if (!(style & style::resize))
	{
		_use_size_hints = true;
		XSizeHints* sizeHints = XAllocSizeHints();
		sizeHints->flags = PMinSize | PMaxSize | USPosition;
		sizeHints->min_width = sizeHints->max_width = width;
		sizeHints->min_height = sizeHints->max_height = height;
		sizeHints->x = windowPosition[0];
        sizeHints->y = windowPosition[1];
		XSetWMNormalHints(_display, _window, sizeHints);
		XFree(sizeHints);
	}

	// Set the window's WM class (this can be used by window managers)
	XClassHint* hint = XAllocClassHint();

	// The instance name should be something unique to this invocation
	// of the application but is rarely if ever used these days.
	// For simplicity, we retrieve it via the base executable name.
	std::string executableName = findExecutableName();
	std::vector<char> windowInstance(executableName.size() + 1, 0);
	std::copy(executableName.begin(), executableName.end(), windowInstance.begin());
	hint->res_name = &windowInstance[0];

	// The class name identifies a class of windows that
	// "are of the same type". We simply use the initial window name as
	// the class name.
	std::string ansiTitle = title;
	std::vector<char> windowClass(ansiTitle.size() + 1, 0);
	std::copy(ansiTitle.begin(), ansiTitle.end(), windowClass.begin());
	hint->res_class = &windowClass[0];

	XSetClassHint(_display, _window, hint);

	XFree(hint);

	// Set the window's name
	set_title(title);

	// Do some common initializations
	initialize();

	// Set fullscreen video mode and switch to fullscreen if necessary
	if (_fullscreen)
	{
        // Disable hint for min and max size,
        // otherwise some windows managers will not remove window decorations
        XSizeHints *sizeHints = XAllocSizeHints();
        long flags = 0;
        XGetWMNormalHints(_display, _window, sizeHints, &flags);
        sizeHints->flags &= ~(PMinSize | PMaxSize);
        XSetWMNormalHints(_display, _window, sizeHints);
        XFree(sizeHints);

        set_video_mode(mode);
		switch_to_fullscreen();
	}
}


////////////////////////////////////////////////////////////
window_impl_x11::~window_impl_x11()
{
	// Cleanup graphical resources
	cleanup();

	// Destroy icon pixmap
	if(_icon_pixmap)
		XFreePixmap(_display, _icon_pixmap);

	// Destroy icon mask pixmap
	if(_icon_mask_pixmap)
		XFreePixmap(_display, _icon_mask_pixmap);

	// Destroy the cursor
	if (_hidden_cursor)
		XFreeCursor(_display, _hidden_cursor);

	// Destroy the input context
	if (_input_context)
		XDestroyIC(_input_context);

	// Destroy the window
	if (_window && !_is_external)
	{
		XUnmapWindow(_display, _window);
		XDestroyWindow(_display, _window);
		XFlush(_display);
	}

	// Close the input method
	if (_input_method)
		XCloseIM(_input_method);

	// Close the connection with the X server
	close_display(_display);

	// Remove this window from the global list of windows (required for focus request)
	std::lock_guard<std::mutex> lock(allWindowsMutex);
	allWindows.erase(std::find(allWindows.begin(), allWindows.end(), this));
}


////////////////////////////////////////////////////////////
window_handle window_impl_x11::native_handle() const
{
	return _window;
}


////////////////////////////////////////////////////////////
void window_impl_x11::process_events()
{
	XEvent event;
	while (XCheckIfEvent(_display, &event, &checkEvent, reinterpret_cast<XPointer>(_window)))
	{
        _events.push_back(event);
	}

	while(!_events.empty())
	{
        event = _events.front();
        _events.pop_front();
        process_event(event);
	}

	// Process clipboard window events
    priv::clipboard_impl::process_events();
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> window_impl_x11::get_position() const
{
	// Get absolute position of our window relative to root window. This
    // takes into account all information that X11 has, including X11
    // border widths and any decorations. It corresponds to where the
    // window actually is, but not necessarily to where we told it to
    // go using setPosition() and XMoveWindow(). To have the two match
    // as expected, we may have to subtract decorations and borders.
    ::Window child;
    int xAbsRelToRoot, yAbsRelToRoot;

    XTranslateCoordinates(_display, _window, DefaultRootWindow(_display),
        0, 0, &xAbsRelToRoot, &yAbsRelToRoot, &child);

    // CASE 1: some rare WMs actually put the window exactly where we tell
    // it to, even with decorations and such, which get shifted back.
    // In these rare cases, we can use the absolute value directly.
    if (isWMAbsolutePositionGood())
        return std::array<std::int32_t, 2>{{xAbsRelToRoot, yAbsRelToRoot}};

    // CASE 2: most modern WMs support EWMH and can define _NET_FRAME_EXTENTS
    // with the exact frame size to subtract, so if present, we prefer it and
    // query it first. According to spec, this already includes any borders.
    long xFrameExtent, yFrameExtent;

    if (getEWMHFrameExtents(_display, _window, xFrameExtent, yFrameExtent))
    {
        // Get final X/Y coordinates: subtract EWMH frame extents from
        // absolute window position.
        return std::array<std::int32_t, 2>{{(xAbsRelToRoot - xFrameExtent), (yAbsRelToRoot - yFrameExtent)}};
    }

    // CASE 3: EWMH frame extents were not available, use geometry.
    // We climb back up to the window before the root and use its
    // geometry information to extract X/Y position. This because
    // re-parenting WMs may re-parent the window multiple times, so
    // we'd have to climb up to the furthest ancestor and sum the
    // relative differences and borders anyway; and doing that to
    // subtract those values from the absolute coordinates of the
    // window is equivalent to going up the tree and asking the
    // furthest ancestor what it's relative distance to the root is.
    // So we use that approach because it's simpler.
    // This approach assumes that any window between the root and
    // our window is part of decorations/borders in some way. This
    // seems to hold true for most reasonable WM implementations.
    ::Window ancestor = _window;
    ::Window root = DefaultRootWindow(_display);

    while (getParentWindow(_display, ancestor) != root)
    {
        // Next window up (parent window).
        ancestor = getParentWindow(_display, ancestor);
    }

    // Get final X/Y coordinates: take the relative position to
    // the root of the furthest ancestor window.
    int xRelToRoot, yRelToRoot;
    unsigned int width, height, borderWidth, depth;

    XGetGeometry(_display, ancestor, &root, &xRelToRoot, &yRelToRoot,
        &width, &height, &borderWidth, &depth);

    return std::array<std::int32_t, 2>{{xRelToRoot, yRelToRoot}};
}


////////////////////////////////////////////////////////////
void window_impl_x11::set_position(const std::array<std::int32_t, 2>& position)
{
	XMoveWindow(_display, _window, position[0], position[1]);
	XFlush(_display);
}


////////////////////////////////////////////////////////////
std::array<std::uint32_t, 2> window_impl_x11::get_size() const
{
	XWindowAttributes attributes;
	XGetWindowAttributes(_display, _window, &attributes);
	return std::array<std::uint32_t, 2>{{static_cast<std::uint32_t>(attributes.width), static_cast<std::uint32_t>(attributes.height)}};
}


////////////////////////////////////////////////////////////
void window_impl_x11::set_size(const std::array<std::uint32_t, 2>& size)
{
	// If resizing is disable for the window we have to update the size hints (required by some window managers).
	if (_use_size_hints)
	{
		XSizeHints* sizeHints = XAllocSizeHints();
		sizeHints->flags = PMinSize | PMaxSize;
		sizeHints->min_width = sizeHints->max_width = size[0];
		sizeHints->min_height = sizeHints->max_height = size[1];
		XSetWMNormalHints(_display, _window, sizeHints);
		XFree(sizeHints);
	}

	XResizeWindow(_display, _window, size[0], size[1]);
	XFlush(_display);
}


////////////////////////////////////////////////////////////
void window_impl_x11::set_title(const std::string& title)
{
	// Bare X11 has no Unicode window title support.
	// There is however an option to tell the window manager your Unicode title via hints.

	// Convert to UTF-8 encoding.
	std::basic_string<std::uint8_t> utf8Title;
	utf32::to_utf8(title.begin(), title.end(), std::back_inserter(utf8Title));

	Atom useUtf8 = get_atom("UTF8_STRING", false);

	// Set the _NET_WM_NAME atom, which specifies a UTF-8 encoded window title.
	Atom wmName = get_atom("_NET_WM_NAME", false);
	XChangeProperty(_display, _window, wmName, useUtf8, 8,
	                PropModeReplace, utf8Title.c_str(), utf8Title.size());

	// Set the _NET_WM_ICON_NAME atom, which specifies a UTF-8 encoded window title.
	Atom wmIconName = get_atom("_NET_WM_ICON_NAME", false);
	XChangeProperty(_display, _window, wmIconName, useUtf8, 8,
	                PropModeReplace, utf8Title.c_str(), utf8Title.size());

	// Set the non-Unicode title as a fallback for window managers who don't support _NET_WM_NAME.
    #ifdef X_HAVE_UTF8_STRING
	Xutf8SetWMProperties(_display,
	                     _window,
	                     title.c_str(),
	                     title.c_str(),
	                     NULL,
	                     0,
	                     NULL,
	                     NULL,
	                     NULL);
    #else
	XmbSetWMProperties(_display,
	                   _window,
	                   title.c_str(),
	                   title.c_str(),
	                   NULL,
	                   0,
	                   NULL,
	                   NULL,
	                   NULL);
    #endif
}


////////////////////////////////////////////////////////////
void window_impl_x11::set_icon(unsigned int width, unsigned int height, const std::uint8_t* pixels)
{
	// X11 wants BGRA pixels: swap red and blue channels
	// Note: this memory will be freed by XDestroyImage
	std::uint8_t* iconPixels = static_cast<std::uint8_t*>(std::malloc(width * height * 4));
	for (std::size_t i = 0; i < width * height; ++i)
	{
		iconPixels[i * 4 + 0] = pixels[i * 4 + 2];
		iconPixels[i * 4 + 1] = pixels[i * 4 + 1];
		iconPixels[i * 4 + 2] = pixels[i * 4 + 0];
		iconPixels[i * 4 + 3] = pixels[i * 4 + 3];
	}

	// Create the icon pixmap
	Visual*      defVisual = DefaultVisual(_display, _screen);
	unsigned int defDepth  = DefaultDepth(_display, _screen);
	XImage* iconImage = XCreateImage(_display, defVisual, defDepth, ZPixmap, 0, (char*)iconPixels, width, height, 32, 0);
	if (!iconImage)
	{
		err() << "Failed to set the window's icon" << std::endl;
		return;
	}

	if(_icon_pixmap)
		XFreePixmap(_display, _icon_pixmap);

	if(_icon_mask_pixmap)
		XFreePixmap(_display, _icon_mask_pixmap);

	_icon_pixmap = XCreatePixmap(_display, RootWindow(_display, _screen), width, height, defDepth);
	XGCValues values;
	GC iconGC = XCreateGC(_display, _icon_pixmap, 0, &values);
	XPutImage(_display, _icon_pixmap, iconGC, iconImage, 0, 0, 0, 0, width, height);
	XFreeGC(_display, iconGC);
	XDestroyImage(iconImage);

	// Create the mask pixmap (must have 1 bit depth)
	std::size_t pitch = (width + 7) / 8;
	std::vector<std::uint8_t> maskPixels(pitch * height, 0);
	for (std::size_t j = 0; j < height; ++j)
	{
		for (std::size_t i = 0; i < pitch; ++i)
		{
			for (std::size_t k = 0; k < 8; ++k)
			{
				if (i * 8 + k < width)
				{
					std::uint8_t opacity = (pixels[(i * 8 + k + j * width) * 4 + 3] > 0) ? 1 : 0;
					maskPixels[i + j * pitch] |= (opacity << k);
				}
			}
		}
	}
	_icon_mask_pixmap = XCreatePixmapFromBitmapData(_display, _window, (char*)&maskPixels[0], width, height, 1, 0, 1);

	// Send our new icon to the window through the WMHints
	XWMHints* hints = XAllocWMHints();
	hints->flags       = IconPixmapHint | IconMaskHint;
	hints->icon_pixmap = _icon_pixmap;
	hints->icon_mask   = _icon_mask_pixmap;
	XSetWMHints(_display, _window, hints);
	XFree(hints);

	// ICCCM wants BGRA pixels: swap red and blue channels
	// ICCCM also wants the first 2 unsigned 32-bit values to be width and height
	std::vector<unsigned long> icccmIconPixels(2 + width * height, 0);
	unsigned long* ptr = &icccmIconPixels[0];

	*ptr++ = width;
	*ptr++ = height;

	for (std::size_t i = 0; i < width * height; ++i)
	{
		*ptr++ = (pixels[i * 4 + 2] << 0 ) |
		         (pixels[i * 4 + 1] << 8 ) |
		         (pixels[i * 4 + 0] << 16) |
		         (pixels[i * 4 + 3] << 24);
	}

	Atom netWmIcon = get_atom("_NET_WM_ICON");

	XChangeProperty(_display,
	                _window,
	                netWmIcon,
	                XA_CARDINAL,
	                32,
	                PropModeReplace,
	                reinterpret_cast<const unsigned char*>(&icccmIconPixels[0]),
	                2 + width * height);

	XFlush(_display);
}


////////////////////////////////////////////////////////////
void window_impl_x11::set_visible(bool visible)
{
	if (visible)
	{
		XMapWindow(_display, _window);

		XFlush(_display);

		// Before continuing, make sure the WM has
		// internally marked the window as viewable
		while (!_window_mapped && !_is_external)
			process_events();
	}
	else
	{
		XUnmapWindow(_display, _window);

		XFlush(_display);

		// Before continuing, make sure the WM has
		// internally marked the window as unviewable
		while (_window_mapped && !_is_external)
			process_events();
	}
}


////////////////////////////////////////////////////////////
void window_impl_x11::maximize()
{    

    Atom _NET_WM_STATE = get_atom("_NET_WM_STATE", false);

    Atom _NET_WM_STATE_MAXIMIZED_VERT = get_atom("_NET_WM_STATE_MAXIMIZED_VERT", false);

    Atom _NET_WM_STATE_MAXIMIZED_HORZ = get_atom("_NET_WM_STATE_MAXIMIZED_HORZ", false);

    if (_window_mapped)
    {

        XEvent e;
        std::memset(&e, 0, sizeof(e));

        e.xany.type = ClientMessage;
        e.xclient.message_type = _NET_WM_STATE;
        e.xclient.format = 32;
        e.xclient.window = _window;
        e.xclient.data.l[0] = _NET_WM_STATE_ADD;
        e.xclient.data.l[1] = _NET_WM_STATE_MAXIMIZED_VERT;
        e.xclient.data.l[2] = _NET_WM_STATE_MAXIMIZED_HORZ;
        e.xclient.data.l[3] = 0l;

        XSendEvent(_display, RootWindow(_display, _screen), 0,
                       SubstructureNotifyMask | SubstructureRedirectMask, &e);

    }
    else
    {
        //X11_SetNetWMState(_this, data->xwindow, window->flags);
    }

    XFlush(_display);
}

////////////////////////////////////////////////////////////
void window_impl_x11::minimize()
{
	XIconifyWindow(_display, _window, _screen);

    XFlush(_display);
}
static Bool is_map_notify(Display *dpy, XEvent *ev, XPointer win)
{
    return ev->type == MapNotify && ev->xmap.window == *((Window*)win);

}
////////////////////////////////////////////////////////////
void window_impl_x11::restore()
{
    {
        Atom _NET_WM_STATE = get_atom("_NET_WM_STATE", false);

        Atom _NET_WM_STATE_MAXIMIZED_VERT = get_atom("_NET_WM_STATE_MAXIMIZED_VERT", false);

        Atom _NET_WM_STATE_MAXIMIZED_HORZ = get_atom("_NET_WM_STATE_MAXIMIZED_HORZ", false);

        if (_window_mapped)
        {

            XEvent e;
            std::memset(&e, 0, sizeof(e));

            e.xany.type = ClientMessage;
            e.xclient.message_type = _NET_WM_STATE;
            e.xclient.format = 32;
            e.xclient.window = _window;
            e.xclient.data.l[0] = _NET_WM_STATE_REMOVE;
            e.xclient.data.l[1] = _NET_WM_STATE_MAXIMIZED_VERT;
            e.xclient.data.l[2] = _NET_WM_STATE_MAXIMIZED_HORZ;
            e.xclient.data.l[3] = 0l;

            XSendEvent(_display, RootWindow(_display, _screen), 0,
                           SubstructureNotifyMask | SubstructureRedirectMask, &e);

        }
        else
        {
            //X11_SetNetWMState(_this, data->xwindow, window->flags);
        }

        XFlush(_display);
    }



    {
        XEvent event;
        if (!_window_mapped)
        {
            XMapRaised(_display,_window);
            /* Blocking wait for "MapNotify" event.
            * We use X11_XIfEvent because pXWindowEvent takes a mask rather than a type,
            * and XCheckTypedWindowEvent doesn't block */

            XIfEvent(_display, &event, &is_map_notify, (XPointer)&_window);

            XFlush(_display);

        }
    }

    {
        Atom _NET_ACTIVE_WINDOW = get_atom("_NET_ACTIVE_WINDOW", false);

        if (_window_mapped)
        {
            XEvent e;
            std::memset(&e, 0, sizeof(e));

            e.xany.type = ClientMessage;
            e.xclient.message_type = _NET_ACTIVE_WINDOW;
            e.xclient.format = 32;
            e.xclient.window = _window;
            e.xclient.data.l[0] = 1;  /* source indication. 1 = application */
            e.xclient.data.l[1] = _last_input_time;
            e.xclient.data.l[2] = 0;

            XSendEvent(_display, RootWindow(_display, _screen), 0,
                       SubstructureNotifyMask | SubstructureRedirectMask, &e);


            XFlush(_display);
        }

    }
}

////////////////////////////////////////////////////////////
void window_impl_x11::set_opacity(float opacity)
{
	const std::uint32_t fully_opaque = 0xFFFFFFFF;	
	const long alpha = (long) ((double)opacity * (double)fully_opaque);
	
	Atom property = get_atom("_NET_WM_WINDOW_OPACITY", false);
	if (property != None)
	{
		XChangeProperty(_display, _window, property, XA_CARDINAL, 32, PropModeReplace, (unsigned char*)&alpha, 1);
		XFlush(_display);
	}
}

////////////////////////////////////////////////////////////
void window_impl_x11::set_mouse_cursor_visible(bool visible)
{
	XDefineCursor(_display, _window, visible ? _last_cursor : _hidden_cursor);
	XFlush(_display);
}


////////////////////////////////////////////////////////////
void window_impl_x11::set_mouse_cursor(const cursor_impl& cursor)
{
	_last_cursor = cursor._cursor;
	XDefineCursor(_display, _window, _last_cursor);
	XFlush(_display);
}


////////////////////////////////////////////////////////////
void window_impl_x11::set_mouse_cursor_grabbed(bool grabbed)
{
	// This has no effect in fullscreen mode
	if (_fullscreen || (_cursor_grabbed == grabbed))
		return;

	if (grabbed)
	{
		// Try multiple times to grab the cursor
		for (unsigned int trial = 0; trial < maxTrialsCount; ++trial)
		{
			int result = XGrabPointer(_display, _window, True, None, GrabModeAsync, GrabModeAsync, _window, None, CurrentTime);

			if (result == GrabSuccess)
			{
				_cursor_grabbed = true;
				break;
			}

			// The cursor grab failed, trying again after a small sleep
			std::this_thread::sleep_for(std::chrono::milliseconds(50));
		}

		if (!_cursor_grabbed)
			err() << "Failed to grab mouse cursor" << std::endl;
	}
	else
	{
		XUngrabPointer(_display, CurrentTime);
	}
}


////////////////////////////////////////////////////////////
void window_impl_x11::set_key_repeat_enabled(bool enabled)
{
	_key_repeat = enabled;
}


////////////////////////////////////////////////////////////
void window_impl_x11::request_focus()
{
	// Focus is only stolen among mml windows, not between applications
	// Check the global list of windows to find out whether an mml window has the focus
	// Note: can't handle console and other non-mml windows belonging to the application.
	bool sfmlWindowFocused = false;

	{
		std::lock_guard<std::mutex> lock(allWindowsMutex);
		for (std::vector<window_impl_x11*>::iterator itr = allWindows.begin(); itr != allWindows.end(); ++itr)
		{
			if ((*itr)->has_focus())
			{
				sfmlWindowFocused = true;
				break;
			}
		}
	}

	// Check if window is viewable (not on other desktop, ...)
	// TODO: Check also if minimized
	XWindowAttributes attributes;
	if (XGetWindowAttributes(_display, _window, &attributes) == 0)
	{
		mml::err() << "Failed to check if window is viewable while requesting focus" << std::endl;
		return; // error getting attribute
	}

	bool windowViewable = (attributes.map_state == IsViewable);

	if (sfmlWindowFocused && windowViewable)
	{
		// Another mml window of this application has the focus and the current window is viewable:
		// steal focus (i.e. bring window to the front and give it input focus)
		grab_focus();
	}
	else
	{
		// Otherwise: display urgency hint (flashing application logo)
		// Ensure WM hints exist, allocate if necessary
		XWMHints* hints = XGetWMHints(_display, _window);
		if (hints == NULL)
			hints = XAllocWMHints();

		// Add urgency (notification) flag to hints
		hints->flags |= XUrgencyHint;
		XSetWMHints(_display, _window, hints);
		XFree(hints);
	}
}


////////////////////////////////////////////////////////////
bool window_impl_x11::has_focus() const
{
	::Window focusedWindow = 0;
	int revertToReturn = 0;
    XGetInputFocus(_display, &focusedWindow, &revertToReturn);

	return (_window == focusedWindow);
}


////////////////////////////////////////////////////////////
void window_impl_x11::grab_focus()
{
	Atom netActiveWindow = None;

	if (ewmhSupported())
		netActiveWindow = get_atom("_NET_ACTIVE_WINDOW");

	// Only try to grab focus if the window is mapped
	XWindowAttributes attr;

	XGetWindowAttributes(_display, _window, &attr);

	if (attr.map_state == IsUnmapped)
		return;

	if (netActiveWindow)
	{
		XEvent event;
		std::memset(&event, 0, sizeof(event));

		event.type = ClientMessage;
		event.xclient.window = _window;
		event.xclient.format = 32;
		event.xclient.message_type = netActiveWindow;
		event.xclient.data.l[0] = 1; // Normal application
		event.xclient.data.l[1] = _last_input_time;
		event.xclient.data.l[2] = 0; // We don't know the currently active window

		int result = XSendEvent(_display,
		                        DefaultRootWindow(_display),
		                        False,
		                        SubstructureNotifyMask | SubstructureRedirectMask,
		                        &event);

		XFlush(_display);

		if (!result)
			err() << "Setting fullscreen failed, could not send \"_NET_ACTIVE_WINDOW\" event" << std::endl;
	}
	else
	{
		XRaiseWindow(_display, _window);
		XSetInputFocus(_display, _window, RevertToPointerRoot, CurrentTime);
		XFlush(_display);
	}
}


////////////////////////////////////////////////////////////
void window_impl_x11::set_video_mode(const video_mode& mode)
{
	// Skip mode switching if the new mode is equal to the desktop mode
	if (mode == video_mode::get_desktop_mode())
		return;

	// Check if the XRandR extension is present
    int xRandRMajor, xRandRMinor;
    if (!checkXRandR(xRandRMajor, xRandRMinor))
    {
        // XRandR extension is not supported: we cannot use fullscreen mode
        err() << "Fullscreen is not supported, switching to window mode" << std::endl;
        return;
    }

    // Get root window
    ::Window rootWindow = RootWindow(_display, _screen);

    // Get the screen resources
    XRRScreenResources* res = XRRGetScreenResources(_display, rootWindow);
    if (!res)
    {
        err() << "Failed to get the current screen resources for fullscreen mode, switching to window mode" << std::endl;
        return;
    }

    RROutput output = getOutputPrimary(rootWindow, res, xRandRMajor, xRandRMinor);

    // Get output info from output
    XRROutputInfo* outputInfo = XRRGetOutputInfo(_display, res, output);
    if (!outputInfo || outputInfo->connection == RR_Disconnected)
    {
        XRRFreeScreenResources(res);

        // If outputInfo->connection == RR_Disconnected, free output info
        if (outputInfo)
            XRRFreeOutputInfo(outputInfo);

        err() << "Failed to get output info for fullscreen mode, switching to window mode" << std::endl;
        return;
    }

    // Retreive current RRMode, screen position and rotation
    XRRCrtcInfo* crtcInfo = XRRGetCrtcInfo(_display, res, outputInfo->crtc);
    if (!crtcInfo)
    {
        XRRFreeScreenResources(res);
        XRRFreeOutputInfo(outputInfo);
        err() << "Failed to get crtc info for fullscreen mode, switching to window mode" << std::endl;
        return;
    }

    // Find RRMode to set
    bool modeFound = false;
    RRMode xRandMode;

    for (int i = 0; (i < res->nmode) && !modeFound; i++)
    {
        if (crtcInfo->rotation == RR_Rotate_90 || crtcInfo->rotation == RR_Rotate_270)
            std::swap(res->modes[i].height, res->modes[i].width);

        // Check if screen size match
        if (res->modes[i].width == static_cast<int>(mode.width) &&
            res->modes[i].height == static_cast<int>(mode.height))
        {
            xRandMode = res->modes[i].id;
            modeFound = true;
        }
    }

    if (!modeFound)
    {
        XRRFreeScreenResources(res);
        XRRFreeOutputInfo(outputInfo);
        err() << "Failed to find a matching RRMode for fullscreen mode, switching to window mode" << std::endl;
        return;
    }

    // Save the current video mode before we switch to fullscreen
    _old_video_mode = crtcInfo->mode;
    _old_rrc_rtc = outputInfo->crtc;

    // Switch to fullscreen mode
    XRRSetCrtcConfig(_display,
                     res,
                     outputInfo->crtc,
                     CurrentTime,
                     crtcInfo->x,
                     crtcInfo->y,
                     xRandMode,
                     crtcInfo->rotation,
                     &output,
                     1);

    // Set "this" as the current fullscreen window
    fullscreenWindow = this;

    XRRFreeScreenResources(res);
    XRRFreeOutputInfo(outputInfo);
    XRRFreeCrtcInfo(crtcInfo);
}


////////////////////////////////////////////////////////////
void window_impl_x11::reset_video_mode()
{
	if (fullscreenWindow == this)
    {
        // Try to set old configuration
        // Check if the XRandR extension
        int xRandRMajor, xRandRMinor;
        if (checkXRandR(xRandRMajor, xRandRMinor))
        {
            XRRScreenResources* res = XRRGetScreenResources(_display, DefaultRootWindow(_display));
            if (!res)
            {
                err() << "Failed to get the current screen resources to reset the video mode" << std::endl;
                return;
            }

            // Retreive current screen position and rotation
            XRRCrtcInfo* crtcInfo = XRRGetCrtcInfo(_display, res, _old_rrc_rtc);
            if (!crtcInfo)
            {
                XRRFreeScreenResources(res);
                err() << "Failed to get crtc info to reset the video mode" << std::endl;
                return;
            }

            RROutput output;

            // if version >= 1.3 get the primary screen else take the first screen
            if ((xRandRMajor == 1 && xRandRMinor >= 3) || xRandRMajor > 1)
            {
                output = XRRGetOutputPrimary(_display, DefaultRootWindow(_display));

                // Check if returned output is valid, otherwise use the first screen
                if (output == None)
                    output = res->outputs[0];
            }
            else{
                output = res->outputs[0];
            }

            XRRSetCrtcConfig(_display,
                             res,
                             _old_rrc_rtc,
                             CurrentTime,
                             crtcInfo->x,
                             crtcInfo->y,
                             _old_video_mode,
                             crtcInfo->rotation,
                             &output,
                             1);

            XRRFreeCrtcInfo(crtcInfo);
            XRRFreeScreenResources(res);
        }

        // Reset the fullscreen window
        fullscreenWindow = NULL;
    }
}


////////////////////////////////////////////////////////////
void window_impl_x11::switch_to_fullscreen()
{
	grab_focus();

	if (ewmhSupported())
	{
		Atom netWmBypassCompositor = get_atom("_NET_WM_BYPASS_COMPOSITOR");

		if (netWmBypassCompositor)
		{
			static const unsigned long bypassCompositor = 1;

			XChangeProperty(_display,
			                _window,
			                netWmBypassCompositor,
			                XA_CARDINAL,
			                32,
			                PropModeReplace,
			                reinterpret_cast<const unsigned char*>(&bypassCompositor),
			                1);
		}

		Atom netWmState = get_atom("_NET_WM_STATE", true);
		Atom netWmStateFullscreen = get_atom("_NET_WM_STATE_FULLSCREEN", true);

		if (!netWmState || !netWmStateFullscreen)
		{
			err() << "Setting fullscreen failed. Could not get required atoms" << std::endl;
			return;
		}

		XEvent event;
		std::memset(&event, 0, sizeof(event));

		event.type = ClientMessage;
		event.xclient.window = _window;
		event.xclient.format = 32;
		event.xclient.message_type = netWmState;
		event.xclient.data.l[0] = 1; // _NET_WM_STATE_ADD
		event.xclient.data.l[1] = netWmStateFullscreen;
		event.xclient.data.l[2] = 0; // No second property
		event.xclient.data.l[3] = 1; // Normal window

		int result = XSendEvent(_display,
		                        DefaultRootWindow(_display),
		                        False,
		                        SubstructureNotifyMask | SubstructureRedirectMask,
		                        &event);

		if (!result)
			err() << "Setting fullscreen failed, could not send \"_NET_WM_STATE\" event" << std::endl;
	}
}


////////////////////////////////////////////////////////////
void window_impl_x11::set_protocols()
{
	Atom wmProtocols = get_atom("WM_PROTOCOLS");
	Atom wmDeleteWindow = get_atom("WM_DELETE_WINDOW");

	if (!wmProtocols)
	{
		err() << "Failed to request WM_PROTOCOLS atom." << std::endl;
		return;
	}

	std::vector<Atom> atoms;

	if (wmDeleteWindow)
	{
		atoms.push_back(wmDeleteWindow);
	}
	else
	{
		err() << "Failed to request WM_DELETE_WINDOW atom." << std::endl;
	}

	Atom netWmPing = None;
	Atom netWmPid = None;

	if (ewmhSupported())
	{
		netWmPing = get_atom("_NET_WM_PING", true);
		netWmPid = get_atom("_NET_WM_PID", true);
	}

	if (netWmPing && netWmPid)
	{
		const long pid = getpid();

		XChangeProperty(_display,
		                _window,
		                netWmPid,
		                XA_CARDINAL,
		                32,
		                PropModeReplace,
		                reinterpret_cast<const unsigned char*>(&pid),
		                1);

		atoms.push_back(netWmPing);
	}

	if (!atoms.empty())
	{
		XChangeProperty(_display,
		                _window,
		                wmProtocols,
		                XA_ATOM,
		                32,
		                PropModeReplace,
		                reinterpret_cast<const unsigned char*>(&atoms[0]),
		                atoms.size());
	}
	else
	{
		err() << "Didn't set any window protocols" << std::endl;
	}
}


////////////////////////////////////////////////////////////
void window_impl_x11::initialize()
{
	// Create the input context
	_input_method = XOpenIM(_display, NULL, NULL, NULL);

	if (_input_method)
	{
		_input_context = XCreateIC(_input_method,
		                           XNClientWindow,
		                           _window,
		                           XNFocusWindow,
		                           _window,
		                           XNInputStyle,
		                           XIMPreeditNothing | XIMStatusNothing,
		                           NULL);
	}
	else
	{
		_input_context = NULL;
	}

	if (!_input_context)
		err() << "Failed to create input context for window -- text_entered event won't be able to return unicode" << std::endl;

	Atom wmWindowType = get_atom("_NET_WM_WINDOW_TYPE", false);
	Atom wmWindowTypeNormal = get_atom("_NET_WM_WINDOW_TYPE_NORMAL", false);

	if (wmWindowType && wmWindowTypeNormal)
	{
		XChangeProperty(_display,
		                _window,
		                wmWindowType,
		                XA_ATOM,
		                32,
		                PropModeReplace,
		                reinterpret_cast<const unsigned char*>(&wmWindowTypeNormal),
		                1);
	}

	// Show the window
	set_visible(true);

	// Raise the window and grab input focus
	grab_focus();

	// Create the hidden cursor
	create_hidden_cursor();

	// Flush the commands queue
	XFlush(_display);

	// Add this window to the global list of windows (required for focus request)
	std::lock_guard<std::mutex> lock(allWindowsMutex);
	allWindows.push_back(this);
}


////////////////////////////////////////////////////////////
void window_impl_x11::update_last_input_time(::Time time)
{
	if (time && (time != _last_input_time))
	{
		Atom netWmUserTime = get_atom("_NET_WM_USER_TIME", true);

		if(netWmUserTime)
		{
			XChangeProperty(_display,
			                _window,
			                netWmUserTime,
			                XA_CARDINAL,
			                32,
			                PropModeReplace,
			                reinterpret_cast<const unsigned char*>(&time),
			                1);
		}

		_last_input_time = time;
	}
}


////////////////////////////////////////////////////////////
void window_impl_x11::create_hidden_cursor()
{
	// Create the cursor's pixmap (1x1 pixels)
	Pixmap cursorPixmap = XCreatePixmap(_display, _window, 1, 1, 1);
	GC graphicsContext = XCreateGC(_display, cursorPixmap, 0, NULL);
	XDrawPoint(_display, cursorPixmap, graphicsContext, 0, 0);
	XFreeGC(_display, graphicsContext);

	// Create the cursor, using the pixmap as both the shape and the mask of the cursor
	XColor color;
	color.flags = DoRed | DoGreen | DoBlue;
	color.red = color.blue = color.green = 0;
	_hidden_cursor = XCreatePixmapCursor(_display, cursorPixmap, cursorPixmap, &color, &color, 0, 0);

	// We don't need the pixmap any longer, free it
	XFreePixmap(_display, cursorPixmap);
}


////////////////////////////////////////////////////////////
void window_impl_x11::cleanup()
{
	// Restore the previous video mode (in case we were running in fullscreen)
	reset_video_mode();

	// Unhide the mouse cursor (in case it was hidden)
	set_mouse_cursor_visible(true);
}


////////////////////////////////////////////////////////////
bool window_impl_x11::process_event(XEvent& windowEvent)
{
	// This function implements a workaround to properly discard
	// repeated key events when necessary. The problem is that the
	// system's key events policy doesn't match mml's one: X server will generate
	// both repeated KeyPress and KeyRelease events when maintaining a key down, while
	// mml only wants repeated KeyPress events. Thus, we have to:
	// - Discard duplicated KeyRelease events when KeyRepeatEnabled is true
	// - Discard both duplicated KeyPress and KeyRelease events when KeyRepeatEnabled is false

	// Detect repeated key events
	// Detect repeated key events
    if (windowEvent.type == KeyRelease)
    {
        // Find the next KeyPress event with matching keycode and time
        std::deque<XEvent>::iterator iter = std::find_if(
            _events.begin(),
            _events.end(),
            KeyRepeatFinder(windowEvent.xkey.keycode, windowEvent.xkey.time)
        );

        if (iter != _events.end())
        {
            // If we don't want repeated events, remove the next KeyPress from the queue
            if (!m_keyRepeat)
                _events.erase(iter);

            // This KeyRelease is a repeated event and we don't want it
            return false;
        }
    }

	// Convert the X11 event to a mml::platform_event
	switch (windowEvent.type)
	{
	    // Destroy event
	    case DestroyNotify:
	    {
		    // The window is about to be destroyed: we must cleanup resources
		    cleanup();
			break;
	    }

		// Gain focus event
	    case FocusIn:
	    {
		    // Update the input context
		    if (_input_context)
				XSetICFocus(_input_context);

			// Grab cursor
			if (_cursor_grabbed)
			{
				// Try multiple times to grab the cursor
				for (unsigned int trial = 0; trial < maxTrialsCount; ++trial)
				{
					int result = XGrabPointer(_display, _window, True, None, GrabModeAsync, GrabModeAsync, _window, None, CurrentTime);

					if (result == GrabSuccess)
					{
						_cursor_grabbed = true;
						break;
					}

					// The cursor grab failed, trying again after a small sleep
					std::this_thread::sleep_for(std::chrono::milliseconds(50));
				}

				if (!_cursor_grabbed)
					err() << "Failed to grab mouse cursor" << std::endl;
			}

			platform_event event;
			event.type = platform_event::gained_focus;
			push_event(event);

			// If the window has been previously marked urgent (notification) as a result of a focus request, undo that
			XWMHints* hints = XGetWMHints(_display, _window);
			if (hints != NULL)
			{
				// Remove urgency (notification) flag from hints
				hints->flags &= ~XUrgencyHint;
				XSetWMHints(_display, _window, hints);
				XFree(hints);
			}

			break;
	    }

		// Lost focus event
	    case FocusOut:
	    {
		    // Update the input context
		    if (_input_context)
				XUnsetICFocus(_input_context);

			// Release cursor
			if (_cursor_grabbed)
				XUngrabPointer(_display, CurrentTime);

			platform_event event;
			event.type = platform_event::lost_focus;
			push_event(event);
			break;
	    }

		// Resize event
	    case ConfigureNotify:
	    {
		    // ConfigureNotify can be triggered for other reasons, check if the size has actually changed
		    if ((windowEvent.xconfigure.width != _previous_size[0]) || (windowEvent.xconfigure.height != _previous_size[1]))
			{
				platform_event event;
				event.type        = platform_event::resized;
				event.size.width  = windowEvent.xconfigure.width;
				event.size.height = windowEvent.xconfigure.height;
				push_event(event);

				_previous_size[0] = windowEvent.xconfigure.width;
				_previous_size[1] = windowEvent.xconfigure.height;
			}
			break;
	    }

		// Close event
	    case ClientMessage:
	    {
		    static Atom wmProtocols = get_atom("WM_PROTOCOLS");

			// Handle window manager protocol messages we support
			if (windowEvent.xclient.message_type == wmProtocols)
			{
				static Atom wmDeleteWindow = get_atom("WM_DELETE_WINDOW");
				static Atom netWmPing = ewmhSupported() ? get_atom("_NET_WM_PING", true) : None;

				if ((windowEvent.xclient.format == 32) && (windowEvent.xclient.data.l[0]) == static_cast<long>(wmDeleteWindow))
				{
					// Handle the WM_DELETE_WINDOW message
					platform_event event;
					event.type = platform_event::closed;
					push_event(event);
				}
				else if (netWmPing && (windowEvent.xclient.format == 32) && (windowEvent.xclient.data.l[0]) == static_cast<long>(netWmPing))
				{
					// Handle the _NET_WM_PING message, send pong back to WM to show that we are responsive
					windowEvent.xclient.window = DefaultRootWindow(_display);

					XSendEvent(_display, DefaultRootWindow(_display), False, SubstructureNotifyMask | SubstructureRedirectMask, &windowEvent);
				}
			}
			break;
	    }

		// key down event
	    case KeyPress:
	    {
		    keyboard::key key = keyboard::Unknown;

			// Try each KeySym index (modifier group) until we get a match
			for (int i = 0; i < 4; ++i)
			{
				// Get the mml keyboard code from the keysym of the key that has been pressed
				key = keysym_to_mml(XLookupKeysym(&windowEvent.xkey, i));

				if (key != keyboard::Unknown)
					break;
			}

			// Fill the event parameters
			// TODO: if modifiers are wrong, use XGetModifierMapping to retrieve the actual modifiers mapping
			platform_event event;
			event.type        = platform_event::key_pressed;
			event.key.code    = key;
			event.key.alt     = windowEvent.xkey.state & Mod1Mask;
			event.key.control = windowEvent.xkey.state & ControlMask;
			event.key.shift   = windowEvent.xkey.state & ShiftMask;
			event.key.system  = windowEvent.xkey.state & Mod4Mask;
			push_event(event);

			// Generate a text_entered event
			if (!XFilterEvent(&windowEvent, None))
			{
                #ifdef X_HAVE_UTF8_STRING
				if (_input_context)
				{
					Status status;
					std::uint8_t  keyBuffer[16];

					int length = Xutf8LookupString(
					    _input_context,
					    &windowEvent.xkey,
					    reinterpret_cast<char*>(keyBuffer),
					    sizeof(keyBuffer),
					    NULL,
					    &status
					);

					if (length > 0)
					{
						std::uint32_t unicode = 0;
						utf8::decode(keyBuffer, keyBuffer + length, unicode, 0);
						if (unicode != 0)
						{
							platform_event textEvent;
							textEvent.type         = platform_event::text_entered;
							textEvent.text.unicode = unicode;
							push_event(textEvent);
						}
					}
				}
				else
                #endif
				{
					static XComposeStatus status;
					char keyBuffer[16];
					if (XLookupString(&windowEvent.xkey, keyBuffer, sizeof(keyBuffer), NULL, &status))
					{
						platform_event textEvent;
						textEvent.type         = platform_event::text_entered;
						textEvent.text.unicode = static_cast<std::uint32_t>(keyBuffer[0]);
						push_event(textEvent);
					}
				}
			}

			update_last_input_time(windowEvent.xkey.time);

			break;
	    }

		// key up event
	    case KeyRelease:
	    {
		    keyboard::key key = keyboard::Unknown;

			// Try each KeySym index (modifier group) until we get a match
			for (int i = 0; i < 4; ++i)
			{
				// Get the mml keyboard code from the keysym of the key that has been released
				key = keysym_to_mml(XLookupKeysym(&windowEvent.xkey, i));

				if (key != keyboard::Unknown)
					break;
			}

			// Fill the event parameters
			platform_event event;
			event.type        = platform_event::key_released;
			event.key.code    = key;
			event.key.alt     = windowEvent.xkey.state & Mod1Mask;
			event.key.control = windowEvent.xkey.state & ControlMask;
			event.key.shift   = windowEvent.xkey.state & ShiftMask;
			event.key.system  = windowEvent.xkey.state & Mod4Mask;
			push_event(event);

			break;
	    }

		// mouse button pressed
	    case ButtonPress:
	    {
		    // XXX: Why button 8 and 9?
		    // Because 4 and 5 are the vertical wheel and 6 and 7 are horizontal wheel ;)
		    unsigned int button = windowEvent.xbutton.button;
			if ((button == Button1) ||
			    (button == Button2) ||
			    (button == Button3) ||
			    (button == 8) ||
			    (button == 9))
			{
				platform_event event;
				event.type          = platform_event::mouse_button_pressed;
				event.mouse_button.x = windowEvent.xbutton.x;
				event.mouse_button.y = windowEvent.xbutton.y;
				switch(button)
				{
				    case Button1: event.mouse_button.button = mouse::left;     break;
				    case Button2: event.mouse_button.button = mouse::middle;   break;
				    case Button3: event.mouse_button.button = mouse::right;    break;
				    case 8:       event.mouse_button.button = mouse::x_button1; break;
				    case 9:       event.mouse_button.button = mouse::x_button2; break;
				}
				push_event(event);
			}

			update_last_input_time(windowEvent.xbutton.time);

			break;
	    }

		// mouse button released
	    case ButtonRelease:
	    {
		    unsigned int button = windowEvent.xbutton.button;
			if ((button == Button1) ||
			    (button == Button2) ||
			    (button == Button3) ||
			    (button == 8) ||
			    (button == 9))
			{
				platform_event event;
				event.type          = platform_event::mouse_button_released;
				event.mouse_button.x = windowEvent.xbutton.x;
				event.mouse_button.y = windowEvent.xbutton.y;
				switch(button)
				{
				    case Button1: event.mouse_button.button = mouse::left;     break;
				    case Button2: event.mouse_button.button = mouse::middle;   break;
				    case Button3: event.mouse_button.button = mouse::right;    break;
				    case 8:       event.mouse_button.button = mouse::x_button1; break;
				    case 9:       event.mouse_button.button = mouse::x_button2; break;
				}
				push_event(event);
			}
			else if ((button == Button4) || (button == Button5))
			{
				platform_event event;

				event.type                   = platform_event::mouse_wheel_scrolled;
				event.mouse_wheel_scroll.wheel = mouse::vertical_wheel;
				event.mouse_wheel_scroll.delta = (button == Button4) ? 1 : -1;
				event.mouse_wheel_scroll.x     = windowEvent.xbutton.x;
				event.mouse_wheel_scroll.y     = windowEvent.xbutton.y;
				push_event(event);
			}
			else if ((button == 6) || (button == 7))
			{
				platform_event event;
				event.type                   = platform_event::mouse_wheel_scrolled;
				event.mouse_wheel_scroll.wheel = mouse::horizontal_wheel;
				event.mouse_wheel_scroll.delta = (button == 6) ? 1 : -1;
				event.mouse_wheel_scroll.x     = windowEvent.xbutton.x;
				event.mouse_wheel_scroll.y     = windowEvent.xbutton.y;
				push_event(event);
			}
			break;
	    }

		// mouse moved
	    case MotionNotify:
	    {
		    platform_event event;
			event.type        = platform_event::mouse_moved;
			event.mouse_move.x = windowEvent.xmotion.x;
			event.mouse_move.y = windowEvent.xmotion.y;
			push_event(event);
			break;
	    }

		// mouse entered
	    case EnterNotify:
	    {
		    if (windowEvent.xcrossing.mode == NotifyNormal)
			{
				platform_event event;
				event.type = platform_event::mouse_entered;
				push_event(event);
			}
			break;
	    }

		// mouse left
	    case LeaveNotify:
	    {
		    if (windowEvent.xcrossing.mode == NotifyNormal)
			{
				platform_event event;
				event.type = platform_event::mouse_left;
				push_event(event);
			}
			break;
	    }

		// window unmapped
	    case UnmapNotify:
	    {
		    if (windowEvent.xunmap.window == _window)
				_window_mapped = false;

			break;
	    }

		// window visibility change
	    case VisibilityNotify:
	    {
		    // We prefer using VisibilityNotify over MapNotify because
		    // some window managers like awesome don't internally flag a
		    // window as viewable even after it is mapped but before it
		    // is visible leading to certain function calls failing with
		    // an unviewable error if called before VisibilityNotify arrives

		    // Empirical testing on most widely used window managers shows
		    // that mapping a window will always lead to a VisibilityNotify
		    // event that is not VisibilityFullyObscured
		    if (windowEvent.xvisibility.window == _window)
			{
				if (windowEvent.xvisibility.state != VisibilityFullyObscured)
					_window_mapped = true;
			}

			break;
	    }

		// window property change
	    case PropertyNotify:
	    {
		    if (!_last_input_time)
				_last_input_time = windowEvent.xproperty.time;

			break;
	    }
	}

	return true;
}

////////////////////////////////////////////////////////////
bool window_impl_x11::checkXRandR(int& xRandRMajor, int& xRandRMinor)
{
    // Check if the XRandR extension is present
    int version;
    if (!XQueryExtension(m_display, "RANDR", &version, &version, &version))
    {
        err() << "XRandR extension is not supported" << std::endl;
        return false;
    }

    // Check XRandR version, 1.2 required
    if (!XRRQueryVersion(m_display, &xRandRMajor, &xRandRMinor) || xRandRMajor < 1 || (xRandRMajor == 1 && xRandRMinor < 2 ))
    {
        err() << "XRandR is too old" << std::endl;
        return false;
    }

    return true;
}


////////////////////////////////////////////////////////////
RROutput window_impl_x11::getOutputPrimary(::Window& rootWindow, XRRScreenResources* res, int xRandRMajor, int xRandRMinor)
{
    // if xRandR version >= 1.3 get the primary screen else take the first screen
    if ((xRandRMajor == 1 && xRandRMinor >= 3) || xRandRMajor > 1)
    {
        RROutput output = XRRGetOutputPrimary(m_display, rootWindow);

        // Check if returned output is valid, otherwise use the first screen
        if (output == None)
            return res->outputs[0];
        else
            return output;
    }

    // xRandr version can't get the primary screen, use the first screen
    return res->outputs[0];
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> window_impl_x11::getPrimaryMonitorPosition()
{
    std::array<std::int32_t, 2> monitorPosition{{0, 0}};

    // Get root window
    ::Window rootWindow = RootWindow(_display, _screen);

    // Get the screen resources
    XRRScreenResources* res = XRRGetScreenResources(_display, rootWindow);
    if (!res)
    {
        err() << "Failed to get the current screen resources for.primary monitor position" << std::endl;
        return monitorPosition;
    }

    // Get xRandr version
    int xRandRMajor, xRandRMinor;
    if (!checkXRandR(xRandRMajor, xRandRMinor))
        xRandRMajor = xRandRMinor = 0;

    RROutput output = getOutputPrimary(rootWindow, res, xRandRMajor, xRandRMinor);

    // Get output info from output
    XRROutputInfo* outputInfo = XRRGetOutputInfo(_display, res, output);
    if (!outputInfo || outputInfo->connection == RR_Disconnected)
    {
        XRRFreeScreenResources(res);

        // If outputInfo->connection == RR_Disconnected, free output info
        if (outputInfo)
            XRRFreeOutputInfo(outputInfo);

        err() << "Failed to get output info for.primary monitor position" << std::endl;
        return monitorPosition;
    }

    // Retreive current RRMode, screen position and rotation
    XRRCrtcInfo* crtcInfo = XRRGetCrtcInfo(_display, res, outputInfo->crtc);
    if (!crtcInfo)
    {
        XRRFreeScreenResources(res);
        XRRFreeOutputInfo(outputInfo);
        err() << "Failed to get crtc info for.primary monitor position" << std::endl;
        return monitorPosition;
    }

    monitorPosition[0] = crtcInfo->x;
    monitorPosition[1] = crtcInfo->y;

    XRRFreeCrtcInfo(crtcInfo);
    XRRFreeOutputInfo(outputInfo);
    XRRFreeScreenResources(res);

    return monitorPosition;
}

} // namespace priv

} // namespace mml
