#pragma once

namespace misc
{

	/// Options for styles to apply to a message box
	enum class Style
	{
		Info,
		Warning,
		Error,
		Question
	};


	/// Options for buttons to provide on a message box
	enum class Buttons
	{
		OK,
		OKCancel,
		YesNo
	};

	/// Possible responses from a message box. 'None' signifies that no option was chosen, and 'Error' signifies that an
	/// error was encountered while creating the message box.
	enum class Selection
	{
		OK,
		Cancel,
		Yes,
		No,
		None,
		Error
	};

	/// The default style to apply to a message box
	const Style kDefaultStyle = Style::Info;

	/// The default buttons to provide on a message box
	const Buttons kDefaultButtons = Buttons::OK;

	//-----------------------------------------------------------------------------
	//  Name : messageBox ()
	/// <summary>
	/// Blocking call to create a modal message box with the given message, title, style, and buttons
	/// </summary>
	//-----------------------------------------------------------------------------
	Selection messageBox(const char *message, const char *title, Style style, Buttons buttons);

	//-----------------------------------------------------------------------------
	//  Name : messageBox ()
	/// <summary>
	/// Convenience function to call messageBox() with the default buttons
	/// </summary>
	//-----------------------------------------------------------------------------
	inline Selection messageBox(const char *message, const char *title, Style style)
	{
		return messageBox(message, title, style, kDefaultButtons);
	}

	//-----------------------------------------------------------------------------
	//  Name : messageBox ()
	/// <summary>
	/// Convenience function to call messageBox() with the default style
	/// </summary>
	//-----------------------------------------------------------------------------
	inline Selection messageBox(const char *message, const char *title, Buttons buttons)
	{
		return messageBox(message, title, kDefaultStyle, buttons);
	}

	//-----------------------------------------------------------------------------
	//  Name : messageBox ()
	/// <summary>
	/// Convenience function to call messageBox() with the default style and buttons
	/// </summary>
	//-----------------------------------------------------------------------------
	inline Selection messageBox(const char *message, const char *title)
	{
		return messageBox(message, title, kDefaultStyle, kDefaultButtons);
	}
}