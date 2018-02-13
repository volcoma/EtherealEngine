#include "parser.h"
#include "config.h"

#include <sstream>
#include <stdexcept>

namespace cfg
{

parser::parser(config& c)
	: conf_(c)
	, in_(nullptr)
	, line_(0)
	, next_token_(NO_TOKEN)
{
}

parser::~parser()
{
}

void parser::parse(std::istream& i, const std::string& f, unsigned int l)
{
	in_ = &i;
	file_ = f;
	line_ = l;

	// prep the look ahead
	std::string dummy;
	get_next_token(dummy);

	while(next_token_ != FILE_END)
	{
		if(next_token_ == NEWLINE)
		{
			// skip empty lines
			get_next_token(dummy);
		}
		else
		{
			parse_section();
		}
	}
}

parser::Token parser::get_next_token(std::string& value)
{
	Token token = next_token_;
	value = next_value_;

	next_token_ = lex_token(next_value_);
	while(next_token_ == WHITESPACE || next_token_ == COMMENT)
	{
		next_token_ = lex_token(next_value_);
	}

	return token;
}

parser::Token parser::lex_token(std::string& value)
{
	value.clear();
	int c = in_->get();
	switch(c)
	{
		case ' ':
		case '\t':
		case '\v':
			value.push_back(std::string::value_type(c));
			return lex_whitespace(value);
		case '\n':
		case '\r':
			value.push_back(std::string::value_type(c));
			return lex_newline(value);
		case '0':
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
		case '6':
		case '7':
		case '8':
		case '9':
		case '-':
		case '+':
			value.push_back(std::string::value_type(c));
			return lex_number(value);
		case 'a':
		case 'b':
		case 'c':
		case 'd':
		case 'e':
		case 'f':
		case 'g':
		case 'h':
		case 'i':
		case 'j':
		case 'k':
		case 'l':
		case 'm':
		case 'n':
		case 'o':
		case 'p':
		case 'q':
		case 'r':
		case 's':
		case 't':
		case 'u':
		case 'v':
		case 'w':
		case 'x':
		case 'y':
		case 'z':
		case 'A':
		case 'B':
		case 'C':
		case 'D':
		case 'E':
		case 'F':
		case 'G':
		case 'H':
		case 'I':
		case 'J':
		case 'K':
		case 'L':
		case 'M':
		case 'N':
		case 'O':
		case 'P':
		case 'Q':
		case 'R':
		case 'S':
		case 'T':
		case 'U':
		case 'V':
		case 'W':
		case 'X':
		case 'Y':
		case 'Z':
		case '_':
			value.push_back(std::string::value_type(c));
			return lex_identifier(value);
		case '"':
			return lex_string(value);
		case '[':
			value = "[";
			return OPEN_BRACE;
		case ']':
			value = "]";
			return CLOSE_BRACE;
		case '=':
			value = "=";
			return EQUALS;
		case '#':
			return lex_comment(value);
		case EOF:
			return FILE_END;
		default:
			value.push_back(std::string::value_type(c));
			error("Unexpected " + value + ".");
			return ERROR;
	}
}

parser::Token parser::lex_whitespace(std::string& value)
{
	int c = in_->get();
	while(true)
	{
		switch(c)
		{
			case ' ':
			case '\t':
			case '\v':
				value.push_back(std::string::value_type(c));
				break;
			default:
				in_->unget();
				return WHITESPACE;
		}
		c = in_->get();
	}
}

parser::Token parser::lex_newline(std::string& value)
{
	int c = in_->get();
	line_++;
	switch(c)
	{
		case '\n':
		case '\r':
			if(c != value[0])
			{
				// \r\n or \n\r
				value.push_back(c);
			}
			else
			{
				// treat \n \n as two newline, obviously
				in_->unget();
			}
			return NEWLINE;
		default:
			in_->unget();
			return NEWLINE;
	}
}

parser::Token parser::lex_number(std::string& value)
{
	int c = in_->get();
	while(true)
	{
		// NOTE: not validating the actual format
		switch(c)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case '.':
				value.push_back(std::string::value_type(c));
				break;
			default:
				in_->unget();
				return NUMBER;
		}
		c = in_->get();
	}
}

