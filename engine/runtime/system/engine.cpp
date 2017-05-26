#include "engine.h"
#include "core/subsystem/simulation.h"
#include "../rendering/renderer.h"
#include "../input/input.h"
#include "../ecs/ecs.h"
#include "../ecs/systems/scene_graph.h"
#include "../ecs/systems/camera_system.h"
#include "../ecs/systems/deferred_rendering.h"
#include "../rendering/render_window.h"
#include "../assets/asset_manager.h"
#include "task.h"

namespace runtime
{
	event<void(std::chrono::duration<float>)> on_frame_begin;
	event<void(std::chrono::duration<float>)> on_frame_update;
	event<void(std::chrono::duration<float>)> on_frame_render;
	event<void(std::chrono::duration<float>)> on_frame_end;

	event<void(const render_window&)> on_window_frame_begin;
	event<void(const render_window&)> on_window_frame_update;
	event<void(const render_window&)> on_window_frame_render;
	event<void(const render_window&)> on_window_frame_end;

	bool engine::initialize()
	{
		auto logger = logging::create(APPLOG,
		{
			std::make_shared<logging::sinks::platform_sink_mt>(),
			std::make_shared<logging::sinks::daily_file_sink_mt>("Log", "log", 23, 59),
		});
		
		// fire engine
		_running = true;

		return true;
	}

	void engine::dispose()
	{

	}


	bool engine::start(std::shared_ptr<render_window> main_window)
	{
		core::add_subsystem<core::simulation>();

		auto& render = core::add_subsystem<renderer>();
		if (!render.init_backend(*main_window))
		{
			APPLOG_ERROR("Could not initialize rendering backend!");
			return false;
		}
		register_window(main_window);
		core::add_subsystem<input>();
		core::add_subsystem<asset_manager>();
		core::add_subsystem<entity_component_system>();
		core::add_subsystem<task_system>();
		core::add_subsystem<scene_graph>();
		core::add_subsystem<camera_system>();
		core::add_subsystem<deferred_rendering>();

		return true;
	}


	void engine::run_one_frame()
	{
		if (!_running)
			return;

		auto& sim = core::get_subsystem<core::simulation>();
		auto& inp = core::get_subsystem<input>();

		sim.run_one_frame();
		auto dt = sim.get_delta_time();

		_focused_window = nullptr;
		//get a copy of the windows for safe iterator invalidation
		auto windows = get_windows();
		for (auto window : windows)
		{
			if (window->hasFocus())
			{
				_focused_window = window;
				break;
			}
		}

		on_frame_begin(dt);

		for (auto window : windows)
		{
			window->frame_begin();
			
			on_window_frame_begin(*window);

			bool has_focus = window->hasFocus();

			sf::Event e;
			while (window->pollEvent(e))
			{
				if (has_focus)
				{
					inp.handle_event(e);
				}
			}

			window->frame_update(dt);
			on_window_frame_update(*window);
		}

		on_frame_update(dt);

		on_frame_render(dt);

		for (auto window : windows)
		{
			window->frame_render(dt);
			on_window_frame_render(*window);

			window->frame_end();
			on_window_frame_end(*window);

			if (window->is_main())
				_running = window->isOpen();
		}

		on_frame_end(dt);
	}
	
	void engine::register_window(std::shared_ptr<render_window> window)
	{
		auto on_closed = [this](render_window& wnd)
		{
			_windows.erase(std::remove_if(std::begin(_windows), std::end(_windows),
				[this, &wnd](std::shared_ptr<render_window>& other)
			{
				return (&wnd == other.get());
			}), std::end(_windows));

			if (&wnd == _focused_window.get())
				_focused_window.reset();
		};

		window->on_closed.connect(on_closed);
		window->prepare_surface();
		_windows.push_back(window);
	}

	void engine::destroy_windows()
	{
		_windows.clear();
		_focused_window.reset();
	}

}