#include "string_utils.h"
#include <chrono>
#include <thread>

namespace string_utils
{
namespace
{
class StrICmp
{
public:
    StrICmp() = default;
    //---------------------------------------------------------------------
    // Constructors & Destructors
    //---------------------------------------------------------------------
    StrICmp(const char* lpszLang) : locale_(lpszLang)
    {
    }

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
        CharLessI(std::locale& Locale) : locale_(Locale)
        {
        }

        //-----------------------------------------------------------------
        // Public Operators
        //-----------------------------------------------------------------
        template<typename T>
        auto operator()(T c1, T c2) -> bool
        {
            return std::tolower(c1, locale_) < std::tolower(c2, locale_);

        } // End Operator

    private:
        //-----------------------------------------------------------------
        // Private Member Variables
        //-----------------------------------------------------------------
        std::locale& locale_;
    };

    //---------------------------------------------------------------------
    // Public Operators
    //---------------------------------------------------------------------
    auto operator()(const std::string& s1, const std::string& s2) -> int
    {
        if(std::lexicographical_compare(s1.begin(), s1.end(), s2.begin(), s2.end(), CharLessI(locale_)))
        {
            return -1;
        }
        if(std::lexicographical_compare(s2.begin(), s2.end(), s1.begin(), s1.end(), CharLessI(locale_)))
        {
            return 1;
        }
        return 0;

    } // End Operator

private:
    //---------------------------------------------------------------------
    // Private Member Variables
    //---------------------------------------------------------------------
    std::locale locale_;
};
} // namespace

auto compare(const std::string& s1, const std::string& s2, bool ignore_case) -> int
{
    if(!ignore_case)
    {
        return s1.compare(s2);
    }
    return StrICmp()(s1, s2);
}

auto trim(const std::string& str) -> std::string
{
    std::string s = str;
    std::string::size_type pos;

    // Early out
    if(s.empty())
    {
        return s;
    }

    // Trim Left
    pos = s.find_first_not_of(" \t");
    if(pos != std::string::npos)
    {
        s.erase(0, pos);
    }
    else
    {
        s.clear();
    }

    // Early out
    if(s.empty())
    {
        return s;
    }

    // Trim right
    pos = s.find_last_not_of(" \t");
    if(pos != std::string::npos)
    {
        s.erase(pos + 1);
    }
    else
    {
        s.clear();
    }

    return s;
}

auto tokenize(const std::string& str, const std::string& delimiters) -> std::vector<std::string>
{
    std::vector<std::string> tokens;
    // Skip delimiters at beginning.
    auto last_pos = str.find_first_not_of(delimiters, 0);
    // Find first "non-delimiter".
    auto pos = str.find_first_of(delimiters, last_pos);

    while(std::string::npos != pos || std::string::npos != last_pos)
    {
        // Found a token, add it to the vector.
        tokens.emplace_back(str.substr(last_pos, pos - last_pos));
        // Skip delimiters.  Note the "not_of"
        last_pos = str.find_first_not_of(delimiters, pos);
        // Find next "non-delimiter"
        pos = str.find_first_of(delimiters, last_pos);
    }

    return tokens;
}

auto to_upper(const std::string& str) -> std::string
{
    std::string s(str);
    std::transform(s.begin(),
                   s.end(),
                   s.begin(),
                   [](char in)
                   {
                       return char(toupper(in));
                   });
    return s;
}

auto to_lower(const std::string& str) -> std::string
{
    std::string s(str);
    std::transform(s.begin(),
                   s.end(),
                   s.begin(),
                   [](char in)
                   {
                       return char(tolower(in));
                   });
    return s;
}

auto begins_with(const std::string& str, const std::string& value, bool ignore_case /*= false*/) -> bool
{
    // Validate requirements
    if(str.length() < value.length())
    {
        return false;
    }
    if(str.empty() || value.empty())
    {
        return false;
    }

    // Do the subsets match?
    return (compare(std::string(str.substr(0, value.length())), value, ignore_case) == 0);
}

auto ends_with(const std::string& str, const std::string& value, bool ignore_case /*= false*/) -> bool
{
    // Validate requirements
    if(str.size() < value.size())
    {
        return false;
    }
    if(str.empty() || value.empty())
    {
        return false;
    }

    // Do the subsets match?
    return (compare(std::string(str.substr(str.length() - value.length())), value, ignore_case) == 0);
}

auto replace(const std::string& str, const std::string& old_seq, const std::string& new_seq) -> std::string
{
    std::string s = str;
    std::string::size_type location = 0;
    std::string::size_type old_length = old_seq.length();
    std::string::size_type new_length = new_seq.length();

    // Search for all replace std::string occurances.
    if(!s.empty())
    {
        while(std::string::npos != (location = s.find(old_seq, location)))
        {
            s.replace(location, old_length, new_seq);
            location += new_length;

            // Break out if we're done
            if(location >= s.length())
            {
                break;
            }

        } // Next

    } // End if not empty

    return s;
}

auto replace(const std::string& str, std::string::value_type old_char, std::string::value_type new_char) -> std::string
{
    std::string s = str;
    std::string::size_type location = 0;

    // Search for all replace std::string occurances.
    if(!s.empty())
    {
        while(std::string::npos != (location = s.find(old_char, location)))
        {
            s.replace(location, 1, 1, new_char);
            location += 1;

            // Break out if we're done
            if(location >= s.length())
            {
                break;
            }

        } // Next

    } // End if not empty

    return s;
}

