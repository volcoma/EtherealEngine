#ifndef CONSOLE_H
#define CONSOLE_H

#include <cassert>

#include <memory>
#include <functional>
#include <string>
#include <sstream>
#include <vector>
#include <set>
#include <unordered_map>

class Console
{
public:
	explicit Console();
	virtual ~Console();

	// disallow copying
	Console(Console const&) = delete;
	Console& operator=(const Console&) = delete;

	/**
	* @brief Registers a new command with the given name and callback.
	*
	* The callback's signature determines what arguments the command
	* requires and may only consist of int, float and std::string arguments.
	*/
	template <typename... Args>
	void registerCommand(const std::string& name, const std::string& description, const std::vector<std::string>& argumentNames, const std::vector<std::string>& defaultArguments, const std::function<void(Args...)>& callback);
	void registerAlias(const std::string& alias, const std::string& command);

	std::string processInput(const std::string& line);
	std::vector<std::string> listOfCommands(const std::string& filter = "");

private:
	struct Command
	{
		const std::string name;
		std::string description;
		const unsigned int numArguments;
		const std::vector<std::string> argumentNames;
		const std::vector<std::string> defaultArguments;
		std::function<void(std::vector<std::string>&)> call;
		std::function<std::string(void)> getUsage;

		explicit Command(const std::string& name, const std::string& description, unsigned int numArguments, const std::vector<std::string>& argumentNames, const std::vector<std::string>& defaultArguments)
			: name(name)
			, description(description)
			, numArguments(numArguments)
			, argumentNames(argumentNames)
			, defaultArguments(defaultArguments)
		{
		}

		// disallow copying
		Command(Command const&) = delete;
		Command& operator=(const Command&) = delete;
	};

	std::unordered_map<std::string, std::shared_ptr<Command>> commands;
	std::set<std::string> names;
	void registerHelpCommand();
	void helpCommand(const std::string& term);

	template <typename T>
	struct argumentConverter
	{
		static inline T convert(const std::string& s);
	};

	static inline std::function<void()> bindCallback(std::function<void()> callback, const std::vector<std::string>& arguments, int argumentIndex);
	template <typename T, typename... Args>
	static std::function<void()> bindCallback(std::function<void(T, Args...)> callback, const std::vector<std::string>& arguments, int argumentIndex);

	template <typename... Args>
	struct NameArguments
	{
		static inline std::string get(const std::vector<std::string>& argumentNames, unsigned int nextName, unsigned int requiredArguments);
	};

	std::stringstream printBuffer;
	void print(std::string output) { printBuffer << output << "\n"; }

public:
	// TODO: make this private (or move to a StringUtils class)
	static std::vector<std::string> tokenizeLine(const std::string& line);

};

/***********************************
* Template function implementaions
***********************************
*/
template<typename T>
struct function_traits;     //renamed it!

template<typename R, typename ...Args>
struct function_traits<std::function<R(Args...)>>
{
	static const size_t nargs = sizeof...(Args);

	typedef R result_type;

	template <size_t i>
	struct arg
	{
		typedef typename std::tuple_element<i, std::tuple<Args...>>::type type;
	};
};

