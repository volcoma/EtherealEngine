////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/window/win32/cursor_impl.hpp>
#include <mml/system/err.hpp>
#include <cstring>

namespace mml
{
namespace priv
{

////////////////////////////////////////////////////////////
cursor_impl::cursor_impl() :
_cursor(NULL)
{
    // That's it.
}


////////////////////////////////////////////////////////////
cursor_impl::~cursor_impl()
{
    release();
}


////////////////////////////////////////////////////////////
bool cursor_impl::load_from_pixels(const std::uint8_t* pixels, const std::array<std::uint32_t, 2>& size, const std::array<std::uint32_t, 2>& hotspot)
{
    release();

    // Create the bitmap that will hold our color data
    BITMAPV5HEADER bitmapHeader;
    std::memset(&bitmapHeader, 0, sizeof(BITMAPV5HEADER));

    bitmapHeader.bV5Size        = sizeof(BITMAPV5HEADER);
    bitmapHeader.bV5Width       = size[0];
    bitmapHeader.bV5Height      = -static_cast<int>(size[1]); // Negative indicates origin is in upper-left corner
    bitmapHeader.bV5Planes      = 1;
    bitmapHeader.bV5BitCount    = 32;
    bitmapHeader.bV5Compression = BI_BITFIELDS;
    bitmapHeader.bV5RedMask     = 0x00ff0000;
    bitmapHeader.bV5GreenMask   = 0x0000ff00;
    bitmapHeader.bV5BlueMask    = 0x000000ff;
    bitmapHeader.bV5AlphaMask   = 0xff000000;

    std::uint32_t* bitmapData = NULL;

    HDC screenDC = GetDC(NULL);
    HBITMAP color = CreateDIBSection(
        screenDC,
        reinterpret_cast<const BITMAPINFO*>(&bitmapHeader),
        DIB_RGB_COLORS,
        reinterpret_cast<void**>(&bitmapData),
        NULL,
        0
    );
    ReleaseDC(NULL, screenDC);

    if (!color)
    {
        err() << "Failed to create cursor color bitmap" << std::endl;
        return false;
    }

    // Fill our bitmap with the cursor color data
    std::uint32_t* bitmapOffset = bitmapData;
    for (std::size_t remaining = size[0] * size[1]; remaining > 0; --remaining, pixels += 4)
    {
        *bitmapOffset++ = (pixels[3] << 24) | (pixels[0] << 16) | (pixels[1] << 8) | pixels[2];
    }

    // Create a dummy mask bitmap (it won't be used)
    HBITMAP mask = CreateBitmap(size[0], size[1], 1, 1, NULL);

    if (!mask)
    {
        DeleteObject(color);
        err() << "Failed to create cursor mask bitmap" << std::endl;
        return false;
    }

    // Create the structure that describes our cursor
    ICONINFO cursorInfo;
    std::memset(&cursorInfo, 0, sizeof(ICONINFO));

    cursorInfo.fIcon    = FALSE; // This is a cursor and not an icon
    cursorInfo.xHotspot = hotspot[0];
    cursorInfo.yHotspot = hotspot[1];
    cursorInfo.hbmColor = color;
    cursorInfo.hbmMask  = mask;

    // Create the cursor
    _cursor = reinterpret_cast<HCURSOR>(CreateIconIndirect(&cursorInfo));

    // The data has been copied into the cursor, so get rid of these
    DeleteObject(color);
    DeleteObject(mask);

    if (_cursor)
    {
        return true;
    }
    else
    {
        err() << "Failed to create cursor from bitmaps" << std::endl;
        return false;
    }
}


////////////////////////////////////////////////////////////
bool cursor_impl::load_from_system(cursor::type type)
{
    release();

    LPCTSTR shape;
    switch (type)
    {
        case cursor::arrow:							shape = IDC_ARROW;       break;
        case cursor::arrow_wait:					shape = IDC_APPSTARTING; break;
        case cursor::wait:							shape = IDC_WAIT;        break;
        case cursor::text:							shape = IDC_IBEAM;       break;
        case cursor::hand:							shape = IDC_HAND;        break;
        case cursor::size_horizontal:				shape = IDC_SIZEWE;      break;
        case cursor::size_vertical:					shape = IDC_SIZENS;      break;
        case cursor::size_top_left_bottom_right:	shape = IDC_SIZENWSE;    break;
        case cursor::size_bottom_left_top_right:	shape = IDC_SIZENESW;    break;
        case cursor::size_all:						shape = IDC_SIZEALL;     break;
        case cursor::cross:							shape = IDC_CROSS;       break;
        case cursor::help:							shape = IDC_HELP;        break;
        case cursor::not_allowed:					shape = IDC_NO;          break;
    }

    // Create a copy of the shared system cursor that we can destroy later
    _cursor = CopyCursor(LoadCursor(NULL, shape));

    if (_cursor)
    {
        return true;
    }
    else
    {
        err() << "Could not create copy of a system cursor" << std::endl;
        return false;
    }
}


////////////////////////////////////////////////////////////
void cursor_impl::release()
{
    if (_cursor) {
        DestroyCursor(_cursor);
        _cursor = NULL;
    }
}

} // namespace priv

} // namespace mml

