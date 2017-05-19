#include "console.h"

#include <memory>
#include <string>
#include <map>

console::console()
{
	register_help_command();
}

console::~console()
{
	commands.clear();
}

/**
* @brief Add an alias to the command.
*/
void console::register_alias(const std::string& alias, const std::string& command)
{
	assert(commands.find(command) != commands.end());
	assert(commands.find(alias) == commands.end());
	assert(names.find(alias) == names.end()); // TODO: rename names?
	commands[alias] = commands[command];
	names.insert(alias);
}

/**
* @brief process an input line possibly containing commands
*
* Tokenize the line and process the tokens.
*
* The first token is the command to be run, the other tokens are
* the arguments.
*
* If the second token is a question mark, output the command's
* description.
*
* TODO:
* - create an history of inputs
* - eventually make the commands lowercase / case insensitive
*/
std::string console::process_input(const std::string& line)
{
	printBuffer.str(std::string()); // clear()
	std::vector<std::string> tokens = tokenize_line(line);

	if (tokens.size() == 0)
		return "";

	std::string identifier = tokens.at(0);

	tokens.erase(tokens.begin());
	auto it = commands.find(identifier);
	if (it != commands.end())
	{
		it->second->call(tokens);
	}
	else
	{
		// TODO: we might want a more flexible way to give feedback
		print("Unknown command \"" + identifier + "\".");

	}
	return printBuffer.str();
}

/**
* @brief Separate a string by spaces into words
*
* TODO:
* - Find if there is a better way to tokenize a string.
*/
std::vector<std::string> console::tokenize_line(const std::string& line)
{
	std::vector<std::string> out;
	std::string currWord;
	bool insideQuotes = false;
	bool escapingQuotes = false;
	// TODO: and with auto?
	// TODO: we might want to use getwc() to correctly read unicode characters
	for (const unsigned char& c : line)
	{
		// ignore control characters
// 		if (std::iscntrl(c) != 0) 
// 		{
// 			// TODO: BOM might not be recognized on non-Windows platforms. We might want to
// 			// check for it.
// 			continue;
// 		}
		/*else */if (c == ' ' && !insideQuotes)
		{
			// keep spaces inside of quoted text
			if (currWord.empty())
			{
				// ignore leading spaces
				continue;
			}
			else
			{
				// finish off word
				out.push_back(currWord);
				currWord.clear();
			}
		}
		else if (!escapingQuotes && c == '\\')
		{
			escapingQuotes = true;
		}
		else if (escapingQuotes)
		{
			if (c != '"' && c != '\\')
			{
				currWord += '\\';
			}
			currWord += c;
			escapingQuotes = false;
		}
		else if (c == '"')
		{
			// finish off word or start quoted text
			if (insideQuotes)
			{
				out.push_back(currWord);
				currWord.clear();
				insideQuotes = false;
			}
			else
			{
				insideQuotes = true;
			}
		}
		else
		{
			currWord += c;
		}
	}
	// add the last word
	if (!currWord.empty())
		out.push_back(currWord);
	return out;
}

void console::register_help_command()
{
	register_command(
		"help",
		"Prints information about using the console or a given command.",
		{ "term" },
		{ "" },
		(std::function<void(std::string)>) ([this](std::string term) {help_command(term); })
	);
}

/**
* TODO:
* - if the commands will ever be case insensitive, the filter should also be
*/
void console::help_command(const std::string& term)
{
	if (term.empty())
	{
		// TODO by Michael: print version number
		print("Welcome to the console of (this engine).");
		print("command syntax:          \"command_name parameter1 parameter2 ...\"");
		print("Type \"help commands [filter]\" to find a command.");
		print("Type \"help command_name\" to display detailed information.");
	}
	else if (term == "commands")
	{
		// TODO: implement the filter
		for (const auto command : list_of_commands())
		{
			print(command);
			if (!commands[command]->description.empty())
				print("    " + commands[command]->description);
		}

	}
	else
	{
		if (commands.find(term) != commands.end())
		{
			print(commands[term]->get_usage());
			if (!commands[term]->description.empty())
				print("    " + commands[term]->description);
		}
		else
		{
			print("Unknown command or variable \"" + term + "\".");
		}
	}
}

/**
* TODO:
* - if the commands will ever be case insensitive, the filter should also be
*/
std::vector<std::string> console::list_of_commands(const std::string& filter)
{
	std::vector<std::string> list{};
	for (auto value : commands)
	{
		if (filter == "" || value.first.compare(0, filter.length(), filter) == 0)
		{
			list.push_back(value.first);
		}
	}
	return list;
}