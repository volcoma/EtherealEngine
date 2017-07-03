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


	bool engine::start(std::unique_ptr<render_window> main_window)
	{
		core::add_subsystem<core::simulation>();

		auto& render = core::add_subsystem<renderer>();
		if (!render.init_backend(*main_window))
		{
			APPLOG_ERROR("Could not initialize rendering backend!");
			return false;
		}
		register_main_window(std::move(main_window));
	
		core::add_subsystem<input>();
		core::add_subsystem<asset_manager>();
		core::add_subsystem<entity_component_system>();
		core::add_subsystem<task_system>();
		core::add_subsystem<scene_graph>();
		core::add_subsystem<camera_system>();
		core::add_subsystem<deferred_rendering>();

		return true;
	}

	render_window* engine::get_focused_window() const
	{
		render_window* focused_window = nullptr;
		//get a copy of the windows for safe iterator invalidation
		const auto& windows = get_windows();
		auto it = std::find_if(std::begin(windows), std::end(windows), [](const auto& window)
		{
			return window->has_focus();
		});

		if (it != std::end(windows))
		{
			focused_window = it->get();
		}

		return focused_window;
	}

	void engine::run_one_frame()
	{
		if (!_running)
			return;

		auto& sim = core::get_subsystem<core::simulation>();
		
		sim.run_one_frame();

		process_pending_windows();

		process_pending_events();
		
		auto dt = sim.get_delta_time();

		if (!_windows.empty())
		{		
			on_frame_begin(dt);

			for (auto& window : _windows)
			{
				window->frame_begin();
				on_window_frame_begin(*window);

				window->frame_update(dt);
				on_window_frame_update(*window);
			}

			on_frame_update(dt);

			on_frame_render(dt);

			for (auto& window : _windows)
			{
				window->frame_render(dt);
				on_window_frame_render(*window);

				window->frame_end();
				on_window_frame_end(*window);
			}

			on_frame_end(dt);
		}
	}
	
	void engine::register_window(std::unique_ptr<render_window> window)
	{
		window->prepare_surface();
		_windows_pending_addition.emplace_back(std::move(window));
	}

	void engine::register_main_window(std::unique_ptr<render_window> window)
	{
		window->set_main(true);
		window->prepare_surface();
		_windows.emplace_back(std::move(window));
	}

	void engine::process_pending_events()
	{
		auto& inp = core::get_subsystem<input>();
		auto window_iterator = std::begin(_windows);
		while (window_iterator != std::end(_windows))
		{
			auto& window = *window_iterator;

			bool has_focus = window->has_focus();
			bool has_closed = false;
			mml::platform_event e;
			while (window->poll_event(e))
			{
				if (e.type == mml::platform_event::closed)
				{
					has_closed = true;
					_running = !window->is_main();
					break;
				}
				else if (has_focus)
				{
					inp.handle_event(e);
				}
			}

			if (has_closed)
			{
				window_iterator = _windows.erase(window_iterator);
			}
			else
			{
				++window_iterator;
			}
		}
	}

	void engine::process_pending_windows()
	{
		for (auto& window : _windows_pending_addition)
		{
			_windows.emplace_back(std::move(window));
		}
		_windows_pending_addition.clear();
	}

	void engine::destroy_windows()
	{
		_windows.clear();
	}

}
