
#ifndef _CFG_PARSER_H_
#define _CFG_PARSER_H_

#include <string>
#include <iostream>

namespace cfg
{
    class config;
    class parser
    {
    public:
    
        parser(config& c);

        ~parser();
        
        /**
         * Parse a stream.
         * 
         * @param in   the stream to parse
         * @param file the file that is parsed, for error purposes 
         * @param line the initial line 
         **/
        void parse(std::istream& in, const std::string& file = "", unsigned int line = 1);
            
    private:
        enum Token
        {
            NO_TOKEN,
            WHITESPACE,
            OPEN_BRACE,  // [
            CLOSE_BRACE, // ]
            EQUALS,      // =
            IDENTIFIER,  // [a-zA-Z_][a-zA-Z0-9_-]*
            STRING,      // "([^"]*)"
            NUMBER,		 // (+|-)[0-9]+(\.[0-9]+)
            NEWLINE,     // \n|(\r\n)|\r
            COMMENT,	 // #[^\n\r]*
            FILE_END,
            ERROR		 // this should never be returned
        };
        
        config&       conf;
        std::istream* in;
        std::string   file;
        unsigned int  line;

        Token next_token;
        std::string next_value;

        std::string section;

        Token get_next_token(std::string& value);

        Token lex_token(std::string& value);
        Token lex_whitespace(std::string& value);
        Token lex_newline(std::string& value);
        Token lex_number(std::string& value);
        Token lex_identifier(std::string& value);
        Token lex_string(std::string& value);
        Token lex_comment(std::string& value);

        void parse_section();

        void parse_section_header();

        void parse_value_pair();

        void error(const std::string& msg);

        parser(const parser&) = delete;
        const parser& operator = (const parser&) = delete;
    };
}

#endif
