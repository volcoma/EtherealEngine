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
#define WINVER         0x0501
#include <mml/window/win32/window_impl_win32.hpp>
#include <mml/window/window_style.hpp>
#include <mml/system/err.hpp>
#include <mml/system/utf.hpp>
#include <vector>
#include <cstring>

// MinGW lacks the definition of some Win32 constants
#ifndef XBUTTON1
    #define XBUTTON1 0x0001
#endif
#ifndef XBUTTON2
    #define XBUTTON2 0x0002
#endif
#ifndef WM_MOUSEHWHEEL
    #define WM_MOUSEHWHEEL 0x020E
#endif
#ifndef MAPVK_VK_TO_VSC
    #define MAPVK_VK_TO_VSC (0)
#endif

// Avoid including <Dbt.h> just for one define
#ifndef DBT_DEVNODES_CHANGED
    #define DBT_DEVNODES_CHANGED 0x0007
#endif

namespace
{
    unsigned int               windowCount      = 0; // Windows owned by mml
    unsigned int               handleCount      = 0; // All window handles
    const wchar_t*             className        = L"MML_Window";
    mml::priv::window_impl_win32* fullscreenWindow = NULL;

    void set_process_dpi_aware()
    {
        // Try SetProcessDpiAwareness first
        HINSTANCE shCoreDll = LoadLibraryW(L"Shcore.dll");

        if (shCoreDll)
        {
            enum ProcessDpiAwareness
            {
                ProcessDpiUnaware         = 0,
                ProcessSystemDpiAware     = 1,
                ProcessPerMonitorDpiAware = 2
            };

            typedef HRESULT (WINAPI* SetProcessDpiAwarenessFuncType)(ProcessDpiAwareness);
            SetProcessDpiAwarenessFuncType SetProcessDpiAwarenessFunc = reinterpret_cast<SetProcessDpiAwarenessFuncType>(GetProcAddress(shCoreDll, "SetProcessDpiAwareness"));

            if (SetProcessDpiAwarenessFunc)
            {
                // We only check for E_INVALIDARG because we would get
                // E_ACCESSDENIED if the DPI was already set previously
                // and S_OK means the call was successful
                if (SetProcessDpiAwarenessFunc(ProcessSystemDpiAware) == E_INVALIDARG)
                {
                    mml::err() << "Failed to set process DPI awareness" << std::endl;
                }
                else
                {
                    FreeLibrary(shCoreDll);
                    return;
                }
            }

            FreeLibrary(shCoreDll);
        }

        // Fall back to SetProcessDPIAware if SetProcessDpiAwareness
        // is not available on this system
        HINSTANCE user32Dll = LoadLibraryW(L"user32.dll");

        if (user32Dll)
        {
            typedef BOOL (WINAPI* SetProcessDPIAwareFuncType)(void);
            SetProcessDPIAwareFuncType SetProcessDPIAwareFunc = reinterpret_cast<SetProcessDPIAwareFuncType>(GetProcAddress(user32Dll, "SetProcessDPIAware"));

            if (SetProcessDPIAwareFunc)
            {
                if (!SetProcessDPIAwareFunc())
                    mml::err() << "Failed to set process DPI awareness" << std::endl;
            }

            FreeLibrary(user32Dll);
        }
    }
}

namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
window_impl_win32::window_impl_win32(window_handle handle) :
_handle          (handle),
_callback        (0),
_cursor_visible   (true), // might need to call GetCursorInfo
_last_cursor      (LoadCursor(NULL, IDC_ARROW)),
_icon            (NULL),
_key_repeat_enabled(true),
_last_size        ({0, 0}),
_resizing        (false),
_surrogate       (0),
_mouse_inside     (false),
_fullscreen      (false),
_cursor_grabbed   (false)
{
    // Set that this process is DPI aware and can handle DPI scaling
    set_process_dpi_aware();

    if (_handle)
    {
        // If we're the first window handle, we only need to poll for joysticks when WM_DEVICECHANGE message is received
        if (handleCount == 0)
            joystick_impl::set_lazy_updates(true);

        ++handleCount;

        // We change the event procedure of the control (it is important to save the old one)
        SetWindowLongPtrW(_handle, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(this));
        _callback = SetWindowLongPtrW(_handle, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&window_impl_win32::global_on_event));
    }
}


