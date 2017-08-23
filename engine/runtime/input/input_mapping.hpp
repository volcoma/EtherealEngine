#pragma once

#include "core/common/hash.hpp"
#include "mml/window/event.hpp"
#include "mml/window/joystick.hpp"
#include "mml/window/keyboard.hpp"
#include "mml/window/mouse.hpp"
#include "mml/window/sensor.hpp"
#include "mml/window/touch.hpp"
#include <map>
#include <string>
#include <vector>
namespace std
{
template <typename S, typename T>
struct hash<pair<S, T>>
{
	inline size_t operator()(const pair<S, T>& v) const
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

template <typename T>
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

protected:
	/// mappings
	std::map<T, std::vector<std::string>> _bindings;
};

struct keyboard_mapper : public input_mapper<mml::keyboard::key>
{
	input_mapping get_mapping(const mml::platform_event& e)
	{
		input_mapping binds;
		if(e.type == mml::platform_event::event_type::key_pressed)
		{
			binds.actions = _bindings[e.key.code];
			binds.type = action_type::pressed;
		}
		if(e.type == mml::platform_event::event_type::key_released)
		{
			binds.actions = _bindings[e.key.code];
			binds.type = action_type::released;
		}
		return binds;
	}
};

struct mouse_button_mapper : public input_mapper<mml::mouse::button>
{
	input_mapping get_mapping(const mml::platform_event& e)
	{
		input_mapping binds;
		if(e.type == mml::platform_event::event_type::mouse_button_pressed)
		{
			binds.actions = _bindings[e.mouse_button.button];
			binds.type = action_type::pressed;
		}
		if(e.type == mml::platform_event::event_type::mouse_button_released)
		{
			binds.actions = _bindings[e.mouse_button.button];
			binds.type = action_type::released;
		}
		return binds;
	}
};

struct mouse_wheel_mapper : public input_mapper<mml::mouse::wheel>
{
	input_mapping get_mapping(const mml::platform_event& e)
	{
		input_mapping binds;
		if(e.type == mml::platform_event::event_type::mouse_wheel_scrolled)
		{
			binds.actions = _bindings[e.mouse_wheel_scroll.wheel];
			binds.type = action_type::changed;
		}
		return binds;
	}
};

struct touch_finger_mapper : public input_mapper<unsigned int>
{
	input_mapping get_mapping(const mml::platform_event& e)
	{
		input_mapping binds;
		if(e.type == mml::platform_event::event_type::touch_began)
		{
			binds.actions = _bindings[e.touch.finger];
			binds.type = action_type::pressed;
		}
		if(e.type == mml::platform_event::event_type::touch_moved)
		{
			binds.actions = _bindings[e.touch.finger];
			binds.type = action_type::changed;
		}
		if(e.type == mml::platform_event::event_type::touch_ended)
		{
			binds.actions = _bindings[e.touch.finger];
			binds.type = action_type::released;
		}
		return binds;
	}
};

struct joystick_button_mapper : public input_mapper<std::pair<unsigned int, unsigned int>>
{
	input_mapping get_mapping(const mml::platform_event& e)
	{
		input_mapping binds;
		if(e.type == mml::platform_event::event_type::joystick_button_pressed)
		{
			binds.actions = _bindings[{e.joystick_button.joystick_id, e.joystick_button.button}];
			binds.type = action_type::pressed;
		}
		if(e.type == mml::platform_event::event_type::joystick_button_released)
		{
			binds.actions = _bindings[{e.joystick_button.joystick_id, e.joystick_button.button}];
			binds.type = action_type::released;
		}
		return binds;
	}
};

struct event_mapper : public input_mapper<mml::platform_event::event_type>
{
	input_mapping get_mapping(const mml::platform_event& e)
	{
		input_mapping binds;
		binds.actions = _bindings[e.type];
		binds.type = action_type::changed;
		return binds;
	}
};
