#pragma once
#include <functional>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

namespace cmd_line
{
namespace detail
{
inline std::uint64_t get_internal_id()
{
	static std::uint64_t id = 0;
	return id++;
}
template <typename T>
inline std::uint64_t get_id()
{
	static std::uint64_t id = get_internal_id();
	return id;
}
}

/// Class used to wrap integer types to specify desired numerical base for specific argument parsing
template <typename T, int num_base = 0>
class numeric_base
{
public:
	/// This constructor required for correct AgrumentCountChecker initialization
	numeric_base()
		: value(0)
		, base(num_base)
	{
	}

	/// This constructor required for default value initialization
	/// \param val comes from default value
	numeric_base(T val)
		: value(val)
		, base(num_base)
	{
	}

	operator T() const
	{
		return this->value;
	}
	operator T*()
	{
		return this->value;
	}

	T value;
	unsigned int base;
};

struct callback_args
{
	const std::vector<std::string>& arguments;
	std::ostream& output;
	std::ostream& error;
};
class parser
{
private:
	class cmd_base
	{
	public:
		explicit cmd_base(const std::string& name, const std::string& alternative,
						  const std::string& description, bool required, bool dominant, bool is_variadic)
			: name(name)
			, command(!name.empty() ? "-" + name : "")
			, alternative(!alternative.empty() ? "--" + alternative : "")
			, description(description)
			, required(required)
			, handled(false)
			, arguments({})
			, dominant(dominant)
			, variadic(is_variadic)

		{
		}

		virtual ~cmd_base()
		{
		}

		std::string name;
		std::string command;
		std::string alternative;
		std::string description;
		bool required;
		bool handled;
		std::vector<std::string> arguments;
		bool const dominant;
		bool const variadic;

		virtual std::string print_value() const = 0;
		virtual bool parse(std::ostream& output, std::ostream& error) = 0;

		inline bool is(const std::string& given) const
		{
			return given == command || given == alternative;
		}
	};

	template <typename T>
	struct argument_count_checker
	{
		static constexpr bool variadic = false;
	};

	template <typename T>
	struct argument_count_checker<cmd_line::numeric_base<T>>
	{
		static constexpr bool variadic = false;
	};

	template <typename T>
	struct argument_count_checker<std::vector<T>>
	{
		static constexpr bool variadic = true;
	};

	template <typename T>
	class cmd_function final : public cmd_base
	{
	public:
		explicit cmd_function(const std::string& name, const std::string& alternative,
							  const std::string& description, bool required, bool dominant)
			: cmd_base(name, alternative, description, required, dominant,
					   argument_count_checker<T>::variadic)
		{
		}

		virtual bool parse(std::ostream& output, std::ostream& error)
		{
			try
			{
				callback_args args{arguments, output, error};
				value = callback(args);
				return true;
			}
			catch(...)
			{
				return false;
			}
		}

		virtual std::string print_value() const
		{
			return "";
		}

		std::function<T(callback_args&)> callback;
		T value;
	};

	template <typename T>
	class cmd_argument final : public cmd_base
	{
	public:
		explicit cmd_argument(const std::string& name, const std::string& alternative,
							  const std::string& description, bool required, bool dominant)
			: cmd_base(name, alternative, description, required, dominant,
					   argument_count_checker<T>::variadic)
		{
		}

		virtual bool parse(std::ostream& /*output*/, std::ostream& /*error*/)
		{
			try
			{
				value = parser::parse(arguments, value);
				return true;
			}
			catch(...)
			{
				return false;
			}
		}

		virtual std::string print_value() const
		{
			return stringify(value);
		}

		T value;
	};

	static int parse(const std::vector<std::string>& elements, const int& /*unused*/, int numberBase = 0)
	{
		if(elements.size() != 1)
		{
			throw std::bad_cast();
		}

		return std::stoi(elements[0], nullptr, numberBase);
	}

	static bool parse(const std::vector<std::string>& elements, const bool& defval)
	{
		if(!elements.empty())
		{
			throw std::runtime_error("A boolean command line parameter cannot have any arguments.");
		}

		return !defval;
	}

	static double parse(const std::vector<std::string>& elements, const double& /*unused*/)
	{
		if(elements.size() != 1)
		{
			throw std::bad_cast();
		}

		return std::stod(elements[0]);
	}

	static float parse(const std::vector<std::string>& elements, const float& /*unused*/)
	{
		if(elements.size() != 1)
		{
			throw std::bad_cast();
		}

		return std::stof(elements[0]);
	}

	static long double parse(const std::vector<std::string>& elements, const long double& /*unused*/)
	{
		if(elements.size() != 1)
		{
			throw std::bad_cast();
		}

		return std::stold(elements[0]);
	}

	static unsigned int parse(const std::vector<std::string>& elements, const unsigned int& /*unused*/,
							  int numberBase = 0)
	{
		if(elements.size() != 1)
		{
			throw std::bad_cast();
		}

		return static_cast<unsigned int>(std::stoul(elements[0], nullptr, numberBase));
	}

