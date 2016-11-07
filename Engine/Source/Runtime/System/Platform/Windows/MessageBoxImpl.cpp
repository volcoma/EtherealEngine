#include "System/MessageBox.h"
#include <windows.h>

namespace misc
{
	namespace
	{

		UINT getIcon(Style style)
		{
			switch (style) {
			case Style::Info:
				return MB_ICONINFORMATION;
			case Style::Warning:
				return MB_ICONWARNING;
			case Style::Error:
				return MB_ICONERROR;
			case Style::Question:
				return MB_ICONQUESTION;
			default:
				return MB_ICONINFORMATION;
			}
		}

		UINT getButtons(Buttons buttons)
		{
			switch (buttons) {
			case Buttons::OK:
				return MB_OK;
			case Buttons::OKCancel:
				return MB_OKCANCEL;
			case Buttons::YesNo:
				return MB_YESNO;
			default:
				return MB_OK;
			}
		}

		Selection getSelection(int response)
		{
			switch (response) {
			case IDOK:
				return Selection::OK;
			case IDCANCEL:
				return Selection::Cancel;
			case IDYES:
				return Selection::Yes;
			case IDNO:
				return Selection::No;
			default:
				return Selection::None;
			}
		}

	} // namespace

	Selection messageBox(const char *message, const char *title, Style style, Buttons buttons)
	{
		UINT flags = MB_TASKMODAL;

		flags |= getIcon(style);
		flags |= getButtons(buttons);

		return getSelection(MessageBox(nullptr, message, title, flags));
	}

}