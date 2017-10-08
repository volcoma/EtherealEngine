#ifndef ARGS_HPP
#define ARGS_HPP

#include <cctype>
#include <cstring>
#include <exception>
#include <iostream>
#include <map>
#include <memory>
#include <regex>
#include <sstream>
#include <string>
#include <unordered_set>
#include <vector>

#ifdef _MSC_VER
#ifndef __cpp_rtti
#define __cpp_rtti _CPPRTTI
#endif // !__cpp_rtti
#endif

#define CMD_LINE_HAS_RTTI __cpp_rtti

namespace cmd_line
{
template <typename T>
T to_local_string(T&& t)
{
	return t;
}

inline size_t string_length(const std::string& s)
{
	return s.length();
}

inline std::string& string_append(std::string& s, std::string a)
{
	return s.append(std::move(a));
}

inline std::string& string_append(std::string& s, size_t n, char c)
{
	return s.append(n, c);
}

template <typename Iterator>
std::string& string_append(std::string& s, Iterator begin, Iterator end)
{
	return s.append(begin, end);
}

template <typename T>
std::string to_utf8_string(T&& t)
{
	return std::forward<T>(t);
}

inline bool empty(const std::string& s)
{
	return s.empty();
}
}

namespace cmd_line
{
namespace
{
#ifdef _WIN32
const std::string LQUOTE("\'");
const std::string RQUOTE("\'");
#else
const std::string LQUOTE("‘");
const std::string RQUOTE("’");
#endif
}

class abstract_value : public std::enable_shared_from_this<abstract_value>
{
public:
	virtual ~abstract_value() = default;
	virtual void parse(const std::string& text) const = 0;

	virtual void parse() const = 0;

	virtual bool has_arg() const = 0;

	virtual bool has_default() const = 0;

	virtual bool is_container() const = 0;

	virtual bool has_implicit() const = 0;

	virtual std::string get_default_value() const = 0;

	virtual std::string get_implicit_value() const = 0;

	virtual std::shared_ptr<abstract_value> default_value(const std::string& value) = 0;

	virtual std::shared_ptr<abstract_value> implicit_value(const std::string& value) = 0;
};

class option_exception : public std::exception
{
public:
	option_exception(const std::string& message)
		: m_message(message)
	{
	}