parser::Token parser::lex_identifier(std::string& value)
{
	int c = in_->get();
	while(true)
	{
		switch(c)
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
			case 'g':
			case 'h':
			case 'i':
			case 'j':
			case 'k':
			case 'l':
			case 'm':
			case 'n':
			case 'o':
			case 'p':
			case 'q':
			case 'r':
			case 's':
			case 't':
			case 'u':
			case 'v':
			case 'w':
			case 'x':
			case 'y':
			case 'z':
			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
			case 'G':
			case 'H':
			case 'I':
			case 'J':
			case 'K':
			case 'L':
			case 'M':
			case 'N':
			case 'O':
			case 'P':
			case 'Q':
			case 'R':
			case 'S':
			case 'T':
			case 'U':
			case 'V':
			case 'W':
			case 'X':
			case 'Y':
			case 'Z':
			case '_':
			case '-':
				value.push_back(std::string::value_type(c));
				break;
			default:
				in_->unget();
				return IDENTIFIER;
		}
		c = in_->get();
	}
}

parser::Token parser::lex_string(std::string& value)
{
	int c = in_->get();
	while(true)
	{
		switch(c)
		{
			case '"':
				return STRING;
			case '\n':
			case '\r':
			case EOF:
				error("Unexpected newline in string.");
				return ERROR;
			case '\\':
				c = in_->get();
				switch(c)
				{
					case '\'':
						value.push_back('\'');
						break;
					case '"':
						value.push_back('"');
						break;
					case '\\':
						value.push_back('\\');
						break;
					case 'a':
						value.push_back('\a');
						break;
					case 'b':
						value.push_back('\b');
						break;
					case 'f':
						value.push_back('\f');
						break;
					case 'n':
						value.push_back('\n');
						break;
					case 'r':
						value.push_back('\r');
						break;
					case 't':
						value.push_back('\t');
						break;
					case 'v':
						value.push_back('\v');
						break;
					default:
						error("Unknown escape sequence.");
						break;
				}
				break;
			default:
				value.push_back(std::string::value_type(c));
				break;
		}
		c = in_->get();
	}
}

parser::Token parser::lex_comment(std::string& value)
{
	int c = in_->get();
	while(true)
	{
		switch(c)
		{
			case '\n':
			case '\r':
			case EOF:
				return COMMENT;
			default:
				value.push_back(std::string::value_type(c));
				break;
		}
		c = in_->get();
	}
}

void parser::parse_section()
{
	parse_section_header();

	while(next_token_ == IDENTIFIER || next_token_ == NEWLINE)
	{
		parse_value_pair();
	}
}

void parser::parse_section_header()
{
	std::string value;
	Token t = get_next_token(value);

	if(t != OPEN_BRACE)
	{
		error("Expected open breace.");
	}

	t = get_next_token(value);
	if(t != IDENTIFIER)
	{
		error("Expected identifier.");
	}
	section_ = value;

	t = get_next_token(value);
	if(t != CLOSE_BRACE)
	{
		error("Expected close brace.");
	}

	t = get_next_token(value);
	if(t != NEWLINE && t != FILE_END)
	{
		error("Expected newline.");
	}
}

void parser::parse_value_pair()
{
	std::string value;
	Token t = get_next_token(value);

	if(t == NEWLINE)
	{
		// accept empty line
		return;
	}

	if(t != IDENTIFIER)
	{
		error("Expected identifier.");
	}
	std::string name = value;

	t = get_next_token(value);
	if(t != EQUALS)
	{
		error("Expected equals.");
	}

	t = get_next_token(value);
	if(t != IDENTIFIER && t != STRING && t != NUMBER)
	{
		error("Expected identifier or string.");
	}

	conf_.set_value(section_, name, value);

	t = get_next_token(value);
	if(t != NEWLINE && t != FILE_END)
	{
		error("Expected newline.");
	}
}

void parser::error(const std::string& msg)
{
	std::stringstream buff;
	if(!file_.empty())
	{
		buff << file_;
	}
	buff << "(" << line_ << "): " << msg;

	throw std::runtime_error(buff.str());
}
}