template <typename... Args>
void Console::registerCommand(const std::string& name, const std::string& description, const std::vector<std::string>& argumentNames, const std::vector<std::string>& defaultArguments, const std::function<void(Args...)>& callback)
{
	static const auto argCount = function_traits<std::function<void(Args...)>>::nargs;
	assert(argumentNames.size() <= argCount);
	assert(defaultArguments.size() <= argCount);
	assert(commands.find(name) == commands.end());
	assert(names.find(name) == names.end());

	auto command = std::make_shared<Command>(name, description, static_cast<unsigned int>(argCount), argumentNames, defaultArguments);
	auto commandRaw = command.get();

	command->call = [this, commandRaw, callback](std::vector<std::string>& arguments)
	{

		// add the arguments checks and set the default arguments
		auto requiredArguments = argCount - commandRaw->defaultArguments.size();

		// make sure the number of arguments matches
		if (arguments.size() < requiredArguments)
		{
			print("Too few arguments.");
		}
		else if (arguments.size() > argCount)
		{
			print("Too many arguments.");
		}
		else
		{
			// append default arguments as necessary
			arguments.insert(arguments.end(), commandRaw->defaultArguments.begin() + (arguments.size() - requiredArguments), commandRaw->defaultArguments.end());
			assert(arguments.size() == argCount);

			bool failed = false;
			std::function<void()> boundCallback;
			// bind the command callback recursively while allowing type conversion errors to raise exceptions
// 			try 
// 			{
			boundCallback = bindCallback(callback, arguments, 0);
			/*			}*/
			// 			catch (const std::exception&) 
			//			{
			// 				// TODO: remove this?
			// 				std::cout << "crashed" << std::endl;
			// 				failed = true;
			// 			}

			if (!failed)
			{
				// actually execute the command
				boundCallback();

				return;
			}
		}

		// if we end up here, something went wrong
		print(commandRaw->getUsage());
	};

	command->getUsage = [this, commandRaw]()
	{
		auto requiredArguments = sizeof...(Args)-commandRaw->defaultArguments.size();
		return "Usage: " + commandRaw->name + NameArguments<Args...>::get(commandRaw->argumentNames, 0, static_cast<unsigned int>(requiredArguments));
	};

	commands[name] = command;
	names.insert(name);
}

/**
* bindCallback, base case
*/
inline std::function<void()> Console::bindCallback(
	std::function<void()> callback,
	const std::vector<std::string>&,
	int)
{
	return callback;
}

/**
* bindCallback, recursion step.
*
* Run the "bind" code for each argument.
*/
template <typename T, typename... Args>
std::function<void()> Console::bindCallback(std::function<void(T, Args...)> callback, const std::vector<std::string>& arguments, int argumentIndex)
{
	T value = argumentConverter<T>::convert(arguments.at(argumentIndex));
	std::function<void(Args...)> nextCallback = [callback, value](Args... args)
	{
		callback(value, args...);
	};
	return bindCallback(nextCallback, arguments, argumentIndex + 1);
}

/**
* default argument converter: fail if none of the specific converters
* has been run
*/
template <typename T>
inline T Console::argumentConverter<T>::convert(const std::string& s)
{
	static_assert(sizeof(T) != sizeof(T), "Console commands may only take arguments of type int, float or std::string.");
}

/**
* convert arguments from string to int
*/
template <>
inline int Console::argumentConverter<int>::convert(const std::string& s)
{
	return std::stoi(s);
}

/**
* convert arguments from string to float
*/
template <>
inline float Console::argumentConverter<float>::convert(const std::string& s)
{
	return std::stof(s);
}

/**
*  (dummy)convert arguments from string to string
*/
template <>
inline std::string Console::argumentConverter<std::string>::convert(const std::string& s)
{
	return s;
}

/**
* Return the list of the arguments of a command pretty printed. Base case.
*/
template <>
struct Console::NameArguments<>
{
	static inline std::string get(
		const std::vector<std::string> argumentNames,
		unsigned int nextName,
		unsigned int requiredArguments)
	{
		(void)argumentNames; // silence the unused compile warnings
		if (nextName > requiredArguments)
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
struct Console::NameArguments<T, Args...>
{
	static inline std::string get(const std::vector<std::string>& argumentNames, unsigned int nextName, unsigned int requiredArguments)
	{
		std::string nameT;
		if (typeid(T) == typeid(int))
		{
			nameT = "<int";
		}
		else if (typeid(T) == typeid(float))
		{
			nameT = "<float";
		}
		else if (typeid(T) == typeid(std::string))
		{
			nameT = "<string";
		}
		else
		{
			nameT = "<???";
		}
		if (argumentNames.size() > nextName && !argumentNames[nextName].empty())
		{
			nameT += " " + argumentNames[nextName];
		}
		nameT += ">";

		if (nextName == requiredArguments)
		{
			nameT = "[" + nameT;
		}

		return " " + nameT + NameArguments<Args...>::get(argumentNames, nextName + 1, requiredArguments);
	}
};


#endif