	virtual const char* what() const noexcept
	{
		return m_message.c_str();
	}

private:
	std::string m_message;
};

class option_spec_exception : public option_exception
{
public:
	option_spec_exception(const std::string& message)
		: option_exception(message)
	{
	}
};

class option_parse_exception : public option_exception
{
public:
	option_parse_exception(const std::string& message)
		: option_exception(message)
	{
	}
};

class option_exists_error : public option_spec_exception
{
public:
	option_exists_error(const std::string& option)
		: option_spec_exception(u8"Option " + LQUOTE + option + RQUOTE + u8" already exists")
	{
	}
};

class invalid_option_format_error : public option_spec_exception
{
public:
	invalid_option_format_error(const std::string& format)
		: option_spec_exception(u8"Invalid option format " + LQUOTE + format + RQUOTE)
	{
	}
};

class option_not_exists_exception : public option_parse_exception
{
public:
	option_not_exists_exception(const std::string& option)
		: option_parse_exception(u8"Option " + LQUOTE + option + RQUOTE + u8" does not exist")
	{
	}
};

class missing_argument_exception : public option_parse_exception
{
public:
	missing_argument_exception(const std::string& option)
		: option_parse_exception(u8"Option " + LQUOTE + option + RQUOTE + u8" is missing an argument")
	{
	}
};

class option_requires_argument_exception : public option_parse_exception
{
public:
	option_requires_argument_exception(const std::string& option)
		: option_parse_exception(u8"Option " + LQUOTE + option + RQUOTE + u8" requires an argument")
	{
	}
};

class option_not_has_argument_exception : public option_parse_exception
{
public:
	option_not_has_argument_exception(const std::string& option, const std::string& arg)
		: option_parse_exception(u8"Option " + LQUOTE + option + RQUOTE +
								 u8" does not take an argument, but argument" + LQUOTE + arg + RQUOTE +
								 " given")
	{
	}
};

class option_not_present_exception : public option_parse_exception
{
public:
	option_not_present_exception(const std::string& option)
		: option_parse_exception(u8"Option " + LQUOTE + option + RQUOTE + u8" not present")
	{
	}
};

class argument_incorrect_type : public option_parse_exception
{
public:
	argument_incorrect_type(const std::string& arg)
		: option_parse_exception(u8"Argument " + LQUOTE + arg + RQUOTE + u8" failed to parse")
	{
	}
};

class option_required_exception : public option_parse_exception
{
public:
	option_required_exception(const std::string& option)
		: option_parse_exception(u8"Option " + LQUOTE + option + RQUOTE + u8" is required but not present")
	{
	}
};

namespace values
{
namespace
{
std::basic_regex<char> integer_pattern("(-)?(0x)?([1-9a-zA-Z][0-9a-zA-Z]*)|(0)");
}

namespace detail
{
template <typename T, bool B>
struct signed_check;

template <typename T>
struct signed_check<T, true>
{
	template <typename U>
	void operator()(bool negative, U u, const std::string& text)
	{
		if(negative)
		{
			if(u > static_cast<U>(-std::numeric_limits<T>::min()))
			{
				throw argument_incorrect_type(text);
			}
		}
		else
		{
			if(u > static_cast<U>(std::numeric_limits<T>::max()))
			{
				throw argument_incorrect_type(text);
			}
		}
	}
};

template <typename T>
struct signed_check<T, false>
{
	template <typename U>
	void operator()(bool, U, const std::string&)
	{
	}
};

template <typename T, typename U>
void check_signed_range(bool negative, U value, const std::string& text)
{
	signed_check<T, std::numeric_limits<T>::is_signed>()(negative, value, text);
}
}

template <typename R, typename T>
R checked_negate(T&& t, const std::string&, std::true_type)
{
	// if we got to here, then `t` is a positive number that fits into
	// `R`. So to avoid MSVC C4146, we first cast it to `R`.
	return -static_cast<R>(t);
}

template <typename R, typename T>
T checked_negate(T&&, const std::string& text, std::false_type)
{
	throw argument_incorrect_type(text);
}

template <typename T>
void integer_parser(const std::string& text, T& value)
{
	std::smatch match;
	std::regex_match(text, match, integer_pattern);

	if(match.length() == 0)
	{
		throw argument_incorrect_type(text);
	}

	if(match.length(4) > 0)
	{
		value = 0;
		return;
	}

	using US = typename std::make_unsigned<T>::type;

	constexpr auto umax = std::numeric_limits<US>::max();
	constexpr bool is_signed = std::numeric_limits<T>::is_signed;
	const bool negative = match.length(1) > 0;
	const auto base = match.length(2) > 0 ? 16 : 10;

	auto value_match = match[3];

	US result = 0;

	for(auto iter = value_match.first; iter != value_match.second; ++iter)
	{
		int digit = 0;

		if(*iter >= '0' && *iter <= '9')
		{
			digit = *iter - '0';
		}
		else if(*iter >= 'a' && *iter <= 'f')
		{
			digit = *iter - 'a' + 10;
		}
		else if(*iter >= 'A' && *iter <= 'F')
		{
			digit = *iter - 'A' + 10;
		}

		if(umax - digit < result * base)
		{
			throw argument_incorrect_type(text);
		}

		result = result * base + digit;
	}

	detail::check_signed_range<T>(negative, result, text);

	if(negative)
	{
		value = checked_negate<T>(result, text, std::integral_constant<bool, is_signed>());
		// if (!is_signed)
		//{
		//  throw argument_incorrect_type(text);
		//}
		// value = -result;
	}
	else
	{
		value = static_cast<T>(result);
	}
}

template <typename T>
void stringstream_parser(const std::string& text, T& value)
{
	std::stringstream in(text);
	in >> value;
	if(!in)
	{
		throw argument_incorrect_type(text);
	}
}

inline void parse_value(const std::string& text, uint8_t& value)
{
	integer_parser(text, value);
}

inline void parse_value(const std::string& text, int8_t& value)
{
	integer_parser(text, value);
}

inline void parse_value(const std::string& text, uint16_t& value)
{
	integer_parser(text, value);
}

inline void parse_value(const std::string& text, int16_t& value)
{
	integer_parser(text, value);
}

inline void parse_value(const std::string& text, uint32_t& value)
{
	integer_parser(text, value);
}

inline void parse_value(const std::string& text, int32_t& value)
{
	integer_parser(text, value);
}

inline void parse_value(const std::string& text, uint64_t& value)
{
	integer_parser(text, value);
}

inline void parse_value(const std::string& text, int64_t& value)
{
	integer_parser(text, value);
}

inline void parse_value(const std::string& /*text*/, bool& value)
{
	// TODO recognise on, off, yes, no, enable, disable
	// so that we can write --long=yes explicitly
	value = true;
}

inline void parse_value(const std::string& text, std::string& value)
{
	value = text;
}

// The fallback parser. It uses the stringstream parser to parse all types
// that have not been overloaded explicitly.  It has to be placed in the
// source code before all other more specialized templates.
template <typename T>
void parse_value(const std::string& text, T& value)
{
	stringstream_parser(text, value);
}

template <typename T>
void parse_value(const std::string& text, std::vector<T>& value)
{
	T v;
	parse_value(text, v);
	value.push_back(v);
}

template <typename T>
struct value_has_arg
{
	static constexpr bool value = true;
};

template <>
struct value_has_arg<bool>
{
	static constexpr bool value = false;
};

template <typename T>
struct type_is_container
{
	static constexpr bool value = false;
};

template <typename T>
struct type_is_container<std::vector<T>>
{
	static constexpr bool value = true;
};

template <typename T>
class standard_value : public abstract_value
{
public:
	standard_value()
		: m_result(std::make_shared<T>())
		, m_store(m_result.get())
	{
	}

