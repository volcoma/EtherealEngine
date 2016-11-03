#pragma  once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "../System/SFML/Window.hpp"
#include "../Input/InputContext.h"
#include "Core/events/event.hpp"

class RenderView;
class RenderWindow : public sf::Window
{
public:
	RenderWindow();
	RenderWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style = sf::Style::Default);
	virtual ~RenderWindow();

	////////////////////////////////////////////////////////////
	/// \brief Function called after the window has been resized
	///
	/// This function is called so that derived classes can
	/// perform custom actions when the size of the window changes.
	///
	////////////////////////////////////////////////////////////
	event<void(RenderWindow&, const uSize&)> onResized;

	////////////////////////////////////////////////////////////
	/// \brief Function called after the window has been closed
	///
	/// This function is called so that derived classes can
	/// perform custom actions when the window is closed
	///
	////////////////////////////////////////////////////////////
	event<void(RenderWindow&)> onClosed;
	
	inline std::shared_ptr<RenderView> getRenderView() { return mView; }
	inline std::shared_ptr<RenderView> getRenderView() const { return mView; }

	inline InputContext& getInput() { return mInputManager; }
	inline const InputContext& getInput() const { return mInputManager; }

	virtual void prepareView();
	virtual void destroyView();
	virtual void frameBegin();
	virtual void frameUpdate(float dt);
	virtual void frameRender();
	virtual void frameEnd();

protected:
	////////////////////////////////////////////////////////////
	/// \brief Processes an event before it is sent to the user
	///
	/// This function is called every time an event is received
	/// from the internal window (through pollEvent or waitEvent).
	/// It filters out unwanted events, and performs whatever internal
	/// stuff the window needs before the event is returned to the
	/// user.
	///
	/// \param event Event to filter
	///
	////////////////////////////////////////////////////////////
	virtual bool filterEvent(const sf::Event& event);
    ////////////////////////////////////////////////////////////
    /// \brief Function called after the window has been resized
    ///
    /// This function is called so that derived classes can
    /// perform custom actions when the size of the window changes.
    ///
    ////////////////////////////////////////////////////////////
    virtual void onResize();

	////////////////////////////////////////////////////////////
	/// \brief Function called after the window has been closed
	///
	/// This function is called so that derived classes can
	/// perform custom actions when the window is closed
	///
	////////////////////////////////////////////////////////////
	virtual void onClose();

	InputContext mInputManager;
	std::shared_ptr<RenderView> mView;	
};