////////////////////////////////////////////////////////////
window_impl_win32::window_impl_win32(video_mode mode, const std::string& title, std::uint32_t style) :
_handle          (NULL),
_callback        (0),
_cursor_visible   (true), // might need to call GetCursorInfo
_last_cursor      (LoadCursor(NULL, IDC_ARROW)),
_icon            (NULL),
_key_repeat_enabled(true),
_last_size        ({mode.width, mode.height}),
_resizing        (false),
_surrogate       (0),
_mouse_inside     (false),
_fullscreen      (style & style::fullscreen),
_cursor_grabbed   (_fullscreen)
{
    // Set that this process is DPI aware and can handle DPI scaling
    set_process_dpi_aware();

    // Register the window class at first call
    if (windowCount == 0)
        register_window_class();

    // Compute position and size
    HDC screenDC = GetDC(NULL);
    int left   = (GetDeviceCaps(screenDC, HORZRES) - static_cast<int>(mode.width))  / 2;
    int top    = (GetDeviceCaps(screenDC, VERTRES) - static_cast<int>(mode.height)) / 2;
    int width  = mode.width;
    int height = mode.height;
    ReleaseDC(NULL, screenDC);

    // Choose the window style according to the style parameter
    DWORD win32Style = WS_VISIBLE;
    if (style == style::none)
    {
        win32Style |= WS_POPUP;
    }
    else
    {
        if (style & style::titlebar) win32Style |= WS_CAPTION | WS_MINIMIZEBOX;
        if (style & style::resize)   win32Style |= WS_THICKFRAME | WS_MAXIMIZEBOX;
        if (style & style::close)    win32Style |= WS_SYSMENU;
    }

    // In windowed mode, adjust width and height so that window will have the requested client area
    if (!_fullscreen)
    {
        RECT rectangle = {0, 0, width, height};
        AdjustWindowRect(&rectangle, win32Style, false);
        width  = rectangle.right - rectangle.left;
        height = rectangle.bottom - rectangle.top;
    }
	std::wstring wtitle;
	wtitle.reserve(title.length() + 1);
	// Convert
	utf32::to_wide(title.begin(), title.end(), std::back_inserter(wtitle), 0);
    // Create the window
    _handle = CreateWindowW(className, wtitle.c_str(), win32Style, left, top, width, height, NULL, NULL, GetModuleHandle(NULL), this);

    // If we're the first window handle, we only need to poll for joysticks when WM_DEVICECHANGE message is received
    if (_handle)
    {
        if (handleCount == 0)
            joystick_impl::set_lazy_updates(true);

        ++handleCount;
    }

    // By default, the OS limits the size of the window the the desktop size,
    // we have to resize it after creation to apply the real size
	set_size(std::array<std::uint32_t, 2>({ mode.width, mode.height }));

    // Switch to fullscreen if requested
    if (_fullscreen)
        switch_to_fullscreen(mode);

    // Increment window count
    windowCount++;
}


////////////////////////////////////////////////////////////
window_impl_win32::~window_impl_win32()
{
    // TODO should we restore the cursor shape and visibility?

    // Destroy the custom icon, if any
    if (_icon)
        DestroyIcon(_icon);

    // If it's the last window handle we have to poll for joysticks again
    if (_handle)
    {
        --handleCount;

        if (handleCount == 0)
            joystick_impl::set_lazy_updates(false);
    }

    if (!_callback)
    {
        // Destroy the window
        if (_handle)
            DestroyWindow(_handle);

        // Decrement the window count
        windowCount--;

        // Unregister window class if we were the last window
        if (windowCount == 0)
            UnregisterClassW(className, GetModuleHandleW(NULL));
    }
    else
    {
        // The window is external: remove the hook on its message callback
        SetWindowLongPtrW(_handle, GWLP_WNDPROC, _callback);
    }
}


////////////////////////////////////////////////////////////
window_handle window_impl_win32::get_system_handle() const
{
    return _handle;
}


////////////////////////////////////////////////////////////
void window_impl_win32::process_events()
{
    // We process the window events only if we own it
    if (!_callback)
    {
        MSG message;
        while (PeekMessageW(&message, NULL, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&message);
            DispatchMessageW(&message);
        }
    }
}


////////////////////////////////////////////////////////////
std::array<std::int32_t, 2> window_impl_win32::get_position() const
{
    RECT rect;
    GetWindowRect(_handle, &rect);

	return std::array<std::int32_t, 2>({ rect.left, rect.top });
}


////////////////////////////////////////////////////////////
void window_impl_win32::set_position(const std::array<std::int32_t, 2>& position)
{
    SetWindowPos(_handle, NULL, position[0], position[1], 0, 0, SWP_NOSIZE | SWP_NOZORDER);

    if(_cursor_grabbed)
        grab_cursor(true);
}


////////////////////////////////////////////////////////////
std::array<std::uint32_t, 2> window_impl_win32::get_size() const
{
    RECT rect;
    GetClientRect(_handle, &rect);

	return std::array<std::uint32_t, 2>
		({ static_cast<std::uint32_t>(rect.right - rect.left), static_cast<std::uint32_t>(rect.bottom - rect.top) });
}


