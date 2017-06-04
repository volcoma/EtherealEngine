#pragma once

//-----------------------------------------------------------------------------
// std::string Header Includes
//-----------------------------------------------------------------------------
#include <string>   // std::string / std::wstd::string

#include <cctype>   // toupper / tolower
#include <stdarg.h>
#include <algorithm>
#include <locale>
#include <vector>
#include <stdio.h>
namespace string_utils
{
	class StrICmp
	{
	public:
        StrICmp() = default;
		//---------------------------------------------------------------------
		// Constructors & Destructors
		//---------------------------------------------------------------------
        StrICmp(const char * lpszLang) : mLocale(lpszLang) {}

		//---------------------------------------------------------------------
		// Public Classes
		//---------------------------------------------------------------------
		//---------------------------------------------------------------------
		// Name : CharLessI (Class)
		// Desc : Case insensitive character less-than operator
		//---------------------------------------------------------------------
		class CharLessI
		{
		public:
			//-----------------------------------------------------------------
			// Constructors & Destructors
			//-----------------------------------------------------------------
			CharLessI(std::locale & Locale) : mLocale(Locale) {}

			//-----------------------------------------------------------------
			// Public Operators
			//-----------------------------------------------------------------
			template<typename T>
			bool operator()(T c1, T c2)
			{
				return std::tolower(c1, mLocale) < std::tolower(c2, mLocale);

			} // End Operator

		private:
			//-----------------------------------------------------------------
			// Private Member Variables
			//-----------------------------------------------------------------
			std::locale & mLocale;
		};

		//---------------------------------------------------------------------
		// Public Operators
		//---------------------------------------------------------------------
		int operator()(const std::string & s1, const std::string & s2)
		{
			if (std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), CharLessI(mLocale)))
				return -1;
			if (std::lexicographical_compare(s2.begin(), s2.end(), s1.begin(), s1.end(), CharLessI(mLocale)))
				return 1;
			return 0;

		} // End Operator

	private:
		//---------------------------------------------------------------------
		// Private Member Variables
		//---------------------------------------------------------------------
		std::locale mLocale;
	};
	//-------------------------------------------------------------------------
	//  Name : compare ()
	/// <summary>
	/// Compare the two std::strings with optional case ignore.
	/// </summary>
	//-------------------------------------------------------------------------
    int compare(const std::string& s1, const std::string & s2, bool ignoreCase);
	//-------------------------------------------------------------------------
	//  Name : trim()
	/// <summary>
	/// Strips all white space (space and tab characters) from both the head 
	/// and tail of this std::string.
	/// </summary>
	//-------------------------------------------------------------------------
	std::string trim(const std::string& str);

	//-------------------------------------------------------------------------
	//  Name : split()
	/// <summary>
	/// Splits the string into multiple substrings.
	/// </summary>
	//-------------------------------------------------------------------------
	std::vector<std::string> split(const std::string &text, char sep, bool skipEmpty = true);

	//-------------------------------------------------------------------------
	//  Name : to_upper()
	/// <summary>
	/// Process the std::string, transforming all characters to upper case where
	/// appropriate.
	/// </summary>
	//-------------------------------------------------------------------------
	std::string to_upper(const std::string& str);

	//-------------------------------------------------------------------------
	//  Name : to_lower()
	/// <summary>
	/// Process the std::string, transforming all characters to lower case where
	/// appropriate.
	/// </summary>
	//-------------------------------------------------------------------------
	std::string to_lower(const std::string& str);

	//-------------------------------------------------------------------------
	//  Name : begins_with ()
	/// <summary>
	/// Determine if this std::string begins with the std::string provided.
	/// </summary>
	//-------------------------------------------------------------------------
	bool begins_with(const std::string& str, const std::string & value, bool ignoreCase = false);

	//-------------------------------------------------------------------------
	//  Name : ends_with ()
	/// <summary>
	/// Determine if this std::string ends with the std::string provided.
	/// </summary>
	//-------------------------------------------------------------------------
	bool ends_with(const std::string& str, const std::string & value, bool ignoreCase = false);

	//-------------------------------------------------------------------------
	//  Name : replace ()
	/// <summary>
	/// Replacing any occurences of the specified character sequence with 
	/// another.
	/// </summary>
	//-------------------------------------------------------------------------
	std::string replace(const std::string& str, const std::string & oldSequence, const std::string & newSequence);

	//-------------------------------------------------------------------------
	//  Name : replace ()
	/// <summary>
	/// Replacing any occurances of the specified character with  another 
	/// directly in-place within this std::string.
	/// </summary>
	//-------------------------------------------------------------------------
	std::string replace(const std::string& str, std::string::value_type oldCharacter, std::string::value_type newCharacter);

	//-------------------------------------------------------------------------
	//  Name : format ()
	/// <summary>
	/// Generate a newly formatted std::string based on a format descriptor and
	/// an optional variable length list of arguments.
	/// </summary>
	//-------------------------------------------------------------------------
	template< typename... Args >
	std::string format(const char* format, Args... args)
	{
		int length = std::snprintf(nullptr, 0, format, args...);
		if (length == 0)
			return std::string();

		char* buf = new char[length + 1];
		length = std::snprintf(buf, length + 1, format, args...);

		std::string str(buf);
		delete[] buf;
		return str;
	}

	//-------------------------------------------------------------------------
	//  Name : format ()
	/// <summary>
	/// Generate a newly formatted std::string based on a format descriptor and
	/// an optional variable bufferLength list of arguments.
	/// </summary>
	//-------------------------------------------------------------------------
	std::string format(const char* format, va_list args);

	//-------------------------------------------------------------------------
	//  Name : word_wrap ()
	/// <summary>
	/// Wraps the std::string up to the maximum length and optionally inserts an
	/// aribitrary padding std::string at the beginning of each new line
	/// </summary>
	//-------------------------------------------------------------------------
	std::string word_wrap(const std::string & value, std::string::size_type maximumLength, const std::string & linePadding = "");

	//-----------------------------------------------------------------------------
	//  Name : command_line_args ()
	/// <summary>
	/// Creates a single string command line.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string command_line_args(int _argc, char* _argv[]);

	//-----------------------------------------------------------------------------
	//  Name : parse_command_line ()
	/// <summary>
	/// Split single string command line into its component parts.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool parse_command_line(const std::string & strCommandLine, std::vector<std::string> & ArgumentsOut);
}
