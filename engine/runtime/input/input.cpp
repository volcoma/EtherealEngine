#include "input.h"
#include "../system/engine.h"

namespace runtime
{
	input::input()
	{
		auto p = sf::Mouse::getPosition();
		_current_cursor_position.x = p.x;
		_current_cursor_position.y = p.y;
		_last_cursor_position = _current_cursor_position;
	}

	void input::reset_state(std::chrono::duration<float>)
	{
		key_reset();

		mouse_reset();

		joystick_reset();
	}

	void input::handle_event(const sf::Event& event)
	{
		_action_mapper.handle_event(event);

		if (key_event(event)) return;

		if (mouse_event(event)) return;

		if (joystick_event(event)) return;

	}

	bool input::is_key_pressed(sf::Keyboard::Key key)
	{
		return _keys_pressed[key];
	}

	bool input::is_key_down(sf::Keyboard::Key key)
	{
		return _keys_down[key];
	}

	bool input::is_key_released(sf::Keyboard::Key key)
	{
		return _keys_released[key];
	}

	bool input::is_mouse_button_pressed(sf::Mouse::Button button)
	{
		return _mouse_buttons_pressed[button];
	}

	bool input::is_mouse_button_down(sf::Mouse::Button button)
	{
		return _mouse_buttons_down[button];
	}

	bool input::is_mouse_button_released(sf::Mouse::Button button)
	{
		return _mouse_buttons_released[button];
	}

	bool input::is_joystick_connected(unsigned int joystickId)
	{
		return _joysticks_connected[joystickId];
	}

	bool input::is_joystick_active(unsigned int joystickId)
	{
		return _joysticks_active[joystickId];
	}

	bool input::is_joystick_disconnected(unsigned int joystickId)
	{
		return _joysticks_disconnected[joystickId];
	}

	bool input::is_joystick_button_pressed(unsigned int joystickId, unsigned int button)
	{
		return _joystick_buttons_pressed[std::pair<unsigned int, unsigned int>(joystickId, button)];
	}

	bool input::is_joystick_button_down(unsigned int joystickId, unsigned int button)
	{
		return _joystick_buttons_down[std::pair<unsigned int, unsigned int>(joystickId, button)];
	}

	bool input::is_joystick_button_released(unsigned int joystickId, unsigned int button)
	{
		return _joystick_buttons_released[std::pair<unsigned int, unsigned int>(joystickId, button)];
	}

	float input::get_joystick_axis_position(unsigned int joystickId, sf::Joystick::Axis axis)
	{
		return _joystick_axis_positions[std::pair<unsigned int, unsigned int>(joystickId, axis)];
	}

	void input::key_reset()
	{
		for (auto& it : _keys_pressed)
		{
			if (it.second)
			{
				_keys_down[it.first] = true;
				it.second = false;
			}
		}

		for (auto& it : _keys_released)
		{
			if (it.second)
				it.second = false;
		}
	}

	bool input::key_event(const sf::Event& event)
	{
		if (event.type == sf::Event::KeyPressed)
		{
			_keys_pressed[event.key.code] = !_keys_down[event.key.code];
			_keys_released[event.key.code] = false;
			return true;
		}
		else if (event.type == sf::Event::KeyReleased)
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
		for (auto& it : _mouse_buttons_pressed)
		{
			if (it.second)
			{
				_mouse_buttons_down[it.first] = true;
				it.second = false;
			}
		}

		for (auto& it : _mouse_buttons_released)
		{
			if (it.second)
				it.second = false;
		}

		if (mouse_wheel_scrolled)
		{
			mouse_wheel_scrolled = false;
			_mouse_scroll_delta = 0.0f;
		}
	}

	bool input::mouse_event(const sf::Event& event)
	{
		if (event.type == sf::Event::MouseButtonPressed)
		{
			_mouse_buttons_pressed[event.mouseButton.button] = !_mouse_buttons_down[event.mouseButton.button];
			_mouse_buttons_released[event.mouseButton.button] = false;
			return true;
		}
		else if (event.type == sf::Event::MouseButtonReleased)
		{
			_mouse_buttons_pressed[event.mouseButton.button] = false;
			_mouse_buttons_down[event.mouseButton.button] = false;
			_mouse_buttons_released[event.mouseButton.button] = true;
			return true;
		}
		else if (event.type == sf::Event::MouseMoved)
		{
			_last_cursor_position = _current_cursor_position;
			ipoint mouse;
			mouse.x = event.mouseMove.x;
			mouse.y = event.mouseMove.y;
			_current_cursor_position = mouse;
			_mouse_move_event = true;
			return true;
		}
		else if (event.type == sf::Event::MouseWheelMoved)
		{
			mouse_wheel_scrolled = true;
			_mouse_scroll_delta = static_cast<float>(event.mouseWheel.delta);
			return true;
		}
		return false;
	}

	void input::joystick_reset()
	{
		for (auto& it : _joystick_buttons_pressed)
		{
			if (it.second)
			{
				_joystick_buttons_down[it.first] = true;
				it.second = false;
			}
		}

		for (auto& it : _joystick_buttons_released)
		{
			if (it.second)
				it.second = false;
		}

		for (auto& it : _joysticks_connected)
		{
			if (it.second)
			{
				_joysticks_active[it.first] = true;
				it.second = false;
			}
		}

		for (auto& it : _joysticks_disconnected)
		{
			if (it.second)
				it.second = false;
		}
	}

	bool input::joystick_event(const sf::Event& event)
	{
		if (event.type == sf::Event::JoystickConnected)
		{
			_joysticks_connected[event.joystickConnect.joystickId] = !_joysticks_active[event.joystickConnect.joystickId];
			_joysticks_disconnected[event.joystickConnect.joystickId] = false;
			return true;
		}
		else if (event.type == sf::Event::JoystickDisconnected)
		{
			_joysticks_connected[event.joystickConnect.joystickId] = false;
			_joysticks_active[event.joystickConnect.joystickId] = false;
			_joysticks_disconnected[event.joystickConnect.joystickId] = true;
			return true;
		}
		else if (event.type == sf::Event::JoystickButtonPressed)
		{
			std::pair<unsigned int, unsigned int> k(event.joystickButton.joystickId, event.joystickButton.button);
			_joystick_buttons_pressed[k] = !_joystick_buttons_down[k];
			_joystick_buttons_released[k] = false;
			return true;
		}
		else if (event.type == sf::Event::JoystickButtonReleased)
		{
			std::pair<unsigned int, unsigned int> k(event.joystickButton.joystickId, event.joystickButton.button);
			_joystick_buttons_pressed[k] = false;
			_joystick_buttons_down[k] = false;
			_joystick_buttons_released[k] = true;
			return true;
		}
		else if (event.type == sf::Event::JoystickMoved)
		{
			std::pair<unsigned int, unsigned int> k(event.joystickMove.joystickId, event.joystickMove.axis);
			_joystick_axis_positions[k] = event.joystickMove.position;
			return true;
		}
		return false;
	}

	ipoint input::get_cursor_delta_move() const
	{
		return ipoint{ get_current_cursor_position().x - get_last_cursor_position().x, get_current_cursor_position().y - get_last_cursor_position().y };
	}

	bool input::initialize()
	{
		on_frame_begin.connect(this, &input::reset_state);

		return true;
	}

	void input::dispose()
	{
		on_frame_begin.disconnect(this, &input::reset_state);
	}

}