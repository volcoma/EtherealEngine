#pragma once

#include "core/subsystem/subsystem.h"
#include "core/events/event.hpp"
#include "core/logging/logging.h"

#include <chrono>
#include <vector>

class RenderWindow;
namespace runtime
{
	//-----------------------------------------------------------------------------
	// Main Class Declarations
	//-----------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : Engine (Class)
	/// <summary>
	/// Ethereal engine, creates the other subsystems.
	/// </summary>
	//-----------------------------------------------------------------------------
	struct Engine : public core::Subsystem
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
		//  Name : destroy_windows ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void destroy_windows();

		//-----------------------------------------------------------------------------
		//  Name : start ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool start(std::shared_ptr<RenderWindow> main_window);

		//-----------------------------------------------------------------------------
		//  Name : run_one_frame ()
		/// <summary>
		/// Perform on frame with specified fps, this will call update/render internally
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
		inline void set_running(bool running) { _running = running; }

		//-----------------------------------------------------------------------------
		//  Name : is_running ()
		/// <summary>
		/// Returns if engine is exiting
		/// </summary>
		//-----------------------------------------------------------------------------
		inline bool is_running() const { return _running; }

		//-----------------------------------------------------------------------------
		//  Name : register_window ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void register_window(std::shared_ptr<RenderWindow> window);

		//-----------------------------------------------------------------------------
		//  Name : get_windows ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		inline const std::vector<std::shared_ptr<RenderWindow>>& get_windows() const { return _windows; }

		//-----------------------------------------------------------------------------
		//  Name : get_focused_window ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		RenderWindow* get_focused_window() { return _focused_window.get(); }

	protected:
		/// exiting flag
		bool _running = false;
		/// engine windows
		std::vector<std::shared_ptr<RenderWindow>> _windows;
		/// currently processed window
		std::shared_ptr<RenderWindow> _focused_window;
	};

	/// engine events
	extern event<void(std::chrono::duration<float>)> on_frame_begin;
	extern event<void(std::chrono::duration<float>)> on_frame_update;
	extern event<void(std::chrono::duration<float>)> on_frame_render;
	extern event<void(std::chrono::duration<float>)> on_frame_end;
}
