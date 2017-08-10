#pragma once

#include "subsystem.h"
#include <chrono>
#include <vector>

namespace core
{
//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : Simulation (Class)
/// <summary>
/// Class responsible for timers.
/// </summary>
//-----------------------------------------------------------------------------
struct simulation : public subsystem
{
	using timepoint_t = std::chrono::time_point<std::chrono::high_resolution_clock>;
	using clock_t = std::chrono::high_resolution_clock;
	using duration_t = std::chrono::high_resolution_clock::duration;

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
	/// Perform on frame computations with specified fps
	/// </summary>
	//-----------------------------------------------------------------------------
	void run_one_frame();

	//-----------------------------------------------------------------------------
	//  Name : get_frame ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const std::uint64_t& get_frame() const
	{
		return _frame;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_min_fps ()
	/// <summary>
	/// Set minimum frames per second. If fps goes lower than this, time will
	/// appear to slow.
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_min_fps(unsigned);

	//-----------------------------------------------------------------------------
	//  Name : set_max_fps ()
	/// <summary>
	/// Set maximum frames per second. The engine will sleep if fps is higher than
	/// this.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_max_fps(unsigned int);

	//-----------------------------------------------------------------------------
	//  Name : set_max_inactive_fps ()
	/// <summary>
	/// Set maximum frames per second when the application does not have input
	/// focus.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_max_inactive_fps(unsigned int);

	//-----------------------------------------------------------------------------
	//  Name : set_time_smoothing_step ()
	/// <summary>
	/// Set how many frames to average for timestep smoothing.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_time_smoothing_step(unsigned int);

	//-----------------------------------------------------------------------------
	//  Name : get_time_since_launch ()
	/// <summary>
	/// Returns duration since launch.
	/// </summary>
	//-----------------------------------------------------------------------------
	duration_t get_time_since_launch() const;

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

protected:
	/// minimum/maximum frames per second
	unsigned int _min_fps = 0;
	///
	unsigned int _max_fps = 60;
	///
	unsigned int _max_inactive_fps = 60;
	/// previous time steps for smoothing in seconds
	std::vector<duration_t> _previous_timesteps;
	/// next frame time step in seconds
	duration_t _timestep = duration_t::zero();
	/// current frame
	std::uint64_t _frame = 0;
	/// how many frames to average for the smoothed time step
	unsigned int _smoothing_step = 11;
	/// frame update timer
	timepoint_t _last_frame_timepoint = clock_t::now();
	/// time point when we launched
	timepoint_t _launch_timepoint = clock_t::now();
};
}