////////////////////////////////////////////////////////////
void window_impl_win32::set_size(const std::array<std::uint32_t, 2>& size)
{
    // SetWindowPos wants the total size of the window (including title bar and borders),
    // so we have to compute it
    RECT rectangle = {0, 0, static_cast<long>(size[0]), static_cast<long>(size[1])};
    AdjustWindowRect(&rectangle, GetWindowLong(_handle, GWL_STYLE), false);
    int width  = rectangle.right - rectangle.left;
    int height = rectangle.bottom - rectangle.top;

    SetWindowPos(_handle, NULL, 0, 0, width, height, SWP_NOMOVE | SWP_NOZORDER);
}


////////////////////////////////////////////////////////////
void window_impl_win32::set_title(const std::string& title)
{
	std::wstring wtitle;
	wtitle.reserve(title.length() + 1);

	// Convert
	utf32::to_wide(title.begin(), title.end(), std::back_inserter(wtitle), 0);
    SetWindowTextW(_handle, wtitle.c_str());
}


////////////////////////////////////////////////////////////
void window_impl_win32::set_icon(unsigned int width, unsigned int height, const std::uint8_t* pixels)
{
    // First destroy the previous one
    if (_icon)
        DestroyIcon(_icon);

    // Windows wants BGRA pixels: swap red and blue channels
    std::vector<std::uint8_t> iconPixels(width * height * 4);
    for (std::size_t i = 0; i < iconPixels.size() / 4; ++i)
    {
        iconPixels[i * 4 + 0] = pixels[i * 4 + 2];
        iconPixels[i * 4 + 1] = pixels[i * 4 + 1];
        iconPixels[i * 4 + 2] = pixels[i * 4 + 0];
        iconPixels[i * 4 + 3] = pixels[i * 4 + 3];
    }

    // Create the icon from the pixel array
    _icon = CreateIcon(GetModuleHandleW(NULL), width, height, 1, 32, NULL, &iconPixels[0]);

    // Set it as both big and small icon of the window
    if (_icon)
    {
        SendMessageW(_handle, WM_SETICON, ICON_BIG,   (LPARAM)_icon);
        SendMessageW(_handle, WM_SETICON, ICON_SMALL, (LPARAM)_icon);
    }
    else
    {
        err() << "Failed to set the window's icon" << std::endl;
    }
}


////////////////////////////////////////////////////////////
void window_impl_win32::set_visible(bool visible)
{
    ShowWindow(_handle, visible ? SW_SHOW : SW_HIDE);
}

////////////////////////////////////////////////////////////
void window_impl_win32::maximize()
{
	ShowWindow(_handle, SW_MAXIMIZE);
}

////////////////////////////////////////////////////////////
void window_impl_win32::minimize()
{
	ShowWindow(_handle, SW_MINIMIZE);
}

////////////////////////////////////////////////////////////
void window_impl_win32::restore()
{
	ShowWindow(_handle, SW_RESTORE);
}

