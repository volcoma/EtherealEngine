#pragma once

#include "core/logging/logging.h"
#include "core/signals/event.hpp"
#include "core/system/subsystem.h"

#include <chrono>
#include <vector>

class render_window;
namespace runtime
{
//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : engine (Class)
/// <summary>
/// Ethereal engine, creates the other subsystems.
/// </summary>
//-----------------------------------------------------------------------------
struct engine : public core::subsystem
{
	//-----------------------------------------------------------------------------
	//  Name : initialize ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	bool initialize() override;

	//-----------------------------------------------------------------------------
	//  Name : dispose ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void dispose() override;

	//-----------------------------------------------------------------------------
	//  Name : start ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	bool start(std::unique_ptr<render_window> main_window);

	//-----------------------------------------------------------------------------
	//  Name : run_one_frame ()
	/// <summary>
	/// Perform on frame with specified fps, this will call update/render
	/// internally
	/// </summary>
	//-----------------------------------------------------------------------------
	void run_one_frame();

	//-----------------------------------------------------------------------------
	//  Name : set_running ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_running(bool running)
	{
		_running = running;
	}

	//-----------------------------------------------------------------------------
	//  Name : is_running ()
	/// <summary>
	/// Returns if engine is exiting
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool is_running() const
	{
		return _running;
	}

	//-----------------------------------------------------------------------------
	//  Name : register_window ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void register_window(std::unique_ptr<render_window> window);

	//-----------------------------------------------------------------------------
	//  Name : register_main_window ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void register_main_window(std::unique_ptr<render_window> window);

	//-----------------------------------------------------------------------------
	//  Name : get_windows ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const std::vector<std::unique_ptr<render_window>>& get_windows() const
	{
		return _windows;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_focused_window ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	render_window* get_focused_window() const;

protected:
	void process_pending_events();
	void process_pending_windows();
	/// exiting flag
	bool _running = false;
	/// engine windows
	std::vector<std::unique_ptr<render_window>> _windows;
	std::vector<std::unique_ptr<render_window>> _windows_pending_addition;
};

/// engine events
extern event<void(std::chrono::duration<float>)> on_frame_begin;
extern event<void(std::chrono::duration<float>)> on_frame_update;
extern event<void(std::chrono::duration<float>)> on_frame_render;
extern event<void(std::chrono::duration<float>)> on_frame_end;

extern event<void(const render_window&)> on_window_frame_begin;
extern event<void(const render_window&)> on_window_frame_update;
extern event<void(const render_window&)> on_window_frame_render;
extern event<void(const render_window&)> on_window_frame_end;
}
