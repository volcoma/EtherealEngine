#pragma once

#include <unordered_map>
#include "input_mapping.hpp"

#include "core/common/basetypes.hpp"
#include "core/events/event.hpp"

using ActionMap = std::unordered_map<std::string, std::unordered_map<ActionType, event<void(const sf::Event&)>>>;

struct ActionMapper
{
	KeyboardMapper keyboardMapper;
	MouseButtonMapper mouseButtonMapper;
	MouseWheelMapper mouseWheelMapper;
	TouchFingerMapper touchFingerMapper;
	JoystickButtonMapper joystickMapper;
	EventMapper eventMapper;
	ActionMap actionMap;

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
	//-----------------------------------------------------------------------------
	//  Name : InputContext ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	InputContext();

	//-----------------------------------------------------------------------------
	//  Name : ~InputContext ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~InputContext() = default;

	//-----------------------------------------------------------------------------
	//  Name : setActionMapper ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setActionMapper(ActionMapper* actionMapper) { mActionMapper = actionMapper; }

	//-----------------------------------------------------------------------------
	//  Name : update ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void update();

	//-----------------------------------------------------------------------------
	//  Name : handleEvent ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void handleEvent(const sf::Event& event);

	//-----------------------------------------------------------------------------
	//  Name : isKeyPressed ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isKeyPressed(sf::Keyboard::Key key);

	//-----------------------------------------------------------------------------
	//  Name : isKeyDown ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isKeyDown(sf::Keyboard::Key key);

	//-----------------------------------------------------------------------------
	//  Name : isKeyReleased ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isKeyReleased(sf::Keyboard::Key key);

	//-----------------------------------------------------------------------------
	//  Name : mouseMoved ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool mouseMoved() const { return mMouseMoveEvent; }

	//-----------------------------------------------------------------------------
	//  Name : getMouseCurrentPosition ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const iPoint& getMouseCurrentPosition() const { return mCurrentMouseInfo; }

	//-----------------------------------------------------------------------------
	//  Name : getMousePreviousPosition ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const iPoint& getMousePreviousPosition() const { return mPreviousMouseInfo; }

	//-----------------------------------------------------------------------------
	//  Name : isMouseButtonPressed ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isMouseButtonPressed(sf::Mouse::Button button);

	//-----------------------------------------------------------------------------
	//  Name : isMouseButtonDown ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isMouseButtonDown(sf::Mouse::Button button);

	//-----------------------------------------------------------------------------
	//  Name : isMouseButtonReleased ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isMouseButtonReleased(sf::Mouse::Button button);

	//-----------------------------------------------------------------------------
	//  Name : mouseWheelScrolled ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool mouseWheelScrolled() const { return mMouseWheelScrolled; }

	//-----------------------------------------------------------------------------
	//  Name : getMouseWheelScrollDelta ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getMouseWheelScrollDelta() const { return mMouseScrollDelta; }

	//-----------------------------------------------------------------------------
	//  Name : isJoystickConnected ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isJoystickConnected(unsigned int joystickId);

	//-----------------------------------------------------------------------------
	//  Name : isJoystickActive ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isJoystickActive(unsigned int joystickId);

	//-----------------------------------------------------------------------------
	//  Name : isJoystickDisconnected ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isJoystickDisconnected(unsigned int joystickId);

	//-----------------------------------------------------------------------------
	//  Name : isJoystickButtonPressed ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isJoystickButtonPressed(unsigned int joystickId, unsigned int button);
	
	//-----------------------------------------------------------------------------
	//  Name : isJoystickButtonDown ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isJoystickButtonDown(unsigned int joystickId, unsigned int button);
	
	//-----------------------------------------------------------------------------
	//  Name : isJoystickButtonReleased ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isJoystickButtonReleased(unsigned int joystickId, unsigned int button);
	
	//-----------------------------------------------------------------------------
	//  Name : getJoystickAxisPosition ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float getJoystickAxisPosition(unsigned int joystickId, sf::Joystick::Axis axis);

private:
	//-----------------------------------------------------------------------------
	//  Name : keyUpdate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void keyUpdate();
	
	//-----------------------------------------------------------------------------
	//  Name : keyEvent ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool keyEvent(const sf::Event& event);
	
	//-----------------------------------------------------------------------------
	//  Name : mouseUpdate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void mouseUpdate();
	
	//-----------------------------------------------------------------------------
	//  Name : mouseEvent ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool mouseEvent(const sf::Event& event);
	
	//-----------------------------------------------------------------------------
	//  Name : joystickUpdate ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void joystickUpdate();
	
	//-----------------------------------------------------------------------------
	//  Name : joystickEvent ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool joystickEvent(const sf::Event& event);

	/// non_owning
	ActionMapper* mActionMapper = nullptr;
	/// 
	bool mMouseMoveEvent = false;
	/// 
	bool mMouseWheelScrolled = false;
	/// 
	float mMouseScrollDelta = 0.0f;
	/// 
	iPoint mCurrentMouseInfo;
	/// 
	iPoint mPreviousMouseInfo;
	/// 
	std::unordered_map<unsigned int, bool> mMouseButtonsPressed;
	/// 
	std::unordered_map<unsigned int, bool> mMouseButtonsDown;
	/// 
	std::unordered_map<unsigned int, bool> mMouseButtonsReleased;
	/// 
	std::unordered_map<unsigned int, bool> mKeysPressed;
	/// 
	std::unordered_map<unsigned int, bool> mKeysDown;
	/// 
	std::unordered_map<unsigned int, bool> mKeysReleased;
	/// 
	std::unordered_map<unsigned int, bool> mJoysticsConnected;
	/// 
	std::unordered_map<unsigned int, bool> mJoysticksActive;
	/// 
	std::unordered_map<unsigned int, bool> mJoysticksDisconnected;
	/// 
	std::unordered_map<std::pair<unsigned int, unsigned int>, bool> mJoystickButtonsPressed;
	/// 
	std::unordered_map<std::pair<unsigned int, unsigned int>, bool> mJoystickButtonsDown;
	/// 
	std::unordered_map<std::pair<unsigned int, unsigned int>, bool> mJoystickButtonsReleased;
	/// 
	std::unordered_map<std::pair<unsigned int, unsigned int>, float> mJoystickAxisPosition;
};