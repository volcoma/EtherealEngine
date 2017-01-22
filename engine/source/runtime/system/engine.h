#pragma once

#include "core/subsystem.h"
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
		using timepoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
		using clock = std::chrono::high_resolution_clock;
		using duration = std::chrono::high_resolution_clock::duration;

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
		//  Name : run_one_frame ()
		/// <summary>
		/// Perform on frame with specified fps, this will call update/render internally
		/// </summary>
		//-----------------------------------------------------------------------------
		void run_one_frame();
	
		//-----------------------------------------------------------------------------
		//  Name : set_min_fps ()
		/// <summary>
		/// Set minimum frames per second. If fps goes lower than this, time will appear to slow.
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void set_min_fps(unsigned);

		//-----------------------------------------------------------------------------
		//  Name : set_max_fps ()
		/// <summary>
		/// Set maximum frames per second. The engine will sleep if fps is higher than this.
		/// </summary>
		//-----------------------------------------------------------------------------
		void set_max_fps(unsigned);
	
		//-----------------------------------------------------------------------------
		//  Name : set_max_inactive_fps ()
		/// <summary>
		/// Set maximum frames per second when the application does not have input focus.
		/// </summary>
		//-----------------------------------------------------------------------------
		void set_max_inactive_fps(unsigned);
		
		//-----------------------------------------------------------------------------
		//  Name : set_time_smoothing_step ()
		/// <summary>
		/// Set how many frames to average for timestep smoothing.
		/// </summary>
		//-----------------------------------------------------------------------------
		void set_time_smoothing_step(unsigned);

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
		//  Name : get_time_since_launch ()
		/// <summary>
		/// Returns duration since launch.
		/// </summary>
		//-----------------------------------------------------------------------------
		duration get_time_since_launch() const;
		
		//-----------------------------------------------------------------------------
		//  Name : get_fps ()
		/// <summary>
		/// Returns frames per second.
		/// </summary>
		//-----------------------------------------------------------------------------
		unsigned get_fps() const;
	
		//-----------------------------------------------------------------------------
		//  Name : get_delta_time ()
		/// <summary>
		/// Returns the delta time in seconds.
		/// </summary>
		//-----------------------------------------------------------------------------
		std::chrono::duration<float> get_delta_time() const;

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
		/// minimum/maximum frames per second
		unsigned _min_fps, _max_fps, _max_inactive_fps;
		/// previous timesteps for smoothing in seconds
		std::vector<duration> _previous_timesteps;
		/// next frame timestep in seconds
		duration _timestep;
		/// how many frames to average for the smoothed timestep
		unsigned _smoothing_step;
		/// frame update timer
		timepoint _last_frame_timepoint;
		/// timepoint when we launched
		timepoint _launch_timepoint;
		/// exiting flag
		bool _running;
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
