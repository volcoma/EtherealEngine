#include "input.h"
#include "../system/events.h"

namespace runtime
{
input::input()
{
	auto p = mml::mouse::get_position();
	_current_cursor_position.x = p[0];
	_current_cursor_position.y = p[1];
	_last_cursor_position = _current_cursor_position;

	on_platform_events.connect(this, &input::platform_events);
	on_frame_end.connect(this, &input::reset_state);
}

input::~input()
{
	on_platform_events.disconnect(this, &input::platform_events);
	on_frame_end.disconnect(this, &input::reset_state);
}

void input::reset_state(std::chrono::duration<float>)
{
	key_reset();

	mouse_reset();

	joystick_reset();
}

void input::handle_event(const mml::platform_event& event)
{
	_action_mapper.handle_event(event);

	if(key_event(event))
		return;

	if(mouse_event(event))
		return;

	if(joystick_event(event))
		return;
}

bool input::is_key_pressed(mml::keyboard::key key)
{
	return _keys_pressed[key];
}

bool input::is_key_down(mml::keyboard::key key)
{
	return _keys_down[key];
}

bool input::is_key_released(mml::keyboard::key key)
{
	return _keys_released[key];
}

bool input::is_mouse_button_pressed(mml::mouse::button button)
{
	return _mouse_buttons_pressed[button];
}

bool input::is_mouse_button_down(mml::mouse::button button)
{
	return _mouse_buttons_down[button];
}

bool input::is_mouse_button_released(mml::mouse::button button)
{
	return _mouse_buttons_released[button];
}

bool input::is_joystick_connected(unsigned int joystick_id)
{
	return _joysticks_connected[joystick_id];
}

bool input::is_joystick_active(unsigned int joystick_id)
{
	return _joysticks_active[joystick_id];
}

bool input::is_joystick_disconnected(unsigned int joystick_id)
{
	return _joysticks_disconnected[joystick_id];
}

bool input::is_joystick_button_pressed(unsigned int joystick_id, unsigned int button)
{
	return _joystick_buttons_pressed[std::pair<unsigned int, unsigned int>(joystick_id, button)];
}

bool input::is_joystick_button_down(unsigned int joystick_id, unsigned int button)
{
	return _joystick_buttons_down[std::pair<unsigned int, unsigned int>(joystick_id, button)];
}

bool input::is_joystick_button_released(unsigned int joystick_id, unsigned int button)
{
	return _joystick_buttons_released[std::pair<unsigned int, unsigned int>(joystick_id, button)];
}

float input::get_joystick_axis_position(unsigned int joystick_id, mml::joystick::axis axis)
{
	return _joystick_axis_positions[std::pair<unsigned int, unsigned int>(joystick_id, axis)];
}

void input::key_reset()
{
	for(auto& it : _keys_pressed)
	{
		if(it.second)
		{
			_keys_down[it.first] = true;
			it.second = false;
		}
	}

	for(auto& it : _keys_released)
	{
		if(it.second)
			it.second = false;
	}
}

bool input::key_event(const mml::platform_event& event)
{
	if(event.type == mml::platform_event::key_pressed)
	{
		_keys_pressed[event.key.code] = !_keys_down[event.key.code];
		_keys_released[event.key.code] = false;
		return true;
	}
	else if(event.type == mml::platform_event::key_released)
	{
		_keys_pressed[event.key.code] = false;
		_keys_down[event.key.code] = false;
		_keys_released[event.key.code] = true;
		return true;
	}
	return false;
}

void input::mouse_reset()
{
	_mouse_move_event = false;
	_last_cursor_position = _current_cursor_position;
	for(auto& it : _mouse_buttons_pressed)
	{
		if(it.second)
		{
			_mouse_buttons_down[it.first] = true;
			it.second = false;
		}
	}

	for(auto& it : _mouse_buttons_released)
	{
		if(it.second)
			it.second = false;
	}

	if(mouse_wheel_scrolled)
	{
		mouse_wheel_scrolled = false;
		_mouse_scroll_delta = 0.0f;
	}
}

bool input::mouse_event(const mml::platform_event& event)
{
	if(event.type == mml::platform_event::mouse_button_pressed)
	{
		_mouse_buttons_pressed[event.mouse_button.button] = !_mouse_buttons_down[event.mouse_button.button];
		_mouse_buttons_released[event.mouse_button.button] = false;
		return true;
	}
	else if(event.type == mml::platform_event::mouse_button_released)
	{
		_mouse_buttons_pressed[event.mouse_button.button] = false;
		_mouse_buttons_down[event.mouse_button.button] = false;
		_mouse_buttons_released[event.mouse_button.button] = true;
		return true;
	}
	else if(event.type == mml::platform_event::mouse_moved)
	{
		_last_cursor_position = _current_cursor_position;
		ipoint mouse;
		mouse.x = event.mouse_move.x;
		mouse.y = event.mouse_move.y;
		_current_cursor_position = mouse;
		_mouse_move_event = true;
		return true;
	}
	else if(event.type == mml::platform_event::mouse_wheel_scrolled)
	{
		mouse_wheel_scrolled = true;
		_mouse_scroll_delta = static_cast<float>(event.mouse_wheel_scroll.delta);
		return true;
	}
	return false;
}

void input::joystick_reset()
{
	for(auto& it : _joystick_buttons_pressed)
	{
		if(it.second)
		{
			_joystick_buttons_down[it.first] = true;
			it.second = false;
		}
	}

	for(auto& it : _joystick_buttons_released)
	{
		if(it.second)
			it.second = false;
	}

	for(auto& it : _joysticks_connected)
	{
		if(it.second)
		{
			_joysticks_active[it.first] = true;
			it.second = false;
		}
	}

	for(auto& it : _joysticks_disconnected)
	{
		if(it.second)
			it.second = false;
	}
}

bool input::joystick_event(const mml::platform_event& event)
{
	if(event.type == mml::platform_event::joystick_connected)
	{
		_joysticks_connected[event.joystick_connect.joystick_id] =
			!_joysticks_active[event.joystick_connect.joystick_id];
		_joysticks_disconnected[event.joystick_connect.joystick_id] = false;
		return true;
	}
	else if(event.type == mml::platform_event::joystick_disconnected)
	{
		_joysticks_connected[event.joystick_connect.joystick_id] = false;
		_joysticks_active[event.joystick_connect.joystick_id] = false;
		_joysticks_disconnected[event.joystick_connect.joystick_id] = true;
		return true;
	}
	else if(event.type == mml::platform_event::joystick_button_pressed)
	{
		std::pair<unsigned int, unsigned int> k(event.joystick_button.joystick_id,
												event.joystick_button.button);
		_joystick_buttons_pressed[k] = !_joystick_buttons_down[k];
		_joystick_buttons_released[k] = false;
		return true;
	}
	else if(event.type == mml::platform_event::joystick_button_released)
	{
		std::pair<unsigned int, unsigned int> k(event.joystick_button.joystick_id,
												event.joystick_button.button);
		_joystick_buttons_pressed[k] = false;
		_joystick_buttons_down[k] = false;
		_joystick_buttons_released[k] = true;
		return true;
	}
	else if(event.type == mml::platform_event::joystick_moved)
	{
		std::pair<unsigned int, unsigned int> k(event.joystick_move.joystick_id, event.joystick_move.axis);
		_joystick_axis_positions[k] = event.joystick_move.position;
		return true;
	}
	return false;
}

ipoint input::get_cursor_delta_move() const
{
	return ipoint{get_current_cursor_position().x - get_last_cursor_position().x,
				  get_current_cursor_position().y - get_last_cursor_position().y};
}

void input::platform_events(const std::pair<std::uint32_t, bool>& info,
							const std::vector<mml::platform_event>& events)
{
	if(info.second)
	{
		for(const auto& e : events)
		{
			handle_event(e);
		}
	}
}
}
