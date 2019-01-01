#include "simulation.h"
#include <algorithm>
#include <thread>

namespace core
{
using namespace std::chrono_literals;

simulation::simulation()
{
	if(max_inactive_fps_ == 0)
	{
		max_inactive_fps_ = std::max(max_inactive_fps_, max_fps_);
	}
}

void simulation::run_one_frame(bool is_active)
{
	// perform waiting loop if maximum fps set
	auto max_fps = max_fps_;
	if(!is_active && max_fps > 0)
	{
		max_fps = std::min(max_inactive_fps_, max_fps);
	}

	duration_t elapsed = clock_t::now() - last_frame_timepoint_;
	if(max_fps > 0)
	{
		duration_t target_duration = 1000ms / max_fps;

		for(;;)
		{
			elapsed = clock_t::now() - last_frame_timepoint_;
			if(elapsed >= target_duration)
			{
				break;
			}

			if(elapsed < duration_t(0))
			{
				break;
			}
			duration_t sleep_time = (target_duration - elapsed);
			auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(sleep_time);

			if(sleep_time > std::chrono::microseconds(1000))
			{
				if(ms.count() > 0)
				{
					sleep_time /= ms.count();
				}

				std::this_thread::sleep_for(sleep_time);
			}
		}
	}

	if(elapsed < duration_t(0))
	{
		elapsed = duration_t(0);
	}
	last_frame_timepoint_ = clock_t::now();

	// if fps lower than minimum, clamp eplased time
	if(min_fps_ > 0)
	{
		duration_t target_duration = 1000ms / min_fps_;
		if(elapsed > target_duration)
		{
			elapsed = target_duration;
		}
	}

	// perform time step smoothing
	if(smoothing_step_ > 0)
	{
		timestep_ = duration_t::zero();
		previous_timesteps_.push_back(elapsed);
		if(previous_timesteps_.size() > smoothing_step_)
		{
			auto begin = previous_timesteps_.begin();
			previous_timesteps_.erase(begin, begin + int(previous_timesteps_.size() - smoothing_step_));
			for(auto step : previous_timesteps_)
			{
				timestep_ += step;
			}
			timestep_ /= static_cast<duration_t::rep>(previous_timesteps_.size());
		}
		else
		{
			timestep_ = previous_timesteps_.back();
		}
	}
	else
	{
		timestep_ = elapsed;
	}

	++frame_;
}

void simulation::set_min_fps(std::uint32_t fps)
{
	min_fps_ = std::max<std::uint32_t>(fps, 0);
}

void simulation::set_max_fps(std::uint32_t fps)
{
	max_fps_ = std::max<std::uint32_t>(fps, 0);
}

void simulation::set_max_inactive_fps(std::uint32_t fps)
{
	max_inactive_fps_ = std::max<std::uint32_t>(fps, 0);
}

void simulation::set_time_smoothing_step(std::uint32_t step)
{
	smoothing_step_ = step;
}

simulation::duration_t simulation::get_time_since_launch() const
{
	return clock_t::now() - launch_timepoint_;
}

std::uint32_t simulation::get_fps() const
{
	auto dt = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(timestep_).count();
	return static_cast<std::uint32_t>(dt == 0.0f ? 0 : 1000.0f / dt);
}

std::chrono::duration<float> simulation::get_delta_time() const
{
	auto dt = std::chrono::duration_cast<std::chrono::duration<float>>(timestep_);
	return dt;
}
}
