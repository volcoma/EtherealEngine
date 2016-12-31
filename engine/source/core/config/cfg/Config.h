
#ifndef _CFG_CONFIG_H_
#define _CFG_CONFIG_H_

#include <string>
#include <map>
#include <iostream>
#include <sstream>
namespace cfg
{
    class Config
    {
    public:
        Config();

        ~Config();
        
        bool has_value(const std::string& section, const std::string& name) const;

        const std::string get(const std::string& section, const std::string& name) const;

		std::string get_value(const std::string& section, const std::string& name, const std::string& defaultValue)
		{
			if (!has_value(section, name))
				return defaultValue;

			return get(section, name);
		}

		template<typename T>
		T get_value(const std::string& section, const std::string& name, const T& defaultValue)
		{
			if (!has_value(section, name))
				return defaultValue;

			auto value = get(section, name);

			T result;
			std::istringstream(value) >> result;
			return result;
		}

        void set(const std::string& section, const std::string& name, const std::string& value);

		template<typename T>
		void set_value(const std::string& section, const std::string& name, const T& value)
		{
			std::ostringstream stream;
			stream << value;
			set(section, name, stream.str());
		}

        void save(const std::string& file);

        void load(const std::string& file);

    private:
        typedef std::map<std::string, std::string> Section;
        std::map<std::string, Section> values;
    };
}

#endif