	standard_value(T* t)
		: m_store(t)
	{
	}

	void parse(const std::string& text) const
	{
		parse_value(text, *m_store);
	}

	bool is_container() const
	{
		return type_is_container<T>::value;
	}

	void parse() const
	{
		parse_value(m_default_value, *m_store);
	}

	bool has_arg() const
	{
		return value_has_arg<T>::value;
	}

	bool has_default() const
	{
		return m_default;
	}

	bool has_implicit() const
	{
		return m_implicit;
	}

	virtual std::shared_ptr<abstract_value> default_value(const std::string& value)
	{
		m_default = true;
		m_default_value = value;
		return shared_from_this();
	}

	virtual std::shared_ptr<abstract_value> implicit_value(const std::string& value)
	{
		m_implicit = true;
		m_implicit_value = value;
		return shared_from_this();
	}

	std::string get_default_value() const
	{
		return m_default_value;
	}

	std::string get_implicit_value() const
	{
		return m_implicit_value;
	}

	const T& get() const
	{
		if(m_store == nullptr)
		{
			return *m_result;
		}
		else
		{
			return *m_store;
		}
	}

protected:
	std::shared_ptr<T> m_result;
	T* m_store;
	bool m_default = false;
	std::string m_default_value;
	bool m_implicit = false;
	std::string m_implicit_value;
};
}

template <typename T>
std::shared_ptr<abstract_value> value()
{
	return std::make_shared<values::standard_value<T>>();
}

template <typename T>
std::shared_ptr<abstract_value> value(T& t)
{
	return std::make_shared<values::standard_value<T>>(&t);
}

class option_adder;

class option_details
{
public:
	option_details(const std::string& desc, std::shared_ptr<const abstract_value> val)
		: m_desc(desc)
		, m_value(val)
		, m_count(0)
	{
	}

	const std::string& description() const
	{
		return m_desc;
	}

	bool has_arg() const
	{
		return m_value->has_arg();
	}

	void parse(const std::string& text)
	{
		m_value->parse(text);
		++m_count;
	}

	void parse_default()
	{
		m_value->parse();
	}

