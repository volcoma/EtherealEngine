#pragma once

#include <algorithm>
#include <cctype> // toupper / tolower
#include <cstdarg>
#include <cstdio>
#include <locale>
#include <random>
#include <string> // std::string / std::wstd::string
#include <vector>
namespace string_utils
{

//-------------------------------------------------------------------------
//  Name : compare ()
/// <summary>
/// Compare the two std::strings with optional case ignore.
/// </summary>
//-------------------------------------------------------------------------
auto compare(const std::string& s1, const std::string& s2, bool ignore_case) -> int;
//-------------------------------------------------------------------------
//  Name : trim()
/// <summary>
/// Strips all white space (space and tab characters) from both the head
/// and tail of this std::string.
/// </summary>
//-------------------------------------------------------------------------
auto trim(const std::string& str) -> std::string;

//-------------------------------------------------------------------------
//  Name : tokenize()
/// <summary>
/// Tokenize the string by the delimiters
/// </summary>
//-------------------------------------------------------------------------
auto tokenize(const std::string& str, const std::string& delimiters) -> std::vector<std::string>;
//-------------------------------------------------------------------------
//  Name : to_upper()
/// <summary>
/// Process the std::string, transforming all characters to upper case where
/// appropriate.
/// </summary>
//-------------------------------------------------------------------------
auto to_upper(const std::string& str) -> std::string;

//-------------------------------------------------------------------------
//  Name : to_lower()
/// <summary>
/// Process the std::string, transforming all characters to lower case where
/// appropriate.
/// </summary>
//-------------------------------------------------------------------------
auto to_lower(const std::string& str) -> std::string;

//-------------------------------------------------------------------------
//  Name : begins_with ()
/// <summary>
/// Determine if this std::string begins with the std::string provided.
/// </summary>
//-------------------------------------------------------------------------
auto begins_with(const std::string& str, const std::string& value, bool ignore_case = false) -> bool;

//-------------------------------------------------------------------------
//  Name : ends_with ()
/// <summary>
/// Determine if this std::string ends with the std::string provided.
/// </summary>
//-------------------------------------------------------------------------
auto ends_with(const std::string& str, const std::string& value, bool ignore_case = false) -> bool;

//-------------------------------------------------------------------------
//  Name : replace ()
/// <summary>
/// Replacing any occurences of the specified character sequence with
/// another.
/// </summary>
//-------------------------------------------------------------------------
auto replace(const std::string& str, const std::string& old_seq, const std::string& new_seq) -> std::string;

//-------------------------------------------------------------------------
//  Name : replace ()
/// <summary>
/// Replacing any occurances of the specified character with another.
/// </summary>
//-------------------------------------------------------------------------
auto replace(const std::string& str, std::string::value_type old_char, std::string::value_type new_char) -> std::string;

//-------------------------------------------------------------------------
//  Name : format ()
/// <summary>
/// Generate a newly formatted std::string based on a format descriptor and
/// an optional variable length list of arguments.
/// </summary>
//-------------------------------------------------------------------------
template<typename... Args>
auto format(const char* format, Args&&... args) -> std::string
{
    auto length = std::snprintf(nullptr, 0, format, std::forward<Args>(args)...);
    if(length == 0)
    {
        return {};
    }

    std::vector<char> buf(length + 1);
    length = std::snprintf(buf.data(), buf.size(), format, std::forward<Args>(args)...);
    std::string str(buf.data());

    return str;
}

//-------------------------------------------------------------------------
//  Name : format ()
/// <summary>
/// Generate a newly formatted std::string based on a format descriptor and
/// an optional variable bufferLength list of arguments.
/// </summary>
//-------------------------------------------------------------------------
auto format(const char* format, va_list args) -> std::string;

//-------------------------------------------------------------------------
//  Name : word_wrap ()
/// <summary>
/// Wraps the std::string up to the maximum length and optionally inserts an
/// aribitrary padding std::string at the beginning of each new line
/// </summary>
//-------------------------------------------------------------------------
auto word_wrap(const std::string& value, std::string::size_type max_length, const std::string& line_padding = "")
    -> std::string;

//-------------------------------------------------------------------------
//  Name : random_string ()
/// <summary>
/// Wraps the std::string up to the maximum length and optionally inserts an
/// aribitrary padding std::string at the beginning of each new line
/// </summary>
//-------------------------------------------------------------------------
auto random_string(std::string::size_type length) -> std::string;

//-----------------------------------------------------------------------------
//  Name : command_line_args ()
/// <summary>
/// Creates a single string command line.
/// </summary>
//-----------------------------------------------------------------------------
auto command_line_args(int _argc, char* _argv[]) -> std::string;

//-----------------------------------------------------------------------------
//  Name : parse_command_line ()
/// <summary>
/// Split single string command line into its component parts.
/// </summary>
//-----------------------------------------------------------------------------
auto parse_command_line(const std::string& cmd_line, std::vector<std::string>& args) -> bool;
} // namespace string_utils
