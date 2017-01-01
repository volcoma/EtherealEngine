#pragma  once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "../system/sfml/Window.hpp"
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
	event<void(RenderWindow&, const uSize&)> on_resized;
	/// event triggered when window is closed.
	event<void(RenderWindow&)> on_closed;

	//-----------------------------------------------------------------------------
	//  Name : get_surface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::shared_ptr<FrameBuffer> get_surface() { return _surface; }

	//-----------------------------------------------------------------------------
	//  Name : get_surface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::shared_ptr<FrameBuffer> get_surface() const { return _surface; }

	//-----------------------------------------------------------------------------
	//  Name : prepare_surface (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void prepare_surface();

	//-----------------------------------------------------------------------------
	//  Name : frameBegin (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_begin();

	//-----------------------------------------------------------------------------
	//  Name : frameUpdate (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_update(float dt);

	//-----------------------------------------------------------------------------
	//  Name : frameRender (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_render();

	//-----------------------------------------------------------------------------
	//  Name : frameEnd (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_end();
	
	//-----------------------------------------------------------------------------
	//  Name : setMain ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_main(bool isMain) { _is_main = isMain; }

	//-----------------------------------------------------------------------------
	//  Name : isMain ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool is_main() const { return _is_main; }
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

	/// Render surface for this window.
	std::shared_ptr<FrameBuffer> _surface;
	///
	bool _is_main = false;
};