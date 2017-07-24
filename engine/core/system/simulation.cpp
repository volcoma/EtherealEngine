#include "simulation.h"
#include <algorithm>
#include <thread>

namespace core
{

bool simulation::initialize()
{
	_timestep = duration_t::zero();
	_last_frame_timepoint = clock_t::now();
	_launch_timepoint = clock_t::now();

	if(_max_inactive_fps == 0)
		_max_inactive_fps = std::max(_max_inactive_fps, _max_fps);

	return true;
}

void simulation::dispose()
{
}

void simulation::run_one_frame()
{
	// perform waiting loop if maximum fps set
	auto max_fps = _max_fps;
	if(max_fps > 0)
		max_fps = std::min(_max_inactive_fps, max_fps);

	if(max_fps > 0)
	{
		duration_t target_duration = std::chrono::milliseconds(1000) / max_fps;
		duration_t eplased = clock_t::now() - _last_frame_timepoint;
		for(;;)
		{
			eplased = clock_t::now() - _last_frame_timepoint;
			if(eplased > target_duration)
				break;

			if(target_duration - eplased > duration_t(1))
				std::this_thread::sleep_for((target_duration - eplased) - duration_t(1));
		}
	}

	duration_t eplased = clock_t::now() - _last_frame_timepoint;
	_last_frame_timepoint = clock_t::now();

	// if fps lower than minimum, clamp eplased time
	if(_min_fps > 0)
	{
		duration_t target_duration = std::chrono::milliseconds(1000) / _min_fps;
		if(eplased > target_duration)
			eplased = target_duration;
	}

	// perform time step smoothing
	if(_smoothing_step > 0)
	{
		_timestep = duration_t::zero();
		_previous_timesteps.push_back(eplased);
		if(_previous_timesteps.size() > _smoothing_step)
		{
			auto begin = _previous_timesteps.begin();
			_previous_timesteps.erase(begin, begin + int(_previous_timesteps.size() - _smoothing_step));
			for(auto step : _previous_timesteps)
				_timestep += step;
			_timestep /= _previous_timesteps.size();
		}
		else
			_timestep = _previous_timesteps.back();
	}
	else
	{
		_timestep = eplased;
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
