#include "simulation.h"
#include <algorithm>
#include <thread>

namespace core
{
using namespace std::literals;

simulation::simulation()
{
	if(_max_inactive_fps == 0)
	{
		_max_inactive_fps = std::max(_max_inactive_fps, _max_fps);
	}
}

void simulation::run_one_frame(bool is_active)
{
	// perform waiting loop if maximum fps set
	auto max_fps = _max_fps;
	if(!is_active && max_fps > 0)
	{
		max_fps = std::min(_max_inactive_fps, max_fps);
	}

	duration_t elapsed = clock_t::now() - _last_frame_timepoint;
	if(max_fps > 0)
	{
		duration_t target_duration = 1000ms / max_fps;

		for(;;)
		{
			elapsed = clock_t::now() - _last_frame_timepoint;
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
	_last_frame_timepoint = clock_t::now();

	// if fps lower than minimum, clamp eplased time
	if(_min_fps > 0)
	{
		duration_t target_duration = 1000ms / _min_fps;
		if(elapsed > target_duration)
		{
			elapsed = target_duration;
		}
	}

	// perform time step smoothing
	if(_smoothing_step > 0)
	{
		_timestep = duration_t::zero();
		_previous_timesteps.push_back(elapsed);
		if(_previous_timesteps.size() > _smoothing_step)
		{
			auto begin = _previous_timesteps.begin();
			_previous_timesteps.erase(begin, begin + int(_previous_timesteps.size() - _smoothing_step));
			for(auto step : _previous_timesteps)
			{
				_timestep += step;
			}
			_timestep /= static_cast<duration_t::rep>(_previous_timesteps.size());
		}
		else
		{
			_timestep = _previous_timesteps.back();
		}
	}
	else
	{
		_timestep = elapsed;
	}

	++_frame;
}

void simulation::set_min_fps(unsigned int fps)
{
	_min_fps = std::max<unsigned int>(fps, 0);
}

void simulation::set_max_fps(unsigned int fps)
{
	_max_fps = std::max<unsigned int>(fps, 0);
}

void simulation::set_max_inactive_fps(unsigned int fps)
{
	_max_inactive_fps = std::max<unsigned int>(fps, 0);
}

void simulation::set_time_smoothing_step(unsigned int step)
{
	_smoothing_step = step;
}

simulation::duration_t simulation::get_time_since_launch() const
{
	return clock_t::now() - _launch_timepoint;
}

unsigned simulation::get_fps() const
{
	auto dt = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(_timestep).count();
	return static_cast<unsigned>(dt == 0.0f ? 0 : 1000.0f / dt);
}

std::chrono::duration<float> simulation::get_delta_time() const
{
	auto dt = std::chrono::duration_cast<std::chrono::duration<float>>(_timestep);
	return dt;
}
}