auto format(const char* format, va_list args) -> std::string
{
    int length = std::vsnprintf(nullptr, 0, format, args);
    if(length == 0)
    {
        return std::string();
    }

    std::vector<char> buf(static_cast<size_t>(length) + 1);
    std::vsnprintf(buf.data(), static_cast<size_t>(length) + 1, format, args);

    std::string str(buf.data());
    return str;
}

auto word_wrap(const std::string& value, std::string::size_type max_length, const std::string& line_padding /*= ""*/)
    -> std::string
{
    std::string wrap_string, current_line;
    auto last_space = std::string::size_type(-1);
    auto line_length = std::string::size_type(0);

    // TODO: Add support for tab character to wrapping method.

    // Loop through each character in the std::string
    for(auto character : value)
    {
        switch(character)
        {
            case '\r':
                // Character type not supported
                break;

            case ' ':
                // A space was found, firstly does it exceed the max line length?
                if(line_length == max_length)
                {
                    // Simply wrap without inserting anything
                    wrap_string += current_line + "\n";
                    current_line = std::string();
                    line_length = 0;
                    last_space = std::string::size_type(-1);

                } // End if will exceed line length
                else
                {
                    // Add padding if we haven'value2 already
                    if(!wrap_string.empty() && line_length == 0)
                    {
                        current_line = line_padding;
                        line_length = current_line.length();

                    } // End if padding required

                    // Record it's position and insert the space
                    current_line += character;
                    last_space = line_length;
                    line_length++;

                } // End if no length exceed
                break;

            case '\n':
                // When we encounter a newline, just break automatically
                wrap_string += current_line + "\n";
                current_line = std::string();
                line_length = 0;
                last_space = std::string::size_type(-1);
                break;

            default:
                // Exceeding line length?
                if(line_length == max_length)
                {
                    // No space found on this line yet?
                    if(last_space == std::string::size_type(-1))
                    {
                        // Just break onto a new line
                        wrap_string += current_line + "\n";
                        current_line = std::string();
                        line_length = 0;
                        last_space = std::string::size_type(-1);

                    } // End if no space found
                    else
                    {
                        // Break onto a new-line where the space was found
                        if(last_space > 0)
                        {
                            wrap_string += current_line.substr(0, last_space) + "\n";
                        }
                        current_line = line_padding;
                        current_line.append(current_line.substr(last_space + 1));
                        line_length = current_line.length();
                        last_space = std::string::size_type(-1);

                    } // End if space found on line

                } // End if exceeding line length

                // Add padding if we haven'value2 already
                if(!wrap_string.empty() && line_length == 0)
                {
                    current_line = line_padding;
                    line_length = current_line.length();

                } // End if padding required

                // Add character
                current_line += character;
                line_length++;

        } // End character switch

    } // Next character

    // Add anything that's left to the wrap std::string
    wrap_string += current_line;

    // Return it
    return wrap_string;
}

auto random_string(std::string::size_type length) -> std::string
{
    using random_generator_t = ::std::mt19937;

    static const auto make_seeded_engine = []()
    {
        std::random_device r;
        std::hash<std::thread::id> hasher;
        std::seed_seq seed(std::initializer_list<typename random_generator_t::result_type>{
            static_cast<typename random_generator_t::result_type>(
                ::std::chrono::system_clock::now().time_since_epoch().count()),
            static_cast<typename random_generator_t::result_type>(hasher(std::this_thread::get_id())),
            r(),
            r(),
            r(),
            r(),
            r(),
            r(),
            r(),
            r()});
        return random_generator_t(seed);
    };

    random_generator_t engine(make_seeded_engine());

    auto randchar = [&]() -> char
    {
        constexpr const char charset[] = "0123456789"
                                         "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
                                         "abcdefghijklmnopqrstuvwxyz";

        const size_t max_index = (sizeof(charset) - 1);
        std::uniform_int_distribution<std::string::size_type> dist(0, max_index);

        return charset[dist(engine)];
    };

    std::string str(length, 0);
    std::generate_n(str.begin(), length, randchar);
    return str;
}

auto command_line_args(int _argc, char* _argv[]) -> std::string
{
    std::string cmd_line;
    for(int i = 1; i < _argc - 1; ++i)
    {
        cmd_line.append(_argv[i]).append(" ");
    }
    cmd_line.append(_argv[_argc - 1]);
    return cmd_line;
}

auto parse_command_line(const std::string& cmd_line, std::vector<std::string>& args) -> bool
{
    // Be polite and clear output array
    args.clear();

    // Parse one character at a time
    bool in_quotes = false;
    bool construct_argument = false;
    for(auto c : cmd_line)
    {
        if(in_quotes)
        {
            if(c == '\"')
            {
                // Quotes are now closed
                in_quotes = false;

            } // End if quote
            else
            {
                // Append to current argument.
                args.back() += c;

            } // End if !quote

        } // End if in quotes
        else
        {
            switch(c)
            {
                case '\"':
                    // Opened quotes
                    in_quotes = true;

                    // If we haven't started constructing an argument yet
                    // create space for one.
                    if(!construct_argument)
                    {
                        args.resize(args.size() + 1);
                    }
                    construct_argument = true;
                    break;

                case ' ':
                case '\t':
                case '\r':
                case '\n':
                    // White space. Finish constructing current argument.
                    construct_argument = false;
                    break;

                default:
                    // If we haven't started constructing an argument yet
                    // create space for one.
                    if(!construct_argument)
                    {
                        args.resize(args.size() + 1);
                    }
                    construct_argument = true;

                    // Append character to current argument.
                    args.back() += c;
                    break;

            } // End switch c

        } // End if !in quotes

    } // Next character

    // Success!
    return true;
}
} // namespace string_utils
