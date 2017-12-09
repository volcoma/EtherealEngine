#ifndef CFG_CONFIG_H
#define CFG_CONFIG_H

#include <iostream>
#include <map>
#include <sstream>
#include <string>
#include <type_traits>

namespace cfg
{
class config
{
public:
	config();

	~config();

	bool has_value(const std::string& section, const std::string& name) const;

	const std::string& get(const std::string& section, const std::string& name) const;

	const std::string& get_value(const std::string& section, const std::string& name,
								 const std::string& defaultValue);

	template <typename T>
	T get_value(const std::string& section, const std::string& name, const T& defaultValue)
	{
		if(!has_value(section, name))
			return defaultValue;

		const auto& value = get(section, name);

		typename std::decay<T>::type result;
		std::istringstream(value) >> result;
		return result;
	}

	void set(const std::string& section, const std::string& name, const std::string& value);

	template <typename T>
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
