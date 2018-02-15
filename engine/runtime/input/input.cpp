#include "input.h"
#include "../system/events.h"

namespace runtime
{
input::input()
{
	auto p = mml::mouse::get_position();
	current_cursor_position_.x = p[0];
	current_cursor_position_.y = p[1];
	last_cursor_position_ = current_cursor_position_;

	on_platform_events.connect(this, &input::platform_events);
	on_frame_end.connect(this, &input::reset_state);
}

input::~input()
{
	on_platform_events.disconnect(this, &input::platform_events);
	on_frame_end.disconnect(this, &input::reset_state);
}

void input::reset_state(delta_t /*unused*/)
{
	key_reset();

	mouse_reset();

	joystick_reset();
}

void input::handle_event(const mml::platform_event& event)
{
	action_mapper_.handle_event(event);

	if(key_event(event))
	{
		return;
	}

	if(mouse_event(event))
	{
		return;
	}

	if(joystick_event(event))
	{
		return;
	}
}

bool input::is_key_pressed(mml::keyboard::key key)
{
    return keys_pressed_[key];
}

bool input::is_key_pressed(mml::keyboard::key key, mml::keyboard::key modifier)
{
    return is_key_pressed(key) && is_key_down(modifier);
}

bool input::is_key_down(mml::keyboard::key key)
{
	return keys_down_[key];
}

bool input::is_key_released(mml::keyboard::key key)
{
	return keys_released_[key];
}

bool input::is_mouse_button_pressed(mml::mouse::button button)
{
	return mouse_buttons_pressed_[button];
}

bool input::is_mouse_button_down(mml::mouse::button button)
{
	return mouse_buttons_down_[button];
}

bool input::is_mouse_button_released(mml::mouse::button button)
{
	return mouse_buttons_released_[button];
}

bool input::is_joystick_connected(unsigned int joystick_id)
{
	return joysticks_connected_[joystick_id];
}

bool input::is_joystick_active(unsigned int joystick_id)
{
	return joysticks_active_[joystick_id];
}

bool input::is_joystick_disconnected(unsigned int joystick_id)
{
	return joysticks_disconnected_[joystick_id];
}

bool input::is_joystick_button_pressed(unsigned int joystick_id, unsigned int button)
{
	return joystick_buttons_pressed_[std::pair<unsigned int, unsigned int>(joystick_id, button)];
}

bool input::is_joystick_button_down(unsigned int joystick_id, unsigned int button)
{
	return joystick_buttons_down_[std::pair<unsigned int, unsigned int>(joystick_id, button)];
}

bool input::is_joystick_button_released(unsigned int joystick_id, unsigned int button)
{
	return joystick_buttons_released_[std::pair<unsigned int, unsigned int>(joystick_id, button)];
}

float input::get_joystick_axis_position(unsigned int joystick_id, mml::joystick::axis axis)
{
	return joystick_axis_positions_[std::pair<unsigned int, unsigned int>(joystick_id, axis)];
}

void input::key_reset()
{
	for(auto& it : keys_pressed_)
	{
		if(it.second)
		{
			keys_down_[it.first] = true;
			it.second = false;
		}
	}

	for(auto& it : keys_released_)
	{
		if(it.second)
		{
			it.second = false;
		}
	}
}

bool input::key_event(const mml::platform_event& event)
{
	if(event.type == mml::platform_event::key_pressed)
	{
		keys_pressed_[event.key.code] = !keys_down_[event.key.code];
		keys_released_[event.key.code] = false;
		return true;
	}
	if(event.type == mml::platform_event::key_released)
	{
		keys_pressed_[event.key.code] = false;
		keys_down_[event.key.code] = false;
		keys_released_[event.key.code] = true;
		return true;
	}
	return false;
}

void input::mouse_reset()
{
	mouse_move_event_ = false;
	last_cursor_position_ = current_cursor_position_;
	for(auto& it : mouse_buttons_pressed_)
	{
		if(it.second)
		{
			mouse_buttons_down_[it.first] = true;
			it.second = false;
		}
	}

	for(auto& it : mouse_buttons_released_)
	{
		if(it.second)
		{
			it.second = false;
		}
	}

	if(mouse_wheel_scrolled_)
	{
		mouse_wheel_scrolled_ = false;
		mouse_scroll_delta_ = 0.0f;
	}
}

bool input::mouse_event(const mml::platform_event& event)
{
	if(event.type == mml::platform_event::mouse_button_pressed)
	{
		mouse_buttons_pressed_[event.mouse_button.button] = !mouse_buttons_down_[event.mouse_button.button];
		mouse_buttons_released_[event.mouse_button.button] = false;
		return true;
	}
	if(event.type == mml::platform_event::mouse_button_released)
	{
		mouse_buttons_pressed_[event.mouse_button.button] = false;
		mouse_buttons_down_[event.mouse_button.button] = false;
		mouse_buttons_released_[event.mouse_button.button] = true;
		return true;
	}
	if(event.type == mml::platform_event::mouse_moved)
	{
		last_cursor_position_ = current_cursor_position_;
		ipoint32_t mouse;
		mouse.x = event.mouse_move.x;
		mouse.y = event.mouse_move.y;
		current_cursor_position_ = mouse;
		mouse_move_event_ = true;
		return true;
	}
	if(event.type == mml::platform_event::mouse_wheel_scrolled)
	{
		mouse_wheel_scrolled_ = true;
		mouse_scroll_delta_ = static_cast<float>(event.mouse_wheel_scroll.delta);
		return true;
	}
	return false;
}

void input::joystick_reset()
{
	for(auto& it : joystick_buttons_pressed_)
	{
		if(it.second)
		{
			joystick_buttons_down_[it.first] = true;
			it.second = false;
		}
	}

	for(auto& it : joystick_buttons_released_)
	{
		if(it.second)
		{
			it.second = false;
		}
	}

	for(auto& it : joysticks_connected_)
	{
		if(it.second)
		{
			joysticks_active_[it.first] = true;
			it.second = false;
		}
	}

	for(auto& it : joysticks_disconnected_)
	{
		if(it.second)
		{
			it.second = false;
		}
	}
}

bool input::joystick_event(const mml::platform_event& event)
{
	if(event.type == mml::platform_event::joystick_connected)
	{
		joysticks_connected_[event.joystick_connect.joystick_id] =
			!joysticks_active_[event.joystick_connect.joystick_id];
		joysticks_disconnected_[event.joystick_connect.joystick_id] = false;
		return true;
	}
	if(event.type == mml::platform_event::joystick_disconnected)
	{
		joysticks_connected_[event.joystick_connect.joystick_id] = false;
		joysticks_active_[event.joystick_connect.joystick_id] = false;
		joysticks_disconnected_[event.joystick_connect.joystick_id] = true;
		return true;
	}
	if(event.type == mml::platform_event::joystick_button_pressed)
	{
		std::pair<unsigned int, unsigned int> k(event.joystick_button.joystick_id,
												event.joystick_button.button);
		joystick_buttons_pressed_[k] = !joystick_buttons_down_[k];
		joystick_buttons_released_[k] = false;
		return true;
	}
	if(event.type == mml::platform_event::joystick_button_released)
	{
		std::pair<unsigned int, unsigned int> k(event.joystick_button.joystick_id,
												event.joystick_button.button);
		joystick_buttons_pressed_[k] = false;
		joystick_buttons_down_[k] = false;
		joystick_buttons_released_[k] = true;
		return true;
	}
	if(event.type == mml::platform_event::joystick_moved)
	{
		std::pair<unsigned int, unsigned int> k(event.joystick_move.joystick_id, event.joystick_move.axis);
		joystick_axis_positions_[k] = event.joystick_move.position;
		return true;
	}
	return false;
}

ipoint32_t input::get_cursor_delta_move() const
{
	return ipoint32_t{get_current_cursor_position().x - get_last_cursor_position().x,
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
