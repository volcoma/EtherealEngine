#pragma once

////////////////////////////////////////////////////////////
// Headers
////////////////////////////////////////////////////////////
#include "core/common/basetypes.hpp"
#include "core/common/nonstd/type_traits.hpp"
#include "core/signals/event.hpp"
#include "mml/window/window.hpp"
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
	inline std::shared_ptr<frame_buffer> get_surface()
	{
		return _surface;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_surface ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::shared_ptr<frame_buffer> get_surface() const
	{
		return _surface;
	}

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
	//  Name : frame_begin (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_begin()
	{
	}

	//-----------------------------------------------------------------------------
	//  Name : frame_update (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_update(std::chrono::duration<float>)
	{
	}

	//-----------------------------------------------------------------------------
	//  Name : frame_render (virtual )
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_render(std::chrono::duration<float>)
	{
	}

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
	inline void set_main(bool isMain)
	{
		_is_main = isMain;
	}

	//-----------------------------------------------------------------------------
	//  Name : is_main ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool is_main() const
	{
		return _is_main;
	}

	std::size_t get_id() const
	{
		return _id;
	}

protected:
	//-----------------------------------------------------------------------------
	//  Name : on_resize (virtual )
	/// <summary>
	/// This function is called so that derived classes can
	/// perform custom actions when the size of the window changes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_resize();

	///
	std::size_t _id = rtti::type_index_sequential_t::id<mml::window, render_window>();
	/// Render surface for this window.
	std::shared_ptr<frame_buffer> _surface;
	///
	bool _is_main = false;
};