	int count() const
	{
		return m_count;
	}

	const abstract_value& value() const
	{
		return *m_value;
	}

	template <typename T>
	const T& as() const
	{
#ifdef CMD_LINE_HAS_RTTI
		return dynamic_cast<const values::standard_value<T>&>(*m_value).get();
#else
		return static_cast<const values::standard_value<T>&>(*m_value).get();
#endif
	}

private:
	std::string m_desc;
	std::shared_ptr<const abstract_value> m_value;
	int m_count;
};

struct help_option_details
{
	std::string s;
	std::string l;
	std::string desc;
	bool has_arg;
	bool has_default;
	std::string default_value;
	bool has_implicit;
	std::string implicit_value;
	std::string arg_help;
	bool is_container;
};

struct help_group_details
{
	std::string name;
	std::string description;
	std::vector<help_option_details> options;
};

class options_parser
{
public:
	options_parser(std::string program, std::string help_string = "")
		: m_program(std::move(program))
		, m_help_string(to_local_string(std::move(help_string)))
		, m_positional_help("positional parameters")
		, m_next_positional(m_positional.end())
	{
	}

	inline options_parser& positional_help(std::string help_text)
	{
		m_positional_help = std::move(help_text);
		return *this;
	}

	inline void parse(int& argc, char**& argv);

	inline option_adder add_options(std::string group = "");

	inline void add_option(const std::string& group, const std::string& s, const std::string& l,
						   std::string desc, std::shared_ptr<const abstract_value> value,
						   std::string arg_help);

	int count(const std::string& o) const
	{
		auto iter = m_options.find(o);
		if(iter == m_options.end())
		{
			return 0;
		}

		return iter->second->count();
	}

	const option_details& operator[](const std::string& option) const
	{
		auto iter = m_options.find(option);

		if(iter == m_options.end())
		{
			throw option_not_present_exception(option);
		}

		return *iter->second;
	}

	// parse positional arguments into the given option
	inline void parse_positional(std::string option);

	inline void parse_positional(std::vector<std::string> options);

	inline std::string help(const std::vector<std::string>& groups = {""}) const;

	inline const std::vector<std::string> groups() const;

	inline const help_group_details& group_help(const std::string& group) const;

private:
	inline void add_one_option(const std::string& option, std::shared_ptr<option_details> details);

	inline bool consume_positional(std::string a);

	inline void add_to_option(const std::string& option, const std::string& arg);

	inline void parse_option(std::shared_ptr<option_details> value, const std::string& name,
							 const std::string& arg = "");

	inline void checked_parse_arg(int argc, char* argv[], int& current, std::shared_ptr<option_details> value,
								  const std::string& name);

	inline std::string help_one_group(const std::string& group) const;

	inline void generate_group_help(std::string& result, const std::vector<std::string>& groups) const;

	inline void generate_all_groups_help(std::string& result) const;

	std::string m_program;
	std::string m_help_string;
	std::string m_positional_help;

	std::map<std::string, std::shared_ptr<option_details>> m_options;
	std::vector<std::string> m_positional;
	std::vector<std::string>::iterator m_next_positional;
	std::unordered_set<std::string> m_positional_set;

	// mapping from groups to help options
	std::map<std::string, help_group_details> m_help;
};

class option_adder
{
public:
	option_adder(options_parser& options, std::string group)
		: m_options(options)
		, m_group(std::move(group))
	{
	}

