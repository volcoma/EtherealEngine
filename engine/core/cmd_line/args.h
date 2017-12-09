#ifndef ARGS_HPP
#define ARGS_HPP

#include <cctype>
#include <cstring>
#include <exception>
#include <iostream>
#include <limits>
#include <map>
#include <memory>
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

size_t string_length(const std::string& s);

std::string& string_append(std::string& s, std::string a);

std::string& string_append(std::string& s, size_t n, char c);

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

bool empty(const std::string& s);
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

bool match_impl(const std::string& text, unsigned long long umax, unsigned long long& result, bool& negative);

template <typename T>
void integer_parser(const std::string& text, T& value)
{

	using US = typename std::make_unsigned<T>::type;

	constexpr auto umax = std::numeric_limits<US>::max();
	constexpr bool is_signed = std::numeric_limits<T>::is_signed;
	unsigned long long result = 0;
	bool negative = false;
	if(!match_impl(text, umax, result, negative))
	{
		value = 0;
		return;
	}

	detail::check_signed_range<T>(negative, result, text);

	if(negative)
	{
		value = checked_negate<T>(US(result), text, std::integral_constant<bool, is_signed>());
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

	void parse(int& argc, char**& argv);

	option_adder add_options(std::string group = "");

	void add_option(const std::string& group, const std::string& s, const std::string& l, std::string desc,
					std::shared_ptr<const abstract_value> value, std::string arg_help);

	int count(const std::string& o) const;

	const option_details& operator[](const std::string& option) const;

	// parse positional arguments into the given option
	void parse_positional(const std::string& option);

	void parse_positional(const std::vector<std::string>& options);

	std::string help(const std::vector<std::string>& groups = {""}) const;

	const std::vector<std::string> groups() const;

	const help_group_details& group_help(const std::string& group) const;

private:
	void add_one_option(const std::string& option, std::shared_ptr<option_details> details);

	bool consume_positional(const std::string& a);

	void add_to_option(const std::string& option, const std::string& arg);

	void parse_option(std::shared_ptr<option_details> value, const std::string& name,
					  const std::string& arg = "");

	void checked_parse_arg(int argc, char* argv[], int& current, std::shared_ptr<option_details> value,
						   const std::string& name);

	std::string help_one_group(const std::string& group) const;

	void generate_group_help(std::string& result, const std::vector<std::string>& groups) const;

	void generate_all_groups_help(std::string& result) const;

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

	option_adder& operator()(const std::string& opts, const std::string& desc,
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
}

#endif // ARGS_HPP
