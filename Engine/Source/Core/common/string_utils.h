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
		//---------------------------------------------------------------------
		// Constructors & Destructors
		//---------------------------------------------------------------------
		StrICmp(const char * lpszLang = "english") : mLocale(lpszLang) {}

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
	int compare(const std::string& s1, const std::string & s2, bool ignoreCase, const char * language = "english");
	//-------------------------------------------------------------------------
	//  Name : trim()
	/// <summary>
	/// Strips all white space (space and tab characters) from both the head 
	/// and tail of this std::string, returning a reference to self (not copy).
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
	//  Name : toUpper()
	/// <summary>
	/// Process the std::string, transforming all characters to upper case where
	/// appropriate. Returns a reference to self (not a copy).
	/// </summary>
	//-------------------------------------------------------------------------
	std::string toUpper(const std::string& str);

	//-------------------------------------------------------------------------
	//  Name : toLower()
	/// <summary>
	/// Process the std::string, transforming all characters to lower case where
	/// appropriate. Returns a reference to self (not a copy).
	/// </summary>
	//-------------------------------------------------------------------------
	std::string toLower(const std::string& str);

	//-------------------------------------------------------------------------
	//  Name : beginsWith ()
	/// <summary>
	/// Determine if this std::string begins with the std::string provided.
	/// </summary>
	//-------------------------------------------------------------------------
	bool beginsWith(const std::string& str, const std::string & value, bool ignoreCase = false);

	//-------------------------------------------------------------------------
	//  Name : endsWith ()
	/// <summary>
	/// Determine if this std::string ends with the std::string provided.
	/// </summary>
	//-------------------------------------------------------------------------
	bool endsWith(const std::string& str, const std::string & value, bool ignoreCase = false);

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
		return std::move(str);
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
	//  Name : wordWrap ()
	/// <summary>
	/// Wraps the std::string up to the maximum length and optionally inserts an
	/// aribitrary padding std::string at the beginning of each new line
	/// </summary>
	//-------------------------------------------------------------------------
	std::string wordWrap(const std::string & value, std::string::size_type maximumLength, const std::string & linePadding = "");

	//-----------------------------------------------------------------------------
	//  Name : commandLineArgs ()
	/// <summary>
	/// Creates a single string command line.
	/// </summary>
	//-----------------------------------------------------------------------------
	std::string commandLineArgs(int _argc, char* _argv[]);

	//-----------------------------------------------------------------------------
	//  Name : parseCommandLine ()
	/// <summary>
	/// Split single string command line into its component parts.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool parseCommandLine(const std::string & strCommandLine, std::vector<std::string> & ArgumentsOut);
}