////////////////////////////////////////////////////////////
void make_transparent(HWND hwnd)
{
	SetWindowLongPtrW(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
}
void make_opaque(HWND hwnd)
{
	SetWindowLongPtrW(hwnd, GWL_EXSTYLE, GetWindowLongPtr(hwnd, GWL_EXSTYLE) & ~WS_EX_LAYERED);
	RedrawWindow(hwnd, NULL, NULL, RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN);
}
void window_impl_win32::set_opacity(float opacity)
{
	HWND hwnd = get_system_handle();
	if (opacity >= 1.0f)
	{
		make_opaque(hwnd);
	}
	else
	{
		make_transparent(hwnd);
        const BYTE alpha = (BYTE) ((int) (opacity * 255.0f));
        SetLayeredWindowAttributes(get_system_handle(), 0, alpha, LWA_ALPHA);
	}
}


////////////////////////////////////////////////////////////
void window_impl_win32::set_mouse_cursor_visible(bool visible)
{
    // Don't call twice ShowCursor with the same parameter value;
    // we don't want to increment/decrement the internal counter
    // more than once.
    if (visible != _cursor_visible)
    {
        _cursor_visible = visible;
        ShowCursor(visible);
    }
}


////////////////////////////////////////////////////////////
void window_impl_win32::set_mouse_cursor_grabbed(bool grabbed)
{
    _cursor_grabbed = grabbed;
    grab_cursor(_cursor_grabbed);
}


////////////////////////////////////////////////////////////
void window_impl_win32::set_mouse_cursor(const cursor_impl& cursor)
{
    _last_cursor = cursor._cursor;
    SetCursor(_last_cursor);
}


////////////////////////////////////////////////////////////
void window_impl_win32::set_key_repeat_enabled(bool enabled)
{
    _key_repeat_enabled = enabled;
}


////////////////////////////////////////////////////////////
void window_impl_win32::request_focus()
{
    // Allow focus stealing only within the same process; compare PIDs of current and foreground window
    DWORD thisPid       = GetWindowThreadProcessId(_handle, NULL);
    DWORD foregroundPid = GetWindowThreadProcessId(GetForegroundWindow(), NULL);

    if (thisPid == foregroundPid)
    {
        // The window requesting focus belongs to the same process as the current window: steal focus
        SetForegroundWindow(_handle);
    }
    else
    {
        // Different process: don't steal focus, but create a taskbar notification ("flash")
        FLASHWINFO info;
        info.cbSize    = sizeof(info);
        info.hwnd      = _handle;
        info.dwFlags   = FLASHW_TRAY;
        info.dwTimeout = 0;
        info.uCount    = 3;

        FlashWindowEx(&info);
    }
}


////////////////////////////////////////////////////////////
bool window_impl_win32::has_focus() const
{
    return _handle == GetForegroundWindow();
}


////////////////////////////////////////////////////////////
void window_impl_win32::register_window_class()
{
    WNDCLASSW windowClass;
    windowClass.style         = 0;
    windowClass.lpfnWndProc   = &window_impl_win32::global_on_event;
    windowClass.cbClsExtra    = 0;
    windowClass.cbWndExtra    = 0;
    windowClass.hInstance     = GetModuleHandleW(NULL);
    windowClass.hIcon         = NULL;
    windowClass.hCursor       = 0;
    windowClass.hbrBackground = 0;
    windowClass.lpszMenuName  = NULL;
    windowClass.lpszClassName = className;
    RegisterClassW(&windowClass);
}


////////////////////////////////////////////////////////////
void window_impl_win32::switch_to_fullscreen(const video_mode& mode)
{
    DEVMODEW devMode;
    devMode.dmSize       = sizeof(devMode);
    devMode.dmPelsWidth  = mode.width;
    devMode.dmPelsHeight = mode.height;
    devMode.dmBitsPerPel = mode.bits_per_pixel;
    devMode.dmFields     = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL;

    // Apply fullscreen mode
    if (ChangeDisplaySettingsW(&devMode, CDS_FULLSCREEN) != DISP_CHANGE_SUCCESSFUL)
    {
        err() << "Failed to change display mode for fullscreen" << std::endl;
        return;
    }

    // Make the window flags compatible with fullscreen mode
    SetWindowLongW(_handle, GWL_STYLE, WS_POPUP | WS_CLIPCHILDREN | WS_CLIPSIBLINGS);
    SetWindowLongW(_handle, GWL_EXSTYLE, WS_EX_APPWINDOW);

    // Resize the window so that it fits the entire screen
    SetWindowPos(_handle, HWND_TOP, 0, 0, mode.width, mode.height, SWP_FRAMECHANGED);
    ShowWindow(_handle, SW_SHOW);

    // Set "this" as the current fullscreen window
    fullscreenWindow = this;
}


////////////////////////////////////////////////////////////
void window_impl_win32::cleanup()
{
    // Restore the previous video mode (in case we were running in fullscreen)
    if (fullscreenWindow == this)
    {
        ChangeDisplaySettingsW(NULL, 0);
        fullscreenWindow = NULL;
    }

    // Unhide the mouse cursor (in case it was hidden)
    set_mouse_cursor_visible(true);

    // No longer track the cursor
    set_tracking(false);

    // No longer capture the cursor
    ReleaseCapture();
}


////////////////////////////////////////////////////////////
void window_impl_win32::set_tracking(bool track)
{
    TRACKMOUSEEVENT mouseEvent;
    mouseEvent.cbSize = sizeof(TRACKMOUSEEVENT);
    mouseEvent.dwFlags = track ? TME_LEAVE : TME_CANCEL;
    mouseEvent.hwndTrack = _handle;
    mouseEvent.dwHoverTime = HOVER_DEFAULT;
    TrackMouseEvent(&mouseEvent);
}


////////////////////////////////////////////////////////////
void window_impl_win32::grab_cursor(bool grabbed)
{
    if (grabbed)
    {
        RECT rect;
        GetClientRect(_handle, &rect);
        MapWindowPoints(_handle, NULL, reinterpret_cast<LPPOINT>(&rect), 2);
        ClipCursor(&rect);
    }
    else
    {
        ClipCursor(NULL);
    }
}


////////////////////////////////////////////////////////////
void window_impl_win32::process_event(UINT message, WPARAM wParam, LPARAM lParam)
{
    // Don't process any message until window is created
    if (_handle == NULL)
        return;

    switch (message)
    {
        // Destroy event
        case WM_DESTROY:
        {
            // Here we must cleanup resources !
            cleanup();
            break;
        }

        // Set cursor event
        case WM_SETCURSOR:
        {
            // The mouse has moved, if the cursor is in our window we must refresh the cursor
            if (LOWORD(lParam) == HTCLIENT)
                SetCursor(_last_cursor);

            break;
        }

        // Close event
        case WM_CLOSE:
        {
            platform_event event;
            event.type = platform_event::closed;
            push_event(event);
            break;
        }

        // Resize event
        case WM_SIZE:
        {
            // Consider only events triggered by a maximize or a un-maximize
            if (wParam != SIZE_MINIMIZED && !_resizing && _last_size != get_size())
            {
                // Update the last handled size
                _last_size = get_size();

                // Push a resize event
                platform_event event;
                event.type        = platform_event::resized;
                event.size.width  = _last_size[0];
                event.size.height = _last_size[1];
                push_event(event);

                // Restore/update cursor grabbing
                grab_cursor(_cursor_grabbed);
            }
            break;
        }

        // Start resizing
        case WM_ENTERSIZEMOVE:
        {
            _resizing = true;
            grab_cursor(false);
            break;
        }

        // Stop resizing
        case WM_EXITSIZEMOVE:
        {
            _resizing = false;

            // Ignore cases where the window has only been moved
            if(_last_size != get_size())
            {
                // Update the last handled size
                _last_size = get_size();

                // Push a resize event
                platform_event event;
                event.type        = platform_event::resized;
                event.size.width  = _last_size[0];
                event.size.height = _last_size[1];
                push_event(event);
            }

            // Restore/update cursor grabbing
            grab_cursor(_cursor_grabbed);
            break;
        }

        // The system request the min/max window size and position
        case WM_GETMINMAXINFO:
        {
            // We override the returned information to remove the default limit
            // (the OS doesn't allow windows bigger than the desktop by default)
            MINMAXINFO* info = reinterpret_cast<MINMAXINFO*>(lParam);
            info->ptMaxTrackSize.x = 50000;
            info->ptMaxTrackSize.y = 50000;
            break;
        }

        // Gain focus event
        case WM_SETFOCUS:
        {
            // Restore cursor grabbing
            grab_cursor(_cursor_grabbed);

            platform_event event;
            event.type = platform_event::gained_focus;
            push_event(event);
            break;
        }

        // Lost focus event
        case WM_KILLFOCUS:
        {
            // Ungrab the cursor
            grab_cursor(false);

            platform_event event;
            event.type = platform_event::lost_focus;
            push_event(event);
            break;
        }

        // Text event
        case WM_CHAR:
        {
            if (_key_repeat_enabled || ((lParam & (1 << 30)) == 0))
            {
                // Get the code of the typed character
				std::uint32_t character = static_cast<std::uint32_t>(wParam);

                // Check if it is the first part of a surrogate pair, or a regular character
                if ((character >= 0xD800) && (character <= 0xDBFF))
                {
                    // First part of a surrogate pair: store it and wait for the second one
                    _surrogate = static_cast<std::uint16_t>(character);
                }
                else
                {
                    // Check if it is the second part of a surrogate pair, or a regular character
                    if ((character >= 0xDC00) && (character <= 0xDFFF))
                    {
                        // Convert the UTF-16 surrogate pair to a single UTF-32 value
                        std::uint16_t utf16[] = {_surrogate, static_cast<std::uint16_t>(character)};
                        mml::utf16::to_utf32(utf16, utf16 + 2, &character);
                        _surrogate = 0;
                    }

                    // Send a text_entered event
                    platform_event event;
                    event.type = platform_event::text_entered;
                    event.text.unicode = character;
                    push_event(event);
                }
            }
            break;
        }

        // Keydown event
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            if (_key_repeat_enabled || ((HIWORD(lParam) & KF_REPEAT) == 0))
            {
                platform_event event;
                event.type        = platform_event::key_pressed;
                event.key.alt     = HIWORD(GetAsyncKeyState(VK_MENU))    != 0;
                event.key.control = HIWORD(GetAsyncKeyState(VK_CONTROL)) != 0;
                event.key.shift   = HIWORD(GetAsyncKeyState(VK_SHIFT))   != 0;
                event.key.system  = HIWORD(GetAsyncKeyState(VK_LWIN)) || HIWORD(GetAsyncKeyState(VK_RWIN));
                event.key.code    = virtual_key_code_to_mml(wParam, lParam);
                push_event(event);
            }
            break;
        }

        // Keyup event
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            platform_event event;
            event.type        = platform_event::key_released;
            event.key.alt     = HIWORD(GetAsyncKeyState(VK_MENU))    != 0;
            event.key.control = HIWORD(GetAsyncKeyState(VK_CONTROL)) != 0;
            event.key.shift   = HIWORD(GetAsyncKeyState(VK_SHIFT))   != 0;
            event.key.system  = HIWORD(GetAsyncKeyState(VK_LWIN)) || HIWORD(GetAsyncKeyState(VK_RWIN));
            event.key.code    = virtual_key_code_to_mml(wParam, lParam);
            push_event(event);
            break;
        }

        // Vertical mouse wheel event
        case WM_MOUSEWHEEL:
        {
            // mouse position is in screen coordinates, convert it to window coordinates
            POINT position;
            position.x = static_cast<std::int16_t>(LOWORD(lParam));
            position.y = static_cast<std::int16_t>(HIWORD(lParam));
            ScreenToClient(_handle, &position);

            std::int16_t delta = static_cast<std::int16_t>(HIWORD(wParam));

            platform_event event;

            event.type                   = platform_event::mouse_wheel_scrolled;
            event.mouse_wheel_scroll.wheel = mouse::vertical_wheel;
            event.mouse_wheel_scroll.delta = static_cast<float>(delta) / 120.f;
            event.mouse_wheel_scroll.x     = position.x;
            event.mouse_wheel_scroll.y     = position.y;
            push_event(event);
            break;
        }

        // Horizontal mouse wheel event
        case WM_MOUSEHWHEEL:
        {
            // mouse position is in screen coordinates, convert it to window coordinates
            POINT position;
            position.x = static_cast<std::int16_t>(LOWORD(lParam));
            position.y = static_cast<std::int16_t>(HIWORD(lParam));
            ScreenToClient(_handle, &position);

            std::int16_t delta = static_cast<std::int16_t>(HIWORD(wParam));

            platform_event event;
            event.type                   = platform_event::mouse_wheel_scrolled;
            event.mouse_wheel_scroll.wheel = mouse::horizontal_wheel;
            event.mouse_wheel_scroll.delta = -static_cast<float>(delta) / 120.f;
            event.mouse_wheel_scroll.x     = position.x;
            event.mouse_wheel_scroll.y     = position.y;
            push_event(event);
            break;
        }

        // mouse left button down event
        case WM_LBUTTONDOWN:
        {
            platform_event event;
            event.type               = platform_event::mouse_button_pressed;
            event.mouse_button.button = mouse::left;
            event.mouse_button.x      = static_cast<std::int16_t>(LOWORD(lParam));
            event.mouse_button.y      = static_cast<std::int16_t>(HIWORD(lParam));
            push_event(event);
            break;
        }

        // mouse left button up event
        case WM_LBUTTONUP:
        {
            platform_event event;
            event.type               = platform_event::mouse_button_released;
            event.mouse_button.button = mouse::left;
            event.mouse_button.x      = static_cast<std::int16_t>(LOWORD(lParam));
            event.mouse_button.y      = static_cast<std::int16_t>(HIWORD(lParam));
            push_event(event);
            break;
        }

        // mouse right button down event
        case WM_RBUTTONDOWN:
        {
            platform_event event;
            event.type               = platform_event::mouse_button_pressed;
            event.mouse_button.button = mouse::right;
            event.mouse_button.x      = static_cast<std::int16_t>(LOWORD(lParam));
            event.mouse_button.y      = static_cast<std::int16_t>(HIWORD(lParam));
            push_event(event);
            break;
        }

        // mouse right button up event
        case WM_RBUTTONUP:
        {
            platform_event event;
            event.type               = platform_event::mouse_button_released;
            event.mouse_button.button = mouse::right;
            event.mouse_button.x      = static_cast<std::int16_t>(LOWORD(lParam));
            event.mouse_button.y      = static_cast<std::int16_t>(HIWORD(lParam));
            push_event(event);
            break;
        }

        // mouse wheel button down event
        case WM_MBUTTONDOWN:
        {
            platform_event event;
            event.type               = platform_event::mouse_button_pressed;
            event.mouse_button.button = mouse::middle;
            event.mouse_button.x      = static_cast<std::int16_t>(LOWORD(lParam));
            event.mouse_button.y      = static_cast<std::int16_t>(HIWORD(lParam));
            push_event(event);
            break;
        }

        // mouse wheel button up event
        case WM_MBUTTONUP:
        {
            platform_event event;
            event.type               = platform_event::mouse_button_released;
            event.mouse_button.button = mouse::middle;
            event.mouse_button.x      = static_cast<std::int16_t>(LOWORD(lParam));
            event.mouse_button.y      = static_cast<std::int16_t>(HIWORD(lParam));
            push_event(event);
            break;
        }

        // mouse X button down event
        case WM_XBUTTONDOWN:
        {
            platform_event event;
            event.type               = platform_event::mouse_button_pressed;
            event.mouse_button.button = HIWORD(wParam) == XBUTTON1 ? mouse::x_button1 : mouse::x_button2;
            event.mouse_button.x      = static_cast<std::int16_t>(LOWORD(lParam));
            event.mouse_button.y      = static_cast<std::int16_t>(HIWORD(lParam));
            push_event(event);
            break;
        }

        // mouse X button up event
        case WM_XBUTTONUP:
        {
            platform_event event;
            event.type               = platform_event::mouse_button_released;
            event.mouse_button.button = HIWORD(wParam) == XBUTTON1 ? mouse::x_button1 : mouse::x_button2;
            event.mouse_button.x      = static_cast<std::int16_t>(LOWORD(lParam));
            event.mouse_button.y      = static_cast<std::int16_t>(HIWORD(lParam));
            push_event(event);
            break;
        }

        // mouse leave event
        case WM_MOUSELEAVE:
        {
            // Avoid this firing a second time in case the cursor is dragged outside
            if (_mouse_inside)
            {
                _mouse_inside = false;

                // Generate a mouse_left event
                platform_event event;
                event.type = platform_event::mouse_left;
                push_event(event);
            }
            break;
        }

        // mouse move event
        case WM_MOUSEMOVE:
        {
            // Extract the mouse local coordinates
            int x = static_cast<std::int16_t>(LOWORD(lParam));
            int y = static_cast<std::int16_t>(HIWORD(lParam));

            // Get the client area of the window
            RECT area;
            GetClientRect(_handle, &area);

            // Capture the mouse in case the user wants to drag it outside
            if ((wParam & (MK_LBUTTON | MK_MBUTTON | MK_RBUTTON | MK_XBUTTON1 | MK_XBUTTON2)) == 0)
            {
                // Only release the capture if we really have it
                if (GetCapture() == _handle)
                    ReleaseCapture();
            }
            else if (GetCapture() != _handle)
            {
                // Set the capture to continue receiving mouse events
                SetCapture(_handle);
            }

            // If the cursor is outside the client area...
            if ((x < area.left) || (x > area.right) || (y < area.top) || (y > area.bottom))
            {
                // and it used to be inside, the mouse left it.
                if (_mouse_inside)
                {
                    _mouse_inside = false;

                    // No longer care for the mouse leaving the window
                    set_tracking(false);

                    // Generate a mouse_left event
                    platform_event event;
                    event.type = platform_event::mouse_left;
                    push_event(event);
                }
            }
            else
            {
                // and vice-versa
                if (!_mouse_inside)
                {
                    _mouse_inside = true;

                    // Look for the mouse leaving the window
                    set_tracking(true);

                    // Generate a mouse_entered event
                    platform_event event;
                    event.type = platform_event::mouse_entered;
                    push_event(event);
                }
            }

            // Generate a MouseMove event
            platform_event event;
            event.type        = platform_event::mouse_moved;
            event.mouse_move.x = x;
            event.mouse_move.y = y;
            push_event(event);
            break;
        }
        case WM_DEVICECHANGE:
        {
            // Some sort of device change has happened, update joystick connections
            if (wParam == DBT_DEVNODES_CHANGED)
                joystick_impl::update_connections();
            break;
        }
    }
}


