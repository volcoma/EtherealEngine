#include "string_utils.h"
#include <chrono>
#include <thread>

int string_utils::compare(const std::string& s1, const std::string& s2, bool ignoreCase)
{
	if(!ignoreCase)
		return s1.compare(s2);
	else
		return StrICmp()(s1, s2);
}

std::string string_utils::trim(const std::string& str)
{
	std::string s = str;
	std::string::size_type pos;

	// Early out
	if(s.empty() == true)
		return s;

	// Trim Left
	pos = s.find_first_not_of(" \t");
	if(pos != std::string::npos)
		s.erase(0, pos);
	else
		s.clear();

	// Early out
	if(s.empty() == true)
		return s;

	// Trim right
	pos = s.find_last_not_of(" \t");
	if(pos != std::string::npos)
		s.erase(pos + 1);
	else
		s.clear();

	return s;
}

std::vector<std::string> string_utils::split(const std::string& text, char sep, bool skipEmpty)
{
	std::vector<std::string> tokens;
	std::size_t start = 0, end = 0;
	while((end = text.find(sep, start)) != std::string::npos)
	{
		std::string temp = text.substr(start, end - start);
		if(temp != "")
			tokens.push_back(temp);
		start = end + 1;
	}
	std::string temp = text.substr(start);
	if(skipEmpty)
	{
		if(temp != "")
		{
			tokens.push_back(temp);
		}
	}
	else
	{
		tokens.push_back(temp);
	}

	return tokens;
}

std::string string_utils::to_upper(const std::string& str)
{
	std::string s(str);
	std::transform(s.begin(), s.end(), s.begin(), toupper);
	return s;
}

std::string string_utils::to_lower(const std::string& str)
{
	std::string s(str);
	std::transform(s.begin(), s.end(), s.begin(), tolower);
	return s;
}

bool string_utils::begins_with(const std::string& str, const std::string& value, bool ignoreCase /*= false*/)
{
	// Validate requirements
	if(str.length() < value.length())
		return false;
	if(str.empty() == true || value.empty() == true)
		return false;

	// Do the subsets match?
	if(compare(std::string(str.substr(0, value.length())), value, ignoreCase) == 0)
		return true;

	// No match
	return false;
}

bool string_utils::ends_with(const std::string& str, const std::string& value, bool ignoreCase /*= false*/)
{
	// Validate requirements
	if(str.size() < value.size())
		return false;
	if(str.empty() == true || value.empty() == true)
		return false;

	// Do the subsets match?
	if(compare(std::string(str.substr(str.length() - value.length())), value, ignoreCase) == 0)
		return true;

	// No match
	return false;
}

std::string string_utils::replace(const std::string& str, const std::string& oldSequence,
								  const std::string& newSequence)
{
	std::string s = str;
	std::string::size_type location = 0;
	std::string::size_type oldLength = oldSequence.length();
	std::string::size_type newLength = newSequence.length();

	// Search for all replace std::string occurances.
	if(s.empty() == false)
	{
		while(std::string::npos != (location = s.find(oldSequence, location)))
		{
			s.replace(location, oldLength, newSequence);
			location += newLength;

			// Break out if we're done
			if(location >= s.length())
				break;

		} // Next

	} // End if not empty

	// Return self
	return s;
}

std::string string_utils::replace(const std::string& str, std::string::value_type oldCharacter,
								  std::string::value_type newCharacter)
{
	std::string s = str;
	std::string::size_type location = 0;

	// Search for all replace std::string occurances.
	if(s.empty() == false)
	{
		while(std::string::npos != (location = s.find(oldCharacter, location)))
		{
			s.replace(location, 1, 1, newCharacter);
			location += 1;

			// Break out if we're done
			if(location >= s.length())
				break;

		} // Next

	} // End if not empty

	// Return self
	return s;
}

std::string string_utils::format(const char* format, va_list args)
{
	int length = std::vsnprintf(nullptr, 0, format, args);
	if(length == 0)
		return std::string();

	char* buf = new char[length + 1];
	std::vsnprintf(buf, length + 1, format, args);

	std::string str(buf);
	delete[] buf;
	return str;
}

