#pragma once

#include "../System/SFML/Window/Event.hpp"
#include "../System/SFML/Window/Mouse.hpp"
#include "../System/SFML/Window/Keyboard.hpp"
#include "../System/SFML/Window/Touch.hpp"
#include "../System/SFML/Window/Joystick.hpp"
#include "../System/SFML/Window/Sensor.hpp"
#include <unordered_map>
#include <vector>
#include <string>

template <class T>
inline void hash_combine(std::size_t & seed, const T & v)
{
	std::hash<T> hasher;
	seed ^= hasher(v) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
}

namespace std
{
	template<typename S, typename T> struct hash<pair<S, T>>
	{
		inline size_t operator()(const pair<S, T> & v) const
		{
			size_t seed = 0;
			::hash_combine(seed, v.first);
			::hash_combine(seed, v.second);
			return seed;
		}
	};
}

enum class ActionType : unsigned int
{
	NotMapped,
	Pressed,
	Changed,
	Released,
	Count,
};

struct Mapping
{
	ActionType type = ActionType::NotMapped;
	std::vector<std::string> actions;
};

template<typename T>
class InputMapper
{
public:
	~InputMapper() {}

	void map(const std::string& action, T input)
	{
		bindings[input].push_back(action);
	}

	virtual Mapping getMapping(const sf::Event& e) = 0;

protected:
	std::unordered_map<T, std::vector<std::string>> bindings;
};


struct KeyboardMapper : public InputMapper<sf::Keyboard::Key>
{
	virtual Mapping getMapping(const sf::Event& e)
	{
		Mapping binds;
		if (e.type == sf::Event::EventType::KeyPressed)
		{
			binds.actions = bindings[e.key.code];
			binds.type = ActionType::Pressed;
		}
		if (e.type == sf::Event::EventType::KeyReleased)
		{
			binds.actions = bindings[e.key.code];
			binds.type = ActionType::Released;
		}
		return binds;
	}

};

struct MouseButtonMapper : public InputMapper<sf::Mouse::Button>
{
	virtual Mapping getMapping(const sf::Event& e)
	{
		Mapping binds;
		if (e.type == sf::Event::EventType::MouseButtonPressed)
		{
			binds.actions = bindings[e.mouseButton.button];
			binds.type = ActionType::Pressed;
		}
		if (e.type == sf::Event::EventType::MouseButtonReleased)
		{
			binds.actions = bindings[e.mouseButton.button];
			binds.type = ActionType::Released;
		}
		return binds;
	}
};

struct MouseWheelMapper : public InputMapper<sf::Mouse::Wheel>
{
	virtual Mapping getMapping(const sf::Event& e)
	{
		Mapping binds;
		if (e.type == sf::Event::EventType::MouseWheelScrolled)
		{
			binds.actions = bindings[e.mouseWheelScroll.wheel];
			binds.type = ActionType::Changed;
		}
		return binds;
	}
};

struct TouchFingerMapper : public InputMapper<unsigned int>
{
	virtual Mapping getMapping(const sf::Event& e)
	{
		Mapping binds;
		if (e.type == sf::Event::EventType::TouchBegan)
		{
			binds.actions = bindings[e.touch.finger];
			binds.type = ActionType::Pressed;
		}
		if (e.type == sf::Event::EventType::TouchMoved)
		{
			binds.actions = bindings[e.touch.finger];
			binds.type = ActionType::Changed;
		}
		if (e.type == sf::Event::EventType::TouchEnded)
		{
			binds.actions = bindings[e.touch.finger];
			binds.type = ActionType::Released;
		}
		return binds;
	}
};


// struct JoystickMapping
// {
// 	unsigned int joystickId;
// 	unsigned int button;
// 
// 	bool operator==(const JoystickMapping& rhs)
// 	{
// 		return joystickId == rhs.joystickId && button == rhs.button;
// 	}
// 
// 	bool operator==(const JoystickMapping rhs)
// 	{
// 		return joystickId == rhs.joystickId && button == rhs.button;
// 	}
// };

// namespace std
// {
// 
// 	template<>
// 	struct hash<JoystickMapping>
// 	{
// 		inline size_t operator()(const JoystickMapping & v) const
// 		{
// 			size_t seed = 0;
// 			::hash_combine(seed, v.joystickId);
// 			::hash_combine(seed, v.button);
// 			return seed;
// 		}
// 	};
// }


struct JoystickButtonMapper : public InputMapper<std::pair<unsigned int, unsigned int>>
{
	virtual Mapping getMapping(const sf::Event& e)
	{
		Mapping binds;
		if (e.type == sf::Event::EventType::JoystickButtonPressed)
		{
			binds.actions = bindings[{e.joystickButton.joystickId, e.joystickButton.button}];
			binds.type = ActionType::Pressed;
		}
		if (e.type == sf::Event::EventType::JoystickButtonReleased)
		{
			binds.actions = bindings[{e.joystickButton.joystickId, e.joystickButton.button}];
			binds.type = ActionType::Released;
		}
		return binds;
	}
};

struct EventMapper : public InputMapper<sf::Event::EventType>
{
	virtual Mapping getMapping(const sf::Event& e)
	{
		Mapping binds;
		binds.actions = bindings[e.type];
		binds.type = ActionType::Changed;
		return binds;
	}
};