////////////////////////////////////////////////////////////
keyboard::key window_impl_win32::virtual_key_code_to_mml(WPARAM key, LPARAM flags)
{
    switch (key)
    {
        // Check the scancode to distinguish between left and right shift
        case VK_SHIFT:
        {
            static UINT lShift = MapVirtualKeyW(VK_LSHIFT, MAPVK_VK_TO_VSC);
            UINT scancode = static_cast<UINT>((flags & (0xFF << 16)) >> 16);
            return scancode == lShift ? keyboard::LShift : keyboard::RShift;
        }

        // Check the "extended" flag to distinguish between left and right alt
        case VK_MENU : return (HIWORD(flags) & KF_EXTENDED) ? keyboard::RAlt : keyboard::LAlt;

        // Check the "extended" flag to distinguish between left and right control
        case VK_CONTROL : return (HIWORD(flags) & KF_EXTENDED) ? keyboard::RControl : keyboard::LControl;

        // Other keys are reported properly
        case VK_LWIN:       return keyboard::LSystem;
        case VK_RWIN:       return keyboard::RSystem;
        case VK_APPS:       return keyboard::Menu;
        case VK_OEM_1:      return keyboard::SemiColon;
        case VK_OEM_2:      return keyboard::Slash;
        case VK_OEM_PLUS:   return keyboard::Equal;
        case VK_OEM_MINUS:  return keyboard::Dash;
        case VK_OEM_4:      return keyboard::LBracket;
        case VK_OEM_6:      return keyboard::RBracket;
        case VK_OEM_COMMA:  return keyboard::Comma;
        case VK_OEM_PERIOD: return keyboard::Period;
        case VK_OEM_7:      return keyboard::Quote;
        case VK_OEM_5:      return keyboard::BackSlash;
        case VK_OEM_3:      return keyboard::Tilde;
        case VK_ESCAPE:     return keyboard::Escape;
        case VK_SPACE:      return keyboard::Space;
        case VK_RETURN:     return keyboard::Return;
        case VK_BACK:       return keyboard::BackSpace;
        case VK_TAB:        return keyboard::Tab;
        case VK_PRIOR:      return keyboard::PageUp;
        case VK_NEXT:       return keyboard::PageDown;
        case VK_END:        return keyboard::End;
        case VK_HOME:       return keyboard::Home;
        case VK_INSERT:     return keyboard::Insert;
        case VK_DELETE:     return keyboard::Delete;
        case VK_ADD:        return keyboard::Add;
        case VK_SUBTRACT:   return keyboard::Subtract;
        case VK_MULTIPLY:   return keyboard::Multiply;
        case VK_DIVIDE:     return keyboard::Divide;
        case VK_PAUSE:      return keyboard::Pause;
        case VK_F1:         return keyboard::F1;
        case VK_F2:         return keyboard::F2;
        case VK_F3:         return keyboard::F3;
        case VK_F4:         return keyboard::F4;
        case VK_F5:         return keyboard::F5;
        case VK_F6:         return keyboard::F6;
        case VK_F7:         return keyboard::F7;
        case VK_F8:         return keyboard::F8;
        case VK_F9:         return keyboard::F9;
        case VK_F10:        return keyboard::F10;
        case VK_F11:        return keyboard::F11;
        case VK_F12:        return keyboard::F12;
        case VK_F13:        return keyboard::F13;
        case VK_F14:        return keyboard::F14;
        case VK_F15:        return keyboard::F15;
        case VK_LEFT:       return keyboard::Left;
        case VK_RIGHT:      return keyboard::Right;
        case VK_UP:         return keyboard::Up;
        case VK_DOWN:       return keyboard::Down;
        case VK_NUMPAD0:    return keyboard::Numpad0;
        case VK_NUMPAD1:    return keyboard::Numpad1;
        case VK_NUMPAD2:    return keyboard::Numpad2;
        case VK_NUMPAD3:    return keyboard::Numpad3;
        case VK_NUMPAD4:    return keyboard::Numpad4;
        case VK_NUMPAD5:    return keyboard::Numpad5;
        case VK_NUMPAD6:    return keyboard::Numpad6;
        case VK_NUMPAD7:    return keyboard::Numpad7;
        case VK_NUMPAD8:    return keyboard::Numpad8;
        case VK_NUMPAD9:    return keyboard::Numpad9;
        case 'A':           return keyboard::A;
        case 'Z':           return keyboard::Z;
        case 'E':           return keyboard::E;
        case 'R':           return keyboard::R;
        case 'T':           return keyboard::T;
        case 'Y':           return keyboard::Y;
        case 'U':           return keyboard::U;
        case 'I':           return keyboard::I;
        case 'O':           return keyboard::O;
        case 'P':           return keyboard::P;
        case 'Q':           return keyboard::Q;
        case 'S':           return keyboard::S;
        case 'D':           return keyboard::D;
        case 'F':           return keyboard::F;
        case 'G':           return keyboard::G;
        case 'H':           return keyboard::H;
        case 'J':           return keyboard::J;
        case 'K':           return keyboard::K;
        case 'L':           return keyboard::L;
        case 'M':           return keyboard::M;
        case 'W':           return keyboard::W;
        case 'X':           return keyboard::X;
        case 'C':           return keyboard::C;
        case 'V':           return keyboard::V;
        case 'B':           return keyboard::B;
        case 'N':           return keyboard::N;
        case '0':           return keyboard::Num0;
        case '1':           return keyboard::Num1;
        case '2':           return keyboard::Num2;
        case '3':           return keyboard::Num3;
        case '4':           return keyboard::Num4;
        case '5':           return keyboard::Num5;
        case '6':           return keyboard::Num6;
        case '7':           return keyboard::Num7;
        case '8':           return keyboard::Num8;
        case '9':           return keyboard::Num9;
    }

    return keyboard::Unknown;
}


