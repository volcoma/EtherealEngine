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
	//-----------------------------------------------------------------------------
	//  Name : RenderWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderWindow();

	//-----------------------------------------------------------------------------
	//  Name : RenderWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style = sf::Style::Default);

	//-----------------------------------------------------------------------------
	//  Name : ~RenderWindow (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~RenderWindow();

	/// event triggered when window is resized.
	event<void(RenderWindow&, const uSize&)> onResized;
	/// event triggered when window is closed.
	event<void(RenderWindow&)> onClosed;

	//-----------------------------------------------------------------------------
	//  Name : getRenderView ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::shared_ptr<RenderView> getRenderView() { return mView; }

	//-----------------------------------------------------------------------------
	//  Name : getRenderView ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::shared_ptr<RenderView> getRenderView() const { return mView; }

	//-----------------------------------------------------------------------------
	//  Name : getInput ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline InputContext& getInput() { return mInput; }

	//-----------------------------------------------------------------------------
	//  Name : getInput ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const InputContext& getInput() const { return mInput; }

	//-----------------------------------------------------------------------------
	//  Name : prepareView (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void prepareView();

	//-----------------------------------------------------------------------------
	//  Name : destroyView (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void destroyView();

	//-----------------------------------------------------------------------------
	//  Name : frameBegin (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameBegin();

	//-----------------------------------------------------------------------------
	//  Name : frameUpdate (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameUpdate(float dt);

	//-----------------------------------------------------------------------------
	//  Name : frameRender (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameRender();

	//-----------------------------------------------------------------------------
	//  Name : frameEnd (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameEnd();

	//-----------------------------------------------------------------------------
	//  Name : delayedClose (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void delayedClose();

protected:
	//-----------------------------------------------------------------------------
	//  Name : filterEvent (virtual )
	/// <summary>
	/// This function is called every time an event is received
	/// from the internal window (through pollEvent or waitEvent).
	/// It filters out unwanted events, and performs whatever internal
	/// stuff the window needs before the event is returned to the
	/// user.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool filterEvent(const sf::Event& event);

	//-----------------------------------------------------------------------------
	//  Name : onResize (virtual )
	/// <summary>
	/// This function is called so that derived classes can
	/// perform custom actions when the size of the window changes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void onResize();

	//-----------------------------------------------------------------------------
	//  Name : onClose (virtual )
	/// <summary>
	/// This function is called so that derived classes can
	/// perform custom actions when the window is closed
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void onClose();

	/// Input Context for this window.
	InputContext mInput;

	/// Render view for this window.
	std::shared_ptr<RenderView> mView;
};