#include "console.h"

#include <map>
#include <memory>
#include <string>

console::console()
{
    register_help_command();
}

console::~console()
{
    commands_.clear();
}

/**
 * @brief Add an alias to the command.
 */
void console::register_alias(const std::string& alias, const std::string& command)
{
    assert(commands_.find(command) != commands_.end());
    assert(commands_.find(alias) == commands_.end());
    assert(names_.find(alias) == names_.end()); // TODO: rename names?
    commands_[alias] = commands_[command];
    names_.insert(alias);
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
    print_buffer.str(std::string()); // clear()
    std::vector<std::string> tokens = tokenize_line(line);

    if(tokens.empty())
    {
        return "";
    }

    std::string identifier = tokens.at(0);

    tokens.erase(tokens.begin());
    auto it = commands_.find(identifier);
    if(it != commands_.end())
    {
        it->second->call(tokens);
    }
    else
    {
        // TODO: we might want a more flexible way to give feedback
        print("Unknown command \"" + identifier + "\".");
    }
    return print_buffer.str();
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
    for(const auto& c : line)
    {
        if(c == ' ' && !insideQuotes)
        {
            // keep spaces inside of quoted text
            if(currWord.empty())
            {
                // ignore leading spaces
                continue;
            }

            // finish off word
            out.push_back(currWord);
            currWord.clear();
        }
        else if(!escapingQuotes && c == '\\')
        {
            escapingQuotes = true;
        }
        else if(escapingQuotes)
        {
            if(c != '"' && c != '\\')
            {
                currWord += '\\';
            }
            currWord += c;
            escapingQuotes = false;
        }
        else if(c == '"')
        {
            // finish off word or start quoted text
            if(insideQuotes)
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
    if(!currWord.empty())
    {
        out.push_back(currWord);
    }
    return out;
}

void console::register_help_command()
{
    register_command("help",
                     "Prints information about using the console or a given command.",
                     {"term"},
                     {""},
                     std::function<void(std::string)>(
                         [this](std::string term)
                         {
                             help_command(term);
                         }));
}

/**
 * TODO:
 * - if the commands will ever be case insensitive, the filter should also be
 */
void console::help_command(const std::string& term)
{
    if(term.empty())
    {
        // TODO by Michael: print version number
        print("Welcome to the console of (this engine).");
        print("command syntax:          \"command_name parameter1 parameter2 ...\"");
        print("Type \"help commands [filter]\" to find a command.");
        print("Type \"help command_name\" to display detailed information.");
    }
    else if(term == "commands")
    {
        // TODO: implement the filter
        for(const auto& command : list_of_commands())
        {
            print(command);
            if(!commands_[command]->description.empty())
            {
                print("    " + commands_[command]->description);
            }
        }
    }
    else
    {
        if(commands_.find(term) != commands_.end())
        {
            print(commands_[term]->get_usage());
            if(!commands_[term]->description.empty())
            {
                print("    " + commands_[term]->description);
            }
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
    for(auto value : commands_)
    {
        if(filter.empty() || value.first.compare(0, filter.length(), filter) == 0)
        {
            list.push_back(value.first);
        }
    }
    return list;
}