////////////////////////////////////////////////////////////
LRESULT CALLBACK window_impl_win32::global_on_event(HWND handle, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Associate handle and window instance when the creation message is received
    if (message == WM_CREATE)
    {
        // Get window_impl_win32 instance (it was passed as the last argument of CreateWindow)
        LONG_PTR window = (LONG_PTR)reinterpret_cast<CREATESTRUCT*>(lParam)->lpCreateParams;

        // Set as the "user data" parameter of the window
        SetWindowLongPtrW(handle, GWLP_USERDATA, window);
    }

    // Get the WindowImpl instance corresponding to the window handle
    window_impl_win32* window = handle ? reinterpret_cast<window_impl_win32*>(GetWindowLongPtr(handle, GWLP_USERDATA)) : NULL;

    // Forward the event to the appropriate function
    if (window)
    {
        window->process_event(message, wParam, lParam);

        if (window->_callback)
            return CallWindowProcW(reinterpret_cast<WNDPROC>(window->_callback), handle, message, wParam, lParam);
    }

    // We don't forward the WM_CLOSE message to prevent the OS from automatically destroying the window
    if (message == WM_CLOSE)
        return 0;

    // Don't forward the menu system command, so that pressing ALT or F10 doesn't steal the focus
    if ((message == WM_SYSCOMMAND) && (wParam == SC_KEYMENU))
        return 0;

    return DefWindowProcW(handle, message, wParam, lParam);
}

} // namespace priv

} // namespace mml

