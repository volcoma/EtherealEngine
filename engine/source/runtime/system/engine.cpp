#include "engine.h"
#include "renderer.h"
#include "Input/input.h"
#include "core/ecs.h"
#include "core/task.h"
#include "core/logging/logging.h"
#include "ecs/systems/transform_system.h"
#include "ecs/systems/camera_system.h"
#include "ecs/systems/rendering_system.h"
#include "rendering/render_window.h"
#include "assets/asset_manager.h"

namespace runtime
{
	event<void(std::chrono::duration<float>)> on_frame_begin;
	event<void(std::chrono::duration<float>)> on_frame_update;
	event<void(std::chrono::duration<float>)> on_frame_render;
	event<void(std::chrono::duration<float>)> on_frame_end;

	bool Engine::initialize()
	{
		auto logger = logging::create("Log",
		{
			std::make_shared<logging::sinks::platform_sink_mt>(),
			std::make_shared<logging::sinks::daily_file_sink_mt>("Log", "log", 23, 59),
		});

		core::add_subsystem<Input>();
		core::add_subsystem<Renderer>();
		core::add_subsystem<AssetManager>();
		core::add_subsystem<core::EntityComponentSystem>();
		core::add_subsystem<core::TaskSystem>();
		core::add_subsystem<TransformSystem>();
		core::add_subsystem<CameraSystem>();
		core::add_subsystem<RenderingSystem>();
		// fire engine
		_running = true;
		_timestep = duration::zero();
		_last_frame_timepoint = clock::now();
		_launch_timepoint = clock::now();

		_max_inactive_fps = 0;
		_min_fps = 0;
		_max_fps = 0;
		_smoothing_step = 11;

		if (_max_inactive_fps == 0)
			_max_inactive_fps = std::max(_max_inactive_fps, _max_fps);

		return true;
	}

	void Engine::dispose()
	{
		
	}

	void Engine::run_one_frame()
	{
		if (!_running)
			return;

		// perform waiting loop if maximum fps set
		auto max_fps = _max_fps;
		if (max_fps > 0)
			max_fps = std::min(_max_inactive_fps, max_fps);

		if (max_fps > 0)
		{
			duration target_duration = std::chrono::milliseconds(1000) / max_fps;
			duration eplased = clock::now() - _last_frame_timepoint;
			for (;; )
			{
				eplased = clock::now() - _last_frame_timepoint;
				if (eplased > target_duration)
					break;

				if (target_duration - eplased > duration(1))
					std::this_thread::sleep_for((target_duration - eplased) - duration(1));
			}
		}

		duration eplased = clock::now() - _last_frame_timepoint;
		_last_frame_timepoint = clock::now();

		// if fps lower than minimum, clamp eplased time
		if (_min_fps > 0)
		{
			duration target_duration = std::chrono::milliseconds(1000) / _min_fps;
			if (eplased > target_duration)
				eplased = target_duration;
		}

		// perform timestep smoothing
		if (_smoothing_step > 0)
		{
			_timestep = duration::zero();
			_previous_timesteps.push_back(eplased);
			if (_previous_timesteps.size() > _smoothing_step)
			{
				auto begin = _previous_timesteps.begin();
				_previous_timesteps.erase(begin, begin + _previous_timesteps.size() - _smoothing_step);
				for (auto step : _previous_timesteps)
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
		
		auto ts = core::get_subsystem<core::TaskSystem>();
		ts->execute_tasks_on_main();

		auto dt = get_delta_time();
		auto input = core::get_subsystem<Input>();
		input->update();

		//get a copy of the windows for safe iterator invalidation
		auto windows = get_windows();
		for (auto window : windows)
		{
			_window = window;
			bool focused = window->hasFocus();
			window->frameBegin();

			sf::Event e;
			while (window->pollEvent(e))
			{
				if (focused)
					input->handle_event(e);
			}

			if (focused)
				on_frame_begin(dt);

			window->frameUpdate(dt.count());
			if (focused)
				on_frame_update(dt);

			window->frameRender();
			if (focused)
				on_frame_render(dt);
			
			window->frameEnd();
			if (focused)
				on_frame_end(dt);

			if (window->isMain())
				_running = window->isOpen();
		}

		auto renderer = core::get_subsystem<runtime::Renderer>();
		renderer->frame_end();
	}
	
	void Engine::set_min_fps(unsigned fps)
	{
		_min_fps = std::max(fps, (unsigned)0);
	}

	void Engine::set_max_fps(unsigned fps)
	{
		_max_fps = std::max(fps, (unsigned)0);
	}

	void Engine::set_max_inactive_fps(unsigned fps)
	{
		_max_inactive_fps = std::max(fps, (unsigned)0);
	}

	void Engine::set_time_smoothing_step(unsigned step)
	{
		_smoothing_step = step;
	}

	Engine::duration Engine::get_time_since_launch() const
	{
		return clock::now() - _launch_timepoint;
	}

	unsigned Engine::get_fps() const
	{
		auto dt = std::chrono::duration_cast<std::chrono::duration<float, std::milli>>(_timestep).count();
		return static_cast<unsigned>(dt == 0.0f ? 0 : 1000.0f / dt);
	}

	std::chrono::duration<float> Engine::get_delta_time() const
	{
		auto dt = std::chrono::duration_cast<std::chrono::duration<float>>(_timestep);
		return dt;
	}

	void Engine::register_window(std::shared_ptr<RenderWindow> window)
	{
		auto onClosed = [this](RenderWindow& wnd)
		{
			_windows.erase(std::remove_if(std::begin(_windows), std::end(_windows),
				[this, &wnd](std::shared_ptr<RenderWindow>& other)
			{
				return (&wnd == other.get());
			}), std::end(_windows));
		};

		window->onClosed.addListener(onClosed);
		window->prepareSurface();
		_windows.push_back(window);
	}

}