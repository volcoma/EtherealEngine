#pragma once

#include "../system/sfml/Window/Event.hpp"
#include "../system/sfml/Window/Mouse.hpp"
#include "../system/sfml/Window/Keyboard.hpp"
#include "../system/sfml/Window/Touch.hpp"
#include "../system/sfml/Window/Joystick.hpp"
#include "../system/sfml/Window/Sensor.hpp"
#include <unordered_map>
#include <vector>
#include <string>
#include "core/common/utils.h"
namespace std
{
	template<typename S, typename T> struct hash<pair<S, T>>
	{
		inline size_t operator()(const pair<S, T> & v) const
		{
			size_t seed = 0;
			utils::hash_combine(seed, v.first);
			utils::hash_combine(seed, v.second);
			return seed;
		}
	};
}

enum class action_type : unsigned int
{
	not_mapped,
	pressed,
	changed,
	released
};

struct input_mapping
{
	action_type type = action_type::not_mapped;
	std::vector<std::string> actions;
};

template<typename T>
class input_mapper
{
public:
	//-----------------------------------------------------------------------------
	//  Name : map ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void map(const std::string& action, T input)
	{
		_bindings[input].push_back(action);
	}

	//-----------------------------------------------------------------------------
	//  Name : get_mapping (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual input_mapping get_mapping(const sf::Event& e) = 0;

protected:
	/// mappings
	std::unordered_map<T, std::vector<std::string>> _bindings;
};


struct keyboard_mapper : public input_mapper<sf::Keyboard::Key>
{
	virtual input_mapping get_mapping(const sf::Event& e)
	{
		input_mapping binds;
		if (e.type == sf::Event::EventType::KeyPressed)
		{
			binds.actions = _bindings[e.key.code];
			binds.type = action_type::pressed;
		}
		if (e.type == sf::Event::EventType::KeyReleased)
		{
			binds.actions = _bindings[e.key.code];
			binds.type = action_type::released;
		}
		return binds;
	}

};

struct mouse_button_mapper : public input_mapper<sf::Mouse::Button>
{
	virtual input_mapping get_mapping(const sf::Event& e)
	{
		input_mapping binds;
		if (e.type == sf::Event::EventType::MouseButtonPressed)
		{
			binds.actions = _bindings[e.mouseButton.button];
			binds.type = action_type::pressed;
		}
		if (e.type == sf::Event::EventType::MouseButtonReleased)
		{
			binds.actions = _bindings[e.mouseButton.button];
			binds.type = action_type::released;
		}
		return binds;
	}
};

struct mouse_wheel_mapper : public input_mapper<sf::Mouse::Wheel>
{
	virtual input_mapping get_mapping(const sf::Event& e)
	{
		input_mapping binds;
		if (e.type == sf::Event::EventType::MouseWheelScrolled)
		{
			binds.actions = _bindings[e.mouseWheelScroll.wheel];
			binds.type = action_type::changed;
		}
		return binds;
	}
};

struct touch_finger_mapper : public input_mapper<unsigned int>
{
	virtual input_mapping get_mapping(const sf::Event& e)
	{
		input_mapping binds;
		if (e.type == sf::Event::EventType::TouchBegan)
		{
			binds.actions = _bindings[e.touch.finger];
			binds.type = action_type::pressed;
		}
		if (e.type == sf::Event::EventType::TouchMoved)
		{
			binds.actions = _bindings[e.touch.finger];
			binds.type = action_type::changed;
		}
		if (e.type == sf::Event::EventType::TouchEnded)
		{
			binds.actions = _bindings[e.touch.finger];
			binds.type = action_type::released;
		}
		return binds;
	}
};

struct joystick_button_mapper : public input_mapper<std::pair<unsigned int, unsigned int>>
{
	virtual input_mapping get_mapping(const sf::Event& e)
	{
		input_mapping binds;
		if (e.type == sf::Event::EventType::JoystickButtonPressed)
		{
			binds.actions = _bindings[{e.joystickButton.joystickId, e.joystickButton.button}];
			binds.type = action_type::pressed;
		}
		if (e.type == sf::Event::EventType::JoystickButtonReleased)
		{
			binds.actions = _bindings[{e.joystickButton.joystickId, e.joystickButton.button}];
			binds.type = action_type::released;
		}
		return binds;
	}
};

struct event_mapper : public input_mapper<sf::Event::EventType>
{
	virtual input_mapping get_mapping(const sf::Event& e)
	{
		input_mapping binds;
		binds.actions = _bindings[e.type];
		binds.type = action_type::changed;
		return binds;
	}
};