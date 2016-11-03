#pragma once

#include <unordered_map>
#include "InputMapping.hpp"

#include "Core/common/basetypes.hpp"
#include "Core/events/event.hpp"

using ActionMap = std::unordered_map<std::string, std::unordered_map<ActionType, event<void(const sf::Event&)>>>;

struct ActionMapper
{
	KeyboardMapper			keyboardMapper;
	MouseButtonMapper		mouseButtonMapper;
	MouseWheelMapper		mouseWheelMapper;
	TouchFingerMapper		touchFingerMapper;
	JoystickButtonMapper	joystickMapper;
	EventMapper				eventMapper;
	ActionMap				actionMap;

	void handleEvent(const sf::Event& event)
	{
		auto triggerCallbacks = [this](auto mapper, const sf::Event& event)
		{
			auto mappings = mapper.getMapping(event);
			for (auto& action : mappings.actions)
			{
				auto& mappedEvent = actionMap[action][mappings.type];
				mappedEvent(event);
			}
		};

		triggerCallbacks(keyboardMapper, event);
		triggerCallbacks(mouseButtonMapper, event);
		triggerCallbacks(mouseWheelMapper, event);
		triggerCallbacks(touchFingerMapper, event);
		triggerCallbacks(joystickMapper, event);
		triggerCallbacks(eventMapper, event);
	}
};


class InputContext
{
public:
	InputContext								();
	~InputContext								() {}

	void			setActionMapper				(ActionMapper* actionMapper);
	void			update						();
	void			handleEvent					(const sf::Event& event);

	bool			isKeyPressed				(sf::Keyboard::Key key);
	bool			isKeyDown					(sf::Keyboard::Key key);
	bool			isKeyReleased				(sf::Keyboard::Key key);

	bool			mouseMoved					();
	const iPoint&	getMouseCurrentPosition		();
	const iPoint&	getMousePreviousPosition	();
	bool			isMouseButtonPressed		(sf::Mouse::Button button);
	bool			isMouseButtonDown			(sf::Mouse::Button button);
	bool			isMouseButtonReleased		(sf::Mouse::Button button);
	bool			mouseWheelScrolled			();
	float			getMouseWheelScrollDelta	();

	bool			isJoystickConnected			(unsigned int joystickId);
	bool			isJoystickActive			(unsigned int joystickId);
	bool			isJoystickDisconnected		(unsigned int joystickId);
	bool			isJoystickButtonPressed		(unsigned int joystickId, unsigned int button);
	bool			isJoystickButtonDown		(unsigned int joystickId, unsigned int button);
	bool			isJoystickButtonReleased	(unsigned int joystickId, unsigned int button);
	float			getJoystickAxisPosition		(unsigned int joystickId, sf::Joystick::Axis axis);

private:
	void			keyUpdate					();
	bool			keyEvent					(const sf::Event& event);


	void			mouseUpdate					();
	bool			mouseEvent					(const sf::Event& event);



	void			joystickUpdate				();
	bool			joystickEvent				(const sf::Event& event);

	// non_owning
	ActionMapper*	mActionMapper = nullptr;

	bool			mMouseMoveEvent = false;
	bool			mMouseWheelScrolled = false;
	float			mMouseScrollDelta = 0.0f;
	iPoint			mCurrentMouseInfo;
	iPoint			mPreviousMouseInfo;

	std::unordered_map<unsigned int, bool> mMouseButtonsPressed;
	std::unordered_map<unsigned int, bool> mMouseButtonsDown;
	std::unordered_map<unsigned int, bool> mMouseButtonsReleased;

	std::unordered_map<unsigned int, bool> mKeysPressed;
	std::unordered_map<unsigned int, bool> mKeysDown;
	std::unordered_map<unsigned int, bool> mKeysReleased;


	std::unordered_map<unsigned int, bool> mJoysticsConnected;
	std::unordered_map<unsigned int, bool> mJoysticksActive;
	std::unordered_map<unsigned int, bool> mJoysticksDisconnected;
	std::unordered_map<std::pair<unsigned int, unsigned int>, bool> mJoystickButtonsPressed;
	std::unordered_map<std::pair<unsigned int, unsigned int>, bool> mJoystickButtonsDown;
	std::unordered_map<std::pair<unsigned int, unsigned int>, bool> mJoystickButtonsReleased;
	std::unordered_map<std::pair<unsigned int, unsigned int>, float> mJoystickAxisPosition;
};