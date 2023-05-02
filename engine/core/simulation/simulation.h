#pragma once

#include <chrono>
#include <vector>

namespace core
{

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : simulation (Class)
/// <summary>
/// Class responsible for timers.
/// </summary>
//-----------------------------------------------------------------------------
struct simulation
{
    using clock_t = std::chrono::steady_clock;
    using timepoint_t = clock_t::time_point;
    using duration_t = clock_t::duration;

    simulation();

    //-----------------------------------------------------------------------------
    //  Name : run_one_frame ()
    /// <summary>
    /// Perform on frame computations with specified fps
    /// </summary>
    //-----------------------------------------------------------------------------
    void run_one_frame(bool is_active);

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
        return frame_;
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
    void set_min_fps(std::uint32_t fps);

    //-----------------------------------------------------------------------------
    //  Name : set_max_fps ()
    /// <summary>
    /// Set maximum frames per second. The engine will sleep if fps is higher than
    /// this.
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_max_fps(std::uint32_t fps);

    //-----------------------------------------------------------------------------
    //  Name : set_max_inactive_fps ()
    /// <summary>
    /// Set maximum frames per second when the application does not have input
    /// focus.
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_max_inactive_fps(std::uint32_t fps);

    //-----------------------------------------------------------------------------
    //  Name : set_time_smoothing_step ()
    /// <summary>
    /// Set how many frames to average for timestep smoothing.
    /// </summary>
    //-----------------------------------------------------------------------------
    void set_time_smoothing_step(std::uint32_t step);

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
    std::uint32_t get_fps() const;

    //-----------------------------------------------------------------------------
    //  Name : get_delta_time ()
    /// <summary>
    /// Returns the delta time in seconds.
    /// </summary>
    //-----------------------------------------------------------------------------
    std::chrono::duration<float> get_delta_time() const;

protected:
    /// minimum/maximum frames per second
    std::uint32_t min_fps_ = 0;
    ///
    std::uint32_t max_fps_ = 200;
    ///
    std::uint32_t max_inactive_fps_ = 20;
    /// previous time steps for smoothing in seconds
    std::vector<duration_t> previous_timesteps_;
    /// next frame time step in seconds
    duration_t timestep_ = duration_t::zero();
    /// current frame
    std::uint64_t frame_ = 0;
    /// how many frames to average for the smoothed time step
    std::uint32_t smoothing_step_ = 11;
    /// frame update timer
    timepoint_t last_frame_timepoint_ = clock_t::now();
    /// time point when we launched
    timepoint_t launch_timepoint_ = clock_t::now();
};
} // namespace core
