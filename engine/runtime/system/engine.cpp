#include "engine.h"
#include "core/subsystem/simulation.h"
#include "core/serialization/serialization.h"
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
		auto logging_container = logging::get_mutable_logging_container();
		logging_container->add_sink(std::make_shared<logging::sinks::platform_sink_mt>());
		logging_container->add_sink(std::make_shared<logging::sinks::daily_file_sink_mt>("Log", 23, 59));

		auto logger = logging::create(APPLOG, logging_container);
		
		serialization::set_warning_logger([](const std::string& msg)
		{
			APPLOG_WARNING(msg);
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
        auto it = std::find_if(std::begin(windows), std::end(windows),[](const auto& window)
        {
            return window->has_focus();
        });

        if(it != std::end(windows))
        {
            _focused_window = *it;
        }

		on_frame_begin(dt);

		for (auto window : windows)
		{
			window->frame_begin();
			
			on_window_frame_begin(*window);

			bool has_focus = window->has_focus();

			mml::platform_event e;
			while (window->poll_event(e))
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
				_running = window->is_open();
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