	inline option_adder& operator()(const std::string& opts, const std::string& desc,
									std::shared_ptr<const abstract_value> value = ::cmd_line::value<bool>(),
									std::string arg_help = "");

private:
	options_parser& m_options;
	std::string m_group;
};

// A helper function for setting required arguments
inline void check_required(const options_parser& options, const std::vector<std::string>& required)
{
	for(auto& r : required)
	{
		if(options.count(r) == 0)
		{
			throw option_required_exception(r);
		}
	}
}

namespace
{
constexpr int OPTION_LONGEST = 30;
constexpr int OPTION_DESC_GAP = 2;

std::basic_regex<char> option_matcher("--([[:alnum:]][-_[:alnum:]]+)(=(.*))?|-([[:alnum:]]+)");

std::basic_regex<char> option_specifier("(([[:alnum:]]),)?[ ]*([[:alnum:]][-_[:alnum:]]*)?");

std::string format_option(const help_option_details& o)
{
	auto& s = o.s;
	auto& l = o.l;

	std::string result = "  ";

	if(s.size() > 0)
	{
		result += "-" + to_local_string(s) + ",";
	}
	else
	{
		result += "   ";
	}

	if(l.size() > 0)
	{
		result += " --" + to_local_string(l);
	}

	if(o.has_arg)
	{
		auto arg = o.arg_help.size() > 0 ? to_local_string(o.arg_help) : "arg";

		if(o.has_implicit)
		{
			result += " [=" + arg + "(=" + to_local_string(o.implicit_value) + ")]";
		}
		else
		{
			result += " " + arg;
		}
	}

	return result;
}

std::string format_description(const help_option_details& o, size_t start, size_t width)
{
	auto desc = o.desc;

	if(o.has_default)
	{
		desc += to_local_string(" (default: " + o.default_value + ")");
	}

	std::string result;

	auto current = std::begin(desc);
	auto startLine = current;
	auto lastSpace = current;

	auto size = size_t{};

	while(current != std::end(desc))
	{
		if(*current == ' ')
		{
			lastSpace = current;
		}

		if(size > width)
		{
			if(lastSpace == startLine)
			{
				string_append(result, startLine, current + 1);
				string_append(result, "\n");
				string_append(result, start, ' ');
				startLine = current + 1;
				lastSpace = startLine;
			}
			else
			{
				string_append(result, startLine, lastSpace);
				string_append(result, "\n");
				string_append(result, start, ' ');
				startLine = lastSpace + 1;
			}
			size = 0;
		}
		else
		{
			++size;
		}

		++current;
	}

	// append whatever is left
	string_append(result, startLine, current);

	return result;
}
}

option_adder options_parser::add_options(std::string group)
{
	return option_adder(*this, std::move(group));
}

option_adder& option_adder::operator()(const std::string& opts, const std::string& desc,
									   std::shared_ptr<const abstract_value> value, std::string arg_help)
{
	std::match_results<const char*> result;
	std::regex_match(opts.c_str(), result, option_specifier);

	if(result.empty())
	{
		throw invalid_option_format_error(opts);
	}

	const auto& short_match = result[2];
	const auto& long_match = result[3];

	if(!short_match.length() && !long_match.length())
	{
		throw invalid_option_format_error(opts);
	}
	else if(long_match.length() == 1 && short_match.length())
	{
		throw invalid_option_format_error(opts);
	}

	auto option_names = [](const std::sub_match<const char*>& short_,
						   const std::sub_match<const char*>& long_) {
		if(long_.length() == 1)
		{
			return std::make_tuple(long_.str(), short_.str());
		}
		else
		{
			return std::make_tuple(short_.str(), long_.str());
		}
	}(short_match, long_match);

	m_options.add_option(m_group, std::get<0>(option_names), std::get<1>(option_names), desc, value,
						 std::move(arg_help));

	return *this;
}

void options_parser::parse_option(std::shared_ptr<option_details> value, const std::string& /*name*/,
								  const std::string& arg)
{
	value->parse(arg);
}

void options_parser::checked_parse_arg(int argc, char* argv[], int& current,
									   std::shared_ptr<option_details> value, const std::string& name)
{
	if(current + 1 >= argc)
	{
		if(value->value().has_implicit())
		{
			parse_option(value, name, value->value().get_implicit_value());
		}
		else
		{
			throw missing_argument_exception(name);
		}
	}
	else
	{
		if(argv[current + 1][0] == '-' && value->value().has_implicit())
		{
			parse_option(value, name, value->value().get_implicit_value());
		}
		else
		{
			parse_option(value, name, argv[current + 1]);
			++current;
		}
	}
}

void options_parser::add_to_option(const std::string& option, const std::string& arg)
{
	auto iter = m_options.find(option);

	if(iter == m_options.end())
	{
		throw option_not_exists_exception(option);
	}

	parse_option(iter->second, option, arg);
}

bool options_parser::consume_positional(std::string a)
{
	while(m_next_positional != m_positional.end())
	{
		auto iter = m_options.find(*m_next_positional);
		if(iter != m_options.end())
		{
			if(!iter->second->value().is_container())
			{
				if(iter->second->count() == 0)
				{
					add_to_option(*m_next_positional, a);
					++m_next_positional;
					return true;
				}
				else
				{
					++m_next_positional;
					continue;
				}
			}
			else
			{
				add_to_option(*m_next_positional, a);
				return true;
			}
		}
		++m_next_positional;
	}

	return false;
}

void options_parser::parse_positional(std::string option)
{
	parse_positional(std::vector<std::string>{option});
}

void options_parser::parse_positional(std::vector<std::string> options)
{
	m_positional = std::move(options);
	m_next_positional = m_positional.begin();

	m_positional_set.insert(m_positional.begin(), m_positional.end());
}

void options_parser::parse(int& argc, char**& argv)
{
	int current = 1;

	int nextKeep = 1;

	bool consume_remaining = false;

	while(current != argc)
	{
		if(strcmp(argv[current], "--") == 0)
		{
			consume_remaining = true;
			++current;
			break;
		}

		std::match_results<const char*> result;
		std::regex_match(argv[current], result, option_matcher);

		if(result.empty())
		{
			// not a flag

			// if true is returned here then it was consumed, otherwise it is
			// ignored
			if(consume_positional(argv[current]))
			{
			}
			else
			{
				argv[nextKeep] = argv[current];
				++nextKeep;
			}
			// if we return from here then it was parsed successfully, so continue
		}
		else
		{
			// short or long option?
			if(result[4].length() != 0)
			{
				const std::string& s = result[4];

				for(std::size_t i = 0; i != s.size(); ++i)
				{
					std::string name(1, s[i]);
					auto iter = m_options.find(name);

					if(iter == m_options.end())
					{
						throw option_not_exists_exception(name);
					}

					auto value = iter->second;

					// if no argument then just add it
					if(!value->has_arg())
					{
						parse_option(value, name);
					}
					else
					{
						// it must be the last argument
						if(i + 1 == s.size())
						{
							checked_parse_arg(argc, argv, current, value, name);
						}
						else if(value->value().has_implicit())
						{
							parse_option(value, name, value->value().get_implicit_value());
						}
						else
						{
							// error
							throw option_requires_argument_exception(name);
						}
					}
				}
			}
			else if(result[1].length() != 0)
			{
				const std::string& name = result[1];

				auto iter = m_options.find(name);

				if(iter == m_options.end())
				{
					throw option_not_exists_exception(name);
				}

				auto opt = iter->second;

				// equals provided for long option?
				if(result[3].length() != 0)
				{
					// parse the option given

					// but if it doesn't take an argument, this is an error
					if(!opt->has_arg())
					{
						throw option_not_has_argument_exception(name, result[3]);
					}

					parse_option(opt, name, result[3]);
				}
				else
				{
					if(opt->has_arg())
					{
						// parse the next argument
						checked_parse_arg(argc, argv, current, opt, name);
					}
					else
					{
						// parse with empty argument
						parse_option(opt, name);
					}
				}
			}
		}

		++current;
	}

	for(auto& opt : m_options)
	{
		auto& detail = opt.second;
		auto& value = detail->value();

		if(!detail->count() && value.has_default())
		{
			detail->parse_default();
		}
	}

	if(consume_remaining)
	{
		while(current < argc)
		{
			if(!consume_positional(argv[current]))
			{
				break;
			}
			++current;
		}

		// adjust argv for any that couldn't be swallowed
		while(current != argc)
		{
			argv[nextKeep] = argv[current];
			++nextKeep;
			++current;
		}
	}

	argc = nextKeep;
}

void options_parser::add_option(const std::string& group, const std::string& s, const std::string& l,
								std::string desc, std::shared_ptr<const abstract_value> value,
								std::string arg_help)
{
	auto stringDesc = to_local_string(std::move(desc));
	auto option = std::make_shared<option_details>(stringDesc, value);

	if(s.size() > 0)
	{
		add_one_option(s, option);
	}

	if(l.size() > 0)
	{
		add_one_option(l, option);
	}

	// add the help details
	auto& options = m_help[group];

	options.options.emplace_back(help_option_details{
		s, l, stringDesc, value->has_arg(), value->has_default(), value->get_default_value(),
		value->has_implicit(), value->get_implicit_value(), std::move(arg_help), value->is_container()});
}

void options_parser::add_one_option(const std::string& option, std::shared_ptr<option_details> details)
{
	auto in = m_options.emplace(option, details);

	if(!in.second)
	{
		throw option_exists_error(option);
	}
}

std::string options_parser::help_one_group(const std::string& g) const
{
	typedef std::vector<std::pair<std::string, std::string>> OptionHelp;

	auto group = m_help.find(g);
	if(group == m_help.end())
	{
		return "";
	}

	OptionHelp format;

	size_t longest = 0;

	std::string result;

	if(!g.empty())
	{
		result += to_local_string(" " + g + " options:\n");
	}

	for(const auto& o : group->second.options)
	{
		if(o.is_container && m_positional_set.find(o.l) != m_positional_set.end())
		{
			continue;
		}

		auto s = format_option(o);
		longest = std::max(longest, string_length(s));
		format.push_back(std::make_pair(s, std::string()));
	}

	longest = std::min(longest, static_cast<size_t>(OPTION_LONGEST));

	// widest allowed description
	auto allowed = size_t{76} - longest - OPTION_DESC_GAP;

	auto fiter = format.begin();
	for(const auto& o : group->second.options)
	{
		if(o.is_container && m_positional_set.find(o.l) != m_positional_set.end())
		{
			continue;
		}

		auto d = format_description(o, longest + OPTION_DESC_GAP, allowed);

		result += fiter->first;
		if(string_length(fiter->first) > longest)
		{
			result += '\n';
			result += to_local_string(std::string(longest + OPTION_DESC_GAP, ' '));
		}
		else
		{
			result +=
				to_local_string(std::string(longest + OPTION_DESC_GAP - string_length(fiter->first), ' '));
		}
		result += d;
		result += '\n';

		++fiter;
	}

	return result;
}

void options_parser::generate_group_help(std::string& result,
										 const std::vector<std::string>& print_groups) const
{
	for(size_t i = 0; i != print_groups.size(); ++i)
	{
		const std::string& group_help_text = help_one_group(print_groups[i]);
		if(empty(group_help_text))
		{
			continue;
		}
		result += group_help_text;
		if(i < print_groups.size() - 1)
		{
			result += '\n';
		}
	}
}

void options_parser::generate_all_groups_help(std::string& result) const
{
	std::vector<std::string> all_groups;
	all_groups.reserve(m_help.size());

	for(auto& group : m_help)
	{
		all_groups.push_back(group.first);
	}

	generate_group_help(result, all_groups);
}

std::string options_parser::help(const std::vector<std::string>& help_groups) const
{
	std::string result = m_help_string + "\nUsage:\n  " + to_local_string(m_program) + " [OPTION...]";

	if(m_positional.size() > 0)
	{
		result += " " + to_local_string(m_positional_help);
	}

	result += "\n\n";

	if(help_groups.size() == 0)
	{
		generate_all_groups_help(result);
	}
	else
	{
		generate_group_help(result, help_groups);
	}

	return to_utf8_string(result);
}

const std::vector<std::string> options_parser::groups() const
{
	std::vector<std::string> g;

	std::transform(
		m_help.begin(), m_help.end(), std::back_inserter(g),
		[](const std::map<std::string, help_group_details>::value_type& pair) { return pair.first; });

	return g;
}

const help_group_details& options_parser::group_help(const std::string& group) const
{
	return m_help.at(group);
}
}

#endif // ARGS_HPP
