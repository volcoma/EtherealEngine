#pragma  once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "mml/window/window.hpp"
#include "core/signals/event.hpp"
#include "core/common/basetypes.hpp"
#include <chrono>

struct frame_buffer;
class render_window : public mml::window
{
public:
	//-----------------------------------------------------------------------------
	//  Name : render_window ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	render_window();

	//-----------------------------------------------------------------------------
	//  Name : render_window ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	render_window(mml::video_mode mode, const std::string& title, std::uint32_t style = mml::style::Default);

	//-----------------------------------------------------------------------------
	//  Name : ~render_window (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~render_window();

	/// event triggered when window is resized.
	event<void(render_window&, const usize&)> on_resized;
	/// event triggered when window is closed.
	event<void(render_window&)> on_closed;

	//-----------------------------------------------------------------------------
	//  Name : get_surface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::shared_ptr<frame_buffer> get_surface() { return _surface; }

	//-----------------------------------------------------------------------------
	//  Name : get_surface ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::shared_ptr<frame_buffer> get_surface() const { return _surface; }

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
	//  Name : frame_begin (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_begin() {}

	//-----------------------------------------------------------------------------
	//  Name : frame_update (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_update(std::chrono::duration<float> dt) {}

	//-----------------------------------------------------------------------------
	//  Name : frame_render (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_render(std::chrono::duration<float> dt) {}

	//-----------------------------------------------------------------------------
	//  Name : frame_end (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_end();
	
	//-----------------------------------------------------------------------------
	//  Name : set_main ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_main(bool isMain) { _is_main = isMain; }

	//-----------------------------------------------------------------------------
	//  Name : is_main ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool is_main() const { return _is_main; }
protected:

	//-----------------------------------------------------------------------------
	//  Name : on_resize (virtual )
	/// <summary>
	/// This function is called so that derived classes can
	/// perform custom actions when the size of the window changes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_resize();

	virtual bool filter_event(const mml::platform_event& event);

	/// Render surface for this window.
	std::shared_ptr<frame_buffer> _surface;
	///
	bool _is_main = false;
};