std::string string_utils::word_wrap(const std::string& value, std::string::size_type maximumLength,
									const std::string& linePadding /*= ""*/)
{
	std::string wrapString, currentLine;
	std::string::size_type lastSpace = std::string::size_type(-1), lineLength = std::string::size_type(0);

	// TODO: Add support for tab character to wrapping method.

	// Loop through each character in the std::string
	for(std::string::size_type i = 0, length = value.length(); i < length; ++i)
	{
		char character = value[i];
		switch(character)
		{
			case '\r':
				// Character type not supported
				break;

			case ' ':
				// A space was found, firstly does it exceed the max line length?
				if(lineLength == maximumLength)
				{
					// Simply wrap without inserting anything
					wrapString += currentLine + "\n";
					currentLine = std::string();
					lineLength = 0;
					lastSpace = -1;

				} // End if will exceed line length
				else
				{
					// Add padding if we haven'value2 already
					if(wrapString.empty() == false && lineLength == 0)
					{
						currentLine = linePadding;
						lineLength = (int)currentLine.length();

					} // End if padding required

					// Record it's position and insert the space
					currentLine += character;
					lastSpace = lineLength;
					lineLength++;

				} // End if no length exceed
				break;

			case '\n':
				// When we encounter a newline, just break automatically
				wrapString += currentLine + "\n";
				currentLine = std::string();
				lineLength = 0;
				lastSpace = -1;
				break;

			default:
				// Exceeding line length?
				if(lineLength == maximumLength)
				{
					// No space found on this line yet?
					if(lastSpace == -1)
					{
						// Just break onto a new line
						wrapString += currentLine + "\n";
						currentLine = std::string();
						lineLength = 0;
						lastSpace = -1;

					} // End if no space found
					else
					{
						// Break onto a new-line where the space was found
						if(lastSpace > 0)
							wrapString += currentLine.substr(0, lastSpace) + "\n";
						currentLine = linePadding + currentLine.substr(lastSpace + 1);
						lineLength = currentLine.length();
						lastSpace = -1;

					} // End if space found on line

				} // End if exceeding line length

				// Add padding if we haven'value2 already
				if(wrapString.empty() == false && lineLength == 0)
				{
					currentLine = linePadding;
					lineLength = currentLine.length();

				} // End if padding required

				// Add character
				currentLine += character;
				lineLength++;

		} // End character switch

	} // Next character

	// Add anything that's left to the wrap std::string
	wrapString += currentLine;

	// Return it
	return wrapString;
}

std::string string_utils::random_string(std::string::size_type length)
{
	using random_generator_t = ::std::mt19937;

	static const auto make_seeded_engine = []() {
		std::random_device r;
		std::hash<std::thread::id> hasher;
		std::seed_seq seed(std::initializer_list<typename random_generator_t::result_type>{
			static_cast<typename random_generator_t::result_type>(
				::std::chrono::system_clock::now().time_since_epoch().count()),
			static_cast<typename random_generator_t::result_type>(hasher(std::this_thread::get_id())), r(),
			r(), r(), r(), r(), r(), r(), r()});
		return random_generator_t(seed);
	};

    random_generator_t engine(make_seeded_engine());
    
	auto randchar = [&]() -> char {
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

std::string string_utils::command_line_args(int _argc, char* _argv[])
{
	std::string cmdLine = "";
	for(int i = 1; i < _argc - 1; ++i)
		cmdLine.append(_argv[i]).append(" ");
	cmdLine.append(_argv[_argc - 1]);
	return cmdLine;
}

bool string_utils::parse_command_line(const std::string& strCommandLine,
									  std::vector<std::string>& ArgumentsOut)
{
	// Be polite and clear output array
	ArgumentsOut.clear();

	// Parse one character at a time
	bool bInQuotes = false;
	bool bConstructingArgument = false;
	for(size_t i = 0; i < strCommandLine.size(); ++i)
	{
		char c = strCommandLine.at(i);
		if(bInQuotes)
		{
			if(c == '\"')
			{
				// Quotes are now closed
				bInQuotes = false;

			} // End if quote
			else
			{
				// Append to current argument.
				ArgumentsOut.back() += c;

			} // End if !quote

		} // End if in quotes
		else
		{
			switch(c)
			{
				case '\"':
					// Opened quotes
					bInQuotes = true;

					// If we haven't started constructing an argument yet
					// create space for one.
					if(!bConstructingArgument)
						ArgumentsOut.resize(ArgumentsOut.size() + 1);
					bConstructingArgument = true;
					break;

				case ' ':
				case '\t':
				case '\r':
				case '\n':
					// White space. Finish constructing current argument.
					bConstructingArgument = false;
					break;

				default:
					// If we haven't started constructing an argument yet
					// create space for one.
					if(!bConstructingArgument)
						ArgumentsOut.resize(ArgumentsOut.size() + 1);
					bConstructingArgument = true;

					// Append character to current argument.
					ArgumentsOut.back() += c;
					break;

			} // End switch c

		} // End if !in quotes

	} // Next character

	// Success!
	return true;
}