	static unsigned long parse(const std::vector<std::string>& elements, const unsigned long& /*unused*/,
							   int numberBase = 0)
	{
		if(elements.size() != 1)
		{
			throw std::bad_cast();
		}

		return std::stoul(elements[0], nullptr, numberBase);
	}

	static long parse(const std::vector<std::string>& elements, const long& /*unused*/)
	{
		if(elements.size() != 1)
		{
			throw std::bad_cast();
		}

		return std::stol(elements[0]);
	}

	static std::string parse(const std::vector<std::string>& elements, const std::string& /*unused*/)
	{
		if(elements.size() != 1)
		{
			throw std::bad_cast();
		}

		return elements[0];
	}

	template <class T>
	static std::vector<T> parse(const std::vector<std::string>& elements, const std::vector<T>& /*unused*/)
	{
		const T defval = T();
		std::vector<T> values{};
		std::vector<std::string> buffer(1);

		for(const auto& element : elements)
		{
			buffer[0] = element;
			values.push_back(parse(buffer, defval));
		}

		return values;
	}

	template <typename T>
	static T parse(const std::vector<std::string>& elements, const numeric_base<T>& wrapper)
	{
		return parse(elements, wrapper.value, 0);
	}

	/// Specialization for number wrapped into numerical base
	/// \tparam T base type of the argument
	/// \tparam base numerical base
	/// \param elements
	/// \param wrapper
	/// \return parsed number
	template <typename T, int base>
	static T parse(const std::vector<std::string>& elements, const numeric_base<T, base>& wrapper)
	{
		return parse(elements, wrapper.value, wrapper.base);
	}

	template <class T>
	static std::string stringify(const T& value)
	{
		return std::to_string(value);
	}

	template <class T, int base>
	static std::string stringify(const numeric_base<T, base>& wrapper)
	{
		return std::to_string(wrapper.value);
	}

	template <class T>
	static std::string stringify(const std::vector<T>& values)
	{
		std::stringstream ss{};
		ss << "[ ";

		for(const auto& value : values)
		{
			ss << stringify(value) << " ";
		}

		ss << "]";
		return ss.str();
	}

	static std::string stringify(const std::string& str)
	{
		return str;
	}

public:
	explicit parser(int argc, const char** argv)
		: _appname(argv[0])
	{
		for(int i = 1; i < argc; ++i)
		{
			_arguments.push_back(argv[i]);
		}
		enable_help();
	}

	explicit parser(int argc, char** argv)
		: _appname(argv[0])
	{
		for(int i = 1; i < argc; ++i)
		{
			_arguments.push_back(argv[i]);
		}
		enable_help();
	}

	bool has_help() const
	{
		for(const auto& command_pair : _commands)
		{
			const auto& command = command_pair.second;
			if(command->name == "h" && command->alternative == "--help")
			{
				return true;
			}
		}

		return false;
	}

	void enable_help()
	{
		set_callback("h", "help", std::function<bool(callback_args&)>([this](callback_args& args) {
						 args.output << this->usage();

						 return false;
					 }),
					 "", true);
	}

	void disable_help()
	{
		for(auto command = _commands.begin(); command != _commands.end(); ++command)
		{
			if((*command).second->name == "h" && (*command).second->alternative == "--help")
			{
				_commands.erase(command);
				break;
			}
		}
	}

	template <typename T>
	void set_default(bool is_required, const std::string& description = "")
	{
		auto command = std::make_unique<cmd_argument<T>>("", "", description, is_required, false);
		_commands.emplace_back(detail::get_id<cmd_argument<T>>(), std::move(command));
	}

	template <typename T>
	void set_required(const std::string& name, const std::string& alternative,
					  const std::string& description = "", bool dominant = false)
	{
		auto command = std::make_unique<cmd_argument<T>>(name, alternative, description, true, dominant);
		_commands.emplace_back(detail::get_id<cmd_argument<T>>(), std::move(command));
	}

	template <typename T>
	void set_optional(const std::string& name, const std::string& alternative, T defaultValue,
					  const std::string& description = "", bool dominant = false)
	{
		auto command = std::make_unique<cmd_argument<T>>(name, alternative, description, false, dominant);
		command->value = defaultValue;
		_commands.emplace_back(detail::get_id<cmd_argument<T>>(), std::move(command));
	}

	template <typename T>
	void set_callback(const std::string& name, const std::string& alternative,
					  std::function<T(callback_args&)> callback, const std::string& description = "",
					  bool dominant = false)
	{
		auto command = std::make_unique<cmd_function<T>>(name, alternative, description, false, dominant);
		command->callback = callback;
		_commands.emplace_back(detail::get_id<cmd_function<T>>(), std::move(command));
	}

	inline void run_and_exit_if_error()
	{
		if(!run())
		{
			exit(1);
		}
	}

	inline bool run()
	{
		return run(std::cout, std::cerr);
	}

	inline bool run(std::ostream& output)
	{
		return run(output, std::cerr);
	}

