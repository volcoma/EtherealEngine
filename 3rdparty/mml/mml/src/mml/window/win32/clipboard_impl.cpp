////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include <mml/system/utf.hpp>
#include <mml/window/win32/clipboard_impl.hpp>

#include <iostream>
#include <windows.h>

namespace mml
{
namespace priv
{
////////////////////////////////////////////////////////////
std::string clipboard_impl::get_string()
{
	std::string text;

	if(!IsClipboardFormatAvailable(CF_UNICODETEXT))
	{
		std::cerr << "Failed to get the clipboard data in Unicode format." << std::endl;
		return text;
	}

	if(!OpenClipboard(nullptr))
	{
		std::cerr << "Failed to open the Win32 clipboard." << std::endl;
		return text;
	}

	HANDLE clipboard_handle = GetClipboardData(CF_UNICODETEXT);

	if(!clipboard_handle)
	{
		std::cerr << "Failed to get Win32 handle for clipboard content." << std::endl;
		CloseClipboard();
		return text;
	}

	auto clip = std::wstring(static_cast<wchar_t*>(GlobalLock(clipboard_handle)));
	utf16::to_utf8(std::begin(clip), std::end(clip), std::begin(text));
	GlobalUnlock(clipboard_handle);

	CloseClipboard();
	return text;
}

////////////////////////////////////////////////////////////
void clipboard_impl::set_string(const std::string& text)
{
	if(!OpenClipboard(nullptr))
	{
		std::cerr << "Failed to open the Win32 clipboard." << std::endl;
		return;
	}

	if(!EmptyClipboard())
	{
		std::cerr << "Failed to empty the Win32 clipboard." << std::endl;
		return;
	}

	// Create a Win32-compatible string
	std::wstring wide;
	utf8::to_wide(std::begin(text), std::end(text), std::back_inserter(wide));
	size_t string_size = (wide.size() + 1) * sizeof(WCHAR);
	HANDLE string_handle = GlobalAlloc(GMEM_MOVEABLE, string_size);

	if(string_handle)
	{
		memcpy(GlobalLock(string_handle), wide.data(), string_size);
		GlobalUnlock(string_handle);
		SetClipboardData(CF_UNICODETEXT, string_handle);
	}

	CloseClipboard();
}

} // namespace priv

} // namespace mml
