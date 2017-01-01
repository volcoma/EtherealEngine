#pragma once

#include "core/subsystem.h"
#include "core/events/event.hpp"
#include "core/logging/logging.h"

#include <chrono>
#include <vector>

class RenderWindow;
namespace runtime
{

	// ethereal engine, creates the other subsystems
	struct Engine : public core::Subsystem
	{
		using timepoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
		using clock = std::chrono::high_resolution_clock;
		using duration = std::chrono::high_resolution_clock::duration;

		bool initialize() override;
		void dispose() override;

		// perform on frame with specified fps, this will call update/render internally
		void run_one_frame();
		// set minimum frames per second. if fps goes lower than this, time will appear to slow.
		void set_min_fps(unsigned);
		// set maximum frames per second. the engine will sleep if fps is higher than this.
		void set_max_fps(unsigned);
		// set maximum frames per second when the application does not have input focus
		void set_max_inactive_fps(unsigned);
		// set how many frames to average for timestep smoothing.
		void set_time_smoothing_step(unsigned);
		// returns if engine is exiting
		bool is_running() const { return _running; }
		// returns duration since lemon-toolkit launched
		duration get_time_since_launch() const;
		// returns frames per second
		unsigned get_fps() const;
		// returns the delta time in seconds
		std::chrono::duration<float> get_delta_time() const;

		void register_window(std::shared_ptr<RenderWindow> window);

		inline const std::vector<std::shared_ptr<RenderWindow>>& get_windows() const { return _windows; }

		RenderWindow& get_window() { return *_window; }
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
		std::shared_ptr<RenderWindow> _window;
	};

	extern event<void(std::chrono::duration<float>)> on_frame_begin;
	extern event<void(std::chrono::duration<float>)> on_frame_update;
	extern event<void(std::chrono::duration<float>)> on_frame_render;
	extern event<void(std::chrono::duration<float>)> on_frame_end;
}
