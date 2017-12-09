#ifndef CONSOLE_H
#define CONSOLE_H

#include <cassert>

#include "../common/nonstd/type_traits.hpp"
#include <functional>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

class console
{
public:
	explicit console();
	virtual ~console();

	// disallow copying
	console(console const&) = delete;
	console& operator=(const console&) = delete;

	/**
	* @brief Registers a new command with the given name and callback.
	*
	* The callback's signature determines what arguments the command
	* requires and may only consist of int, float and std::string arguments.
	*/
	template <typename... Args>
	void register_command(const std::string& name, const std::string& description,
						  const std::vector<std::string>& argumentNames,
						  const std::vector<std::string>& defaultArguments,
						  const std::function<void(Args...)>& callback);
	void register_alias(const std::string& alias, const std::string& command);

	std::string process_input(const std::string& line);
	std::vector<std::string> list_of_commands(const std::string& filter = "");

private:
	struct command
	{
		const std::string name;
		std::string description;
		const unsigned int num_arguments;
		const std::vector<std::string> argument_names;
		const std::vector<std::string> default_arguments;
		std::function<void(std::vector<std::string>&)> call;
		std::function<std::string(void)> get_usage;

		explicit command(const std::string& name, const std::string& description, unsigned int numArguments,
						 const std::vector<std::string>& argumentNames,
						 const std::vector<std::string>& defaultArguments)
			: name(name)
			, description(description)
			, num_arguments(numArguments)
			, argument_names(argumentNames)
			, default_arguments(defaultArguments)
		{
		}

		// disallow copying
		command(command const&) = delete;
		command& operator=(const command&) = delete;
	};

	std::unordered_map<std::string, std::shared_ptr<command>> commands;
	std::set<std::string> names;
	void register_help_command();
	void help_command(const std::string& term);

	template <typename T>
	struct argumentConverter
	{
		static inline T convert(const std::string& s);
	};

	static inline std::function<void()> bind_callback(std::function<void()> callback,
													  const std::vector<std::string>& arguments,
													  int argumentIndex);
	template <typename T, typename... Args>
	static std::function<void()> bind_callback(std::function<void(T, Args...)> callback,
											   const std::vector<std::string>& arguments, int argumentIndex);

	template <typename... Args>
	struct NameArguments
	{
		static inline std::string get(const std::vector<std::string>& argumentNames, unsigned int nextName,
									  unsigned int requiredArguments);
	};

	std::stringstream print_buffer;
	void print(const std::string& output)
	{
		print_buffer << output << "\n";
	}

private:
	static std::vector<std::string> tokenize_line(const std::string& line);
};

template <typename... Args>
void console::register_command(const std::string& name, const std::string& description,
							   const std::vector<std::string>& argumentNames,
							   const std::vector<std::string>& defaultArguments,
							   const std::function<void(Args...)>& callback)
{
	const std::size_t argCount = sizeof...(Args);
	assert(argumentNames.size() <= argCount);
	assert(defaultArguments.size() <= argCount);
	assert(commands.find(name) == commands.end());
	assert(names.find(name) == names.end());

	auto cmd = std::make_shared<command>(name, description, static_cast<unsigned int>(argCount),
										 argumentNames, defaultArguments);
	auto commandRaw = cmd.get();

	cmd->call = [this, commandRaw, callback, argCount](std::vector<std::string>& arguments) {

		// add the arguments checks and set the default arguments
		auto requiredArguments = argCount - commandRaw->default_arguments.size();

		// make sure the number of arguments matches
		if(arguments.size() < requiredArguments)
		{
			print("Too few arguments.");
		}
		else if(arguments.size() > argCount)
		{
			print("Too many arguments.");
		}
		else
		{
			// append default arguments as necessary
			const auto offset = int(arguments.size() - requiredArguments);
			arguments.insert(arguments.end(), commandRaw->default_arguments.begin() + offset,
							 commandRaw->default_arguments.end());
			assert(arguments.size() == argCount);

			bool failed = false;
			std::function<void()> boundCallback = bind_callback(callback, arguments, 0);

			if(!failed)
			{
				// actually execute the command
				boundCallback();

				return;
			}
		}

		// if we end up here, something went wrong
		print(commandRaw->get_usage());
	};

	cmd->get_usage = [this, commandRaw]() {
		auto requiredArguments = sizeof...(Args) - commandRaw->default_arguments.size();
		return "Usage: " + commandRaw->name +
			   NameArguments<Args...>::get(commandRaw->argument_names, 0,
										   static_cast<unsigned int>(requiredArguments));
	};

	commands[name] = cmd;
	names.insert(name);
}

/**
* bindCallback, base case
*/
inline std::function<void()> console::bind_callback(std::function<void()> callback,
													const std::vector<std::string>&, int)
{
	return callback;
}

/**
* bindCallback, recursion step.
*
* Run the "bind" code for each argument.
*/
template <typename T, typename... Args>
std::function<void()> console::bind_callback(std::function<void(T, Args...)> callback,
											 const std::vector<std::string>& arguments, int argumentIndex)
{
	T value = argumentConverter<T>::convert(arguments.at(argumentIndex));
	std::function<void(Args...)> nextCallback = [callback, value](Args... args) { callback(value, args...); };
	return bind_callback(nextCallback, arguments, argumentIndex + 1);
}

/**
* default argument converter: fail if none of the specific converters
* has been run
*/
//template <typename T>
//inline T console::argumentConverter<T>::convert(const std::string&)
//{
//	static_assert(false, "console commands may only take "
//										  "arguments of type int, float or "
//										  "std::string.");
//}

/**
* convert arguments from string to int
*/
template <>
inline int console::argumentConverter<int>::convert(const std::string& s)
{
	return std::stoi(s);
}

/**
* convert arguments from string to float
*/
template <>
inline float console::argumentConverter<float>::convert(const std::string& s)
{
	return std::stof(s);
}

/**
*  (dummy)convert arguments from string to string
*/
template <>
inline std::string console::argumentConverter<std::string>::convert(const std::string& s)
{
	return s;
}

/**
* Return the list of the arguments of a command pretty printed. Base case.
*/
template <>
struct console::NameArguments<>
{
	static inline std::string get(const std::vector<std::string> argumentNames, unsigned int nextName,
								  unsigned int requiredArguments)
	{
		(void)argumentNames; // silence the unused compile warnings
		if(nextName > requiredArguments)
		{
			return "]";
		}
		else
		{
			return "";
		}
	}
};

/**
* Return the list of the arguments of a command pretty printed. Recursion step.
*/
template <typename T, typename... Args>
struct console::NameArguments<T, Args...>
{
	static inline std::string get(const std::vector<std::string>& argumentNames, unsigned int nextName,
								  unsigned int requiredArguments)
	{
		std::string nameT;
		if(rtti::type_id<T>() == rtti::type_id<int>())
		{
			nameT = "<int";
		}
		else if(rtti::type_id<T>() == rtti::type_id<float>())
		{
			nameT = "<float";
		}
		else if(rtti::type_id<T>() == rtti::type_id<std::string>())
		{
			nameT = "<string";
		}
		else
		{
			nameT = "<???";
		}
		if(argumentNames.size() > nextName && !argumentNames[nextName].empty())
		{
			nameT += " " + argumentNames[nextName];
		}
		nameT += ">";

		if(nextName == requiredArguments)
		{
			nameT = "[" + nameT;
		}

		return " " + nameT + NameArguments<Args...>::get(argumentNames, nextName + 1, requiredArguments);
	}
};

#endif
