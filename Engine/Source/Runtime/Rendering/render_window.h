#pragma  once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "../system/SFML/Window.hpp"
#include "../input/input_context.h"
#include "core/events/event.hpp"

struct FrameBuffer;
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
	inline std::shared_ptr<FrameBuffer> getRenderSurface() { return mSurface; }

	//-----------------------------------------------------------------------------
	//  Name : getRenderView ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::shared_ptr<FrameBuffer> getRenderSurface() const { return mSurface; }

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
	//  Name : prepareSurface (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void prepareSurface();

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
	//  Name : setMain ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setMain(bool isMain) { mIsMain = isMain; }

	//-----------------------------------------------------------------------------
	//  Name : isMain ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool isMain() const { return mIsMain; }
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
	/// Render surface for this window.
	std::shared_ptr<FrameBuffer> mSurface;
	///
	bool mIsMain = false;
};