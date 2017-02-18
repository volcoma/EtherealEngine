#include "engine.h"
#include "core/subsystem/simulation.h"
#include "rendering/renderer.h"
#include "input/input.h"
#include "ecs/ecs.h"
#include "task.h"
#include "ecs/systems/scene_graph.h"
#include "ecs/systems/camera_system.h"
#include "ecs/systems/deferred_rendering.h"
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
		auto logger = logging::create(APPLOG,
		{
			std::make_shared<logging::sinks::platform_sink_mt>(),
			std::make_shared<logging::sinks::daily_file_sink_mt>("Log", "log", 23, 59),
		});
		
		// fire engine
		_running = true;

		return true;
	}

	void Engine::dispose()
	{

	}


	bool Engine::start(std::shared_ptr<RenderWindow> main_window)
	{
		core::add_subsystem<core::Simulation>();

		auto renderer = core::add_subsystem<Renderer>();
		if (!renderer->init_backend(*main_window))
		{
			APPLOG_ERROR("Could not initialize rendering backend!");
			return false;
		}
		register_window(main_window);
		core::add_subsystem<Input>();
		core::add_subsystem<AssetManager>();
		core::add_subsystem<EntityComponentSystem>();
		core::add_subsystem<TaskSystem>();
		core::add_subsystem<SceneGraph>();
		core::add_subsystem<CameraSystem>();
		core::add_subsystem<DeferredRendering>();

		return true;
	}


	void Engine::run_one_frame()
	{
		if (!_running)
			return;

		auto sim = core::get_subsystem<core::Simulation>();
		auto input = core::get_subsystem<Input>();

		sim->run_one_frame();
		auto dt = sim->get_delta_time();

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

			bool has_focus = window->hasFocus();

			sf::Event e;
			while (window->pollEvent(e))
			{
				if (has_focus)
				{
					input->handle_event(e);
				}
			}

			window->frame_update(dt);
		}

		on_frame_update(dt);

		on_frame_render(dt);

		for (auto window : windows)
		{
			window->frame_render(dt);
			window->frame_end();

			if (window->is_main())
				_running = window->isOpen();
		}

		on_frame_end(dt);
	}
	
	void Engine::register_window(std::shared_ptr<RenderWindow> window)
	{
		auto on_closed = [this](RenderWindow& wnd)
		{
			_windows.erase(std::remove_if(std::begin(_windows), std::end(_windows),
				[this, &wnd](std::shared_ptr<RenderWindow>& other)
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

	void Engine::destroy_windows()
	{
		_windows.clear();
		_focused_window.reset();
	}

}