	bool run(std::ostream& output, std::ostream& error)
	{
		if(!_arguments.empty())
		{
			auto current = find_default();

			for(const auto& arg : _arguments)
			{
				auto isarg = !arg.empty() && arg[0] == '-';
				auto associated = isarg ? find(arg) : nullptr;

				if(associated != nullptr)
				{
					current = associated;
					associated->handled = true;
				}
				else if(current == nullptr)
				{
					error << no_default();
					return false;
				}
				else
				{
					current->arguments.push_back(arg);
					current->handled = true;
					if(!current->variadic)
					{
						// If the current command is not variadic, then no more arguments
						// should be added to it. In this case, switch back to the default
						// command.
						current = find_default();
					}
				}
			}
		}

		// First, parse dominant arguments since they succeed even if required
		// arguments are missing.
		for(auto& command_pair : _commands)
		{
			auto& command = command_pair.second;
			if(command->handled && command->dominant && !command->parse(output, error))
			{
				error << howto_use(command);
				return false;
			}
		}

		// Next, check for any missing arguments.
		for(const auto& command_pair : _commands)
		{
			const auto& command = command_pair.second;
			if(command->required && !command->handled)
			{
				error << howto_required(command);
				return false;
			}
		}

		// Finally, parse all remaining arguments.
		for(auto& command_pair : _commands)
		{
			auto& command = command_pair.second;
			if(command->handled && !command->dominant && !command->parse(output, error))
			{
				error << howto_use(command);
				return false;
			}
		}

		return true;
	}

	template <typename T>
	T get(const std::string& name) const
	{
		const std::string alternative_name = "--" + name;
		for(const auto& command_pair : _commands)
		{
			const auto command_type_id = command_pair.first;
			const auto& command = command_pair.second;
			if(command->name == name || command->alternative == alternative_name)
			{
				const auto requested_id = detail::get_id<cmd_argument<T>>();
				if(command_type_id != requested_id)
				{
					throw std::runtime_error("Invalid usage of the parameter " + name + " detected.");
				}
				else
				{
					auto cmd = static_cast<cmd_argument<T>*>(command.get());
					if(cmd)
					{
						return cmd->value;
					}
				}
			}
		}

		throw std::runtime_error("The parameter " + name + " could not be found.");
	}

	template <typename T>
	bool try_get(const std::string& name, T& result) const
	{
		try
		{
			result = get<T>(name);
			return true;
		}
		catch(const std::exception&)
		{
			return false;
		}
	}

	template <typename T>
	T get_if(const std::string& name, std::function<T(T)> callback) const
	{
		auto value = get<T>(name);
		return callback(value);
	}

	int requirements() const
	{
		int count = 0;

		for(const auto& command_pair : _commands)
		{
			const auto& command = command_pair.second;
			if(command->required)
			{
				++count;
			}
		}

		return count;
	}

	int commands() const
	{
		return static_cast<int>(_commands.size());
	}

	inline const std::string& app_name() const
	{
		return _appname;
	}

protected:
	cmd_base* find(const std::string& name)
	{
		for(auto& command_pair : _commands)
		{
			auto& command = command_pair.second;
			if(command->is(name))
			{
				return command.get();
			}
		}

		return nullptr;
	}

	cmd_base* find_default()
	{
		for(auto& command_pair : _commands)
		{
			auto& command = command_pair.second;
			if(command->name.empty())
			{
				return command.get();
			}
		}

		return nullptr;
	}

	std::string usage() const
	{
		std::stringstream ss{};
		ss << "Available parameters:\n\n";

		for(const auto& command_pair : _commands)
		{
			const auto& command = command_pair.second;

			ss << "  " << command->command << "\t" << command->alternative;

			if(command->required)
			{
				ss << "\t(required)";
			}

			ss << "\n   " << command->description;

			if(!command->required)
			{
				ss << "\n   "
				   << "This parameter is optional. The default value is '" + command->print_value() << "'.";
			}

			ss << "\n\n";
		}

		return ss.str();
	}

	void print_help(std::stringstream& ss) const
	{
		if(has_help())
		{
			ss << "For more help use --help or -h.\n";
		}
	}

	std::string howto_required(const std::unique_ptr<cmd_base>& command) const
	{
		std::stringstream ss{};
		ss << "The parameter " << command->name << " is required.\n";
		ss << command->description << '\n';
		print_help(ss);
		return ss.str();
	}

	std::string howto_use(const std::unique_ptr<cmd_base>& command) const
	{
		std::stringstream ss{};
		ss << "The parameter " << command->name << " has invalid arguments.\n";
		ss << command->description << '\n';
		print_help(ss);
		return ss.str();
	}

	std::string no_default() const
	{
		std::stringstream ss{};
		ss << "No default parameter has been specified.\n";
		ss << "The given argument must be used with a parameter.\n";
		print_help(ss);
		return ss.str();
	}

private:
	const std::string _appname;
	std::vector<std::string> _arguments;
	std::vector<std::pair<std::uint64_t, std::unique_ptr<cmd_base>>> _commands;
};
}
