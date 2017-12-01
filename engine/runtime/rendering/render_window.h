#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "core/common/basetypes.hpp"
#include "core/common/nonstd/type_traits.hpp"
#include "core/signals/event.hpp"
#include "mml/window/window.hpp"
#include <chrono>
namespace gfx
{
struct frame_buffer;
}

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
	render_window(mml::video_mode mode, const std::string& title, std::uint32_t style = mml::style::standard);

	//-----------------------------------------------------------------------------
	//  Name : ~render_window (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~render_window();

	//-----------------------------------------------------------------------------
	//  Name : get_surface ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<gfx::frame_buffer> get_surface();

	//-----------------------------------------------------------------------------
	//  Name : get_surface ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<gfx::frame_buffer> get_surface() const;

	//-----------------------------------------------------------------------------
	//  Name : frame_end (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual std::uint8_t begin_present_pass();

	std::uint32_t get_id() const;

protected:
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
	//  Name : destroy_surface (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void destroy_surface();
	//-----------------------------------------------------------------------------
	//  Name : on_resize (virtual )
	/// <summary>
	/// This function is called so that derived classes can
	/// perform custom actions when the size of the window changes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_resize();

	///
	std::uint32_t _id = 0;
	/// Render surface for this window.
	std::shared_ptr<gfx::frame_buffer> _surface;
};
