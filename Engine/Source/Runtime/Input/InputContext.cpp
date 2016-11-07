#include "InputContext.h"

InputContext::InputContext()
{
	auto p = sf::Mouse::getPosition();
	mCurrentMouseInfo.x = p.x;
	mCurrentMouseInfo.y = p.y;
	mPreviousMouseInfo = mCurrentMouseInfo;
}

void InputContext::update()
{
	keyUpdate();

	mouseUpdate();

	joystickUpdate();
}

void InputContext::handleEvent(const sf::Event& event)
{
	if (mActionMapper)
		mActionMapper->handleEvent(event);

	if (keyEvent(event)) return;

	if (mouseEvent(event)) return;

	if (joystickEvent(event)) return;

}

bool InputContext::isKeyPressed(sf::Keyboard::Key key)
{
	return mKeysPressed[key];
}

bool InputContext::isKeyDown(sf::Keyboard::Key key)
{
	return mKeysDown[key];
}

bool InputContext::isKeyReleased(sf::Keyboard::Key key)
{
	return mKeysReleased[key];
}

bool InputContext::isMouseButtonPressed(sf::Mouse::Button button)
{
	return mMouseButtonsPressed[button];
}

bool InputContext::isMouseButtonDown(sf::Mouse::Button button)
{
	return mMouseButtonsDown[button];
}

bool InputContext::isMouseButtonReleased(sf::Mouse::Button button)
{
	return mMouseButtonsReleased[button];
}

bool InputContext::isJoystickConnected(unsigned int joystickId)
{
	return mJoysticsConnected[joystickId];
}

bool InputContext::isJoystickActive(unsigned int joystickId)
{
	return mJoysticksActive[joystickId];
}

bool InputContext::isJoystickDisconnected(unsigned int joystickId)
{
	return mJoysticksDisconnected[joystickId];
}

bool InputContext::isJoystickButtonPressed(unsigned int joystickId, unsigned int button)
{
	return mJoystickButtonsPressed[std::pair<unsigned int, unsigned int>(joystickId, button)];
}

bool InputContext::isJoystickButtonDown(unsigned int joystickId, unsigned int button)
{
	return mJoystickButtonsDown[std::pair<unsigned int, unsigned int>(joystickId, button)];
}

bool InputContext::isJoystickButtonReleased(unsigned int joystickId, unsigned int button)
{
	return mJoystickButtonsReleased[std::pair<unsigned int, unsigned int>(joystickId, button)];
}

float InputContext::getJoystickAxisPosition(unsigned int joystickId, sf::Joystick::Axis axis)
{
	return mJoystickAxisPosition[std::pair<unsigned int, unsigned int>(joystickId, axis)];
}

void InputContext::keyUpdate()
{
	for (auto& it : mKeysPressed)
	{
		if (it.second)
		{
			mKeysDown[it.first] = true;
			it.second = false;
		}
	}

	for (auto& it : mKeysReleased)
	{
		if (it.second)
			it.second = false;
	}
}

bool InputContext::keyEvent(const sf::Event& event)
{
	if (event.type == sf::Event::KeyPressed)
	{
		mKeysPressed[event.key.code] = !mKeysDown[event.key.code];
		mKeysReleased[event.key.code] = false;
		return true;
	}
	else if (event.type == sf::Event::KeyReleased)
	{
		mKeysPressed[event.key.code] = false;
		mKeysDown[event.key.code] = false;
		mKeysReleased[event.key.code] = true;
		return true;
	}
	return false;
}

void InputContext::mouseUpdate()
{
	mMouseMoveEvent = false;
	mPreviousMouseInfo = mCurrentMouseInfo;
	for (auto& it : mMouseButtonsPressed)
	{
		if (it.second)
		{
			mMouseButtonsDown[it.first] = true;
			it.second = false;
		}
	}

	for (auto& it : mMouseButtonsReleased)
	{
		if (it.second)
			it.second = false;
	}

	if (mMouseWheelScrolled)
	{
		mMouseWheelScrolled = false;
		mMouseScrollDelta = 0.0f;
	}
}

bool InputContext::mouseEvent(const sf::Event& event)
{
	if (event.type == sf::Event::MouseButtonPressed)
	{
		mMouseButtonsPressed[event.mouseButton.button] = !mMouseButtonsDown[event.mouseButton.button];
		mMouseButtonsReleased[event.mouseButton.button] = false;
		return true;
	}
	else if (event.type == sf::Event::MouseButtonReleased)
	{
		mMouseButtonsPressed[event.mouseButton.button] = false;
		mMouseButtonsDown[event.mouseButton.button] = false;
		mMouseButtonsReleased[event.mouseButton.button] = true;
		return true;
	}
	else if (event.type == sf::Event::MouseMoved)
	{
		mPreviousMouseInfo = mCurrentMouseInfo;
		iPoint mouse;
		mouse.x = event.mouseMove.x;
		mouse.y = event.mouseMove.y;
		mCurrentMouseInfo = mouse;
		mMouseMoveEvent = true;
		return true;
	}
	else if (event.type == sf::Event::MouseWheelMoved)
	{
		mMouseWheelScrolled = true;
		mMouseScrollDelta = static_cast<float>(event.mouseWheel.delta);
		return true;
	}
	return false;
}

void InputContext::joystickUpdate()
{
	for (auto& it : mJoystickButtonsPressed)
	{
		if (it.second)
		{
			mJoystickButtonsDown[it.first] = true;
			it.second = false;
		}
	}

	for (auto& it : mJoystickButtonsReleased)
	{
		if (it.second)
			it.second = false;
	}

	for (auto& it : mJoysticsConnected)
	{
		if (it.second)
		{
			mJoysticksActive[it.first] = true;
			it.second = false;
		}
	}

	for (auto& it : mJoysticksDisconnected)
	{
		if (it.second)
			it.second = false;
	}
}

bool InputContext::joystickEvent(const sf::Event& event)
{
	if (event.type == sf::Event::JoystickConnected)
	{
		mJoysticsConnected[event.joystickConnect.joystickId] = !mJoysticksActive[event.joystickConnect.joystickId];
		mJoysticksDisconnected[event.joystickConnect.joystickId] = false;
		return true;
	}
	else if (event.type == sf::Event::JoystickDisconnected)
	{
		mJoysticsConnected[event.joystickConnect.joystickId] = false;
		mJoysticksActive[event.joystickConnect.joystickId] = false;
		mJoysticksDisconnected[event.joystickConnect.joystickId] = true;
		return true;
	}
	else if (event.type == sf::Event::JoystickButtonPressed)
	{
		std::pair<unsigned int, unsigned int> k(event.joystickButton.joystickId, event.joystickButton.button);
		mJoystickButtonsPressed[k] = !mJoystickButtonsDown[k];
		mJoystickButtonsReleased[k] = false;
		return true;
	}
	else if (event.type == sf::Event::JoystickButtonReleased)
	{
		std::pair<unsigned int, unsigned int> k(event.joystickButton.joystickId, event.joystickButton.button);
		mJoystickButtonsPressed[k] = false;
		mJoystickButtonsDown[k] = false;
		mJoystickButtonsReleased[k] = true;
		return true;
	}
	else if (event.type == sf::Event::JoystickMoved)
	{
		std::pair<unsigned int, unsigned int> k(event.joystickMove.joystickId, event.joystickMove.axis);
		mJoystickAxisPosition[k] = event.joystickMove.position;
		return true;
	}
	return false;
}
