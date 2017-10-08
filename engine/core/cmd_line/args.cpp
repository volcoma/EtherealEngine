#include "args.h"
#include <regex>

namespace cmd_line
{
namespace
{
constexpr int OPTION_LONGEST = 30;
constexpr int OPTION_DESC_GAP = 2;

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

static std::basic_regex<char> integer_pattern("(-)?(0x)?([1-9a-zA-Z][0-9a-zA-Z]*)|(0)");

static std::basic_regex<char> option_matcher("--([[:alnum:]][-_[:alnum:]]+)(=(.*))?|-([[:alnum:]]+)");

static std::basic_regex<char> option_specifier("(([[:alnum:]]),)?[ ]*([[:alnum:]][-_[:alnum:]]*)?");

size_t string_length(const std::string& s)
{
	return s.length();
}

std::string& string_append(std::string& s, std::string a)
{
	return s.append(std::move(a));
}

std::string& string_append(std::string& s, size_t n, char c)
{
	return s.append(n, c);
}

option_adder& option_adder::operator()(const std::string& opts, const std::string& desc,
												 std::shared_ptr<const abstract_value> value,
												 std::string arg_help)
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

bool empty(const std::string& s)
{
	return s.empty();
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

bool values::match_impl(const std::string& text, unsigned long long umax,
						unsigned long long& result, bool& negative)
{
	std::smatch match;
	std::regex_match(text, match, integer_pattern);

	if(match.length() == 0)
	{
		throw argument_incorrect_type(text);
	}

	if(match.length(4) > 0)
	{
		return false;
	}

	negative = match.length(1) > 0;
	const auto base = match.length(2) > 0 ? 16 : 10;

	auto value_match = match[3];

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

	return true;
}


option_adder options_parser::add_options(std::string group)
{
	return option_adder(*this, std::move(group));
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

int options_parser::count(const std::string &o) const
{
    auto iter = m_options.find(o);
    if(iter == m_options.end())
    {
        return 0;
    }
    
    return iter->second->count();
}

const option_details &options_parser::operator[](const std::string &option) const
{
    auto iter = m_options.find(option);
    
    if(iter == m_options.end())
    {
        throw option_not_present_exception(option);
    }
    
    return *iter->second;
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

const help_group_details &options_parser::group_help(const std::string &group) const
{
    return m_help.at(group);
}

}
