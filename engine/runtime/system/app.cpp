#include "app.h"
#include "../assets/asset_manager.h"
#include "../ecs/ecs.h"
#include "../ecs/systems/bone_system.h"
#include "../ecs/systems/camera_system.h"
#include "../ecs/systems/deferred_rendering.h"
#include "../ecs/systems/scene_graph.h"
#include "../input/input.h"
#include "../rendering/render_window.h"
#include "../rendering/renderer.h"
#include "core/logging/logging.h"
#include "core/serialization/serialization.h"
#include "core/system/simulation.h"
#include "core/system/task_system.h"
#include "events.h"

namespace runtime
{

void app::setup(cmd_line::options_parser& parser)
{
	auto logging_container = logging::get_mutable_logging_container();
	logging_container->add_sink(std::make_shared<logging::sinks::platform_sink_mt>());
	logging_container->add_sink(std::make_shared<logging::sinks::daily_file_sink_mt>("Log", 23, 59));

	auto logger = logging::create(APPLOG, logging_container);

	serialization::set_warning_logger([](const std::string& msg) { APPLOG_WARNING(msg); });

	gfx::set_info_logger([](const std::string& msg) { APPLOG_INFO(msg); });
	gfx::set_warning_logger([](const std::string& msg) { APPLOG_WARNING(msg); });
	gfx::set_error_logger([](const std::string& msg) { APPLOG_ERROR(msg); });
    
    
    {
        auto value = cmd_line::value<std::string>();
        value->default_value("auto");
        parser.add_option("renderer",
                          "r", 
                          "renderer", 
                          "Select preferred renderer.",
                          value,
                          "Select preferred renderer.");
    }
}

void app::start(cmd_line::options_parser& parser)
{
	core::add_subsystem<core::simulation>();
	core::add_subsystem<core::task_system>();
	core::add_subsystem<renderer>(parser);
	core::add_subsystem<input>();
	core::add_subsystem<asset_manager>();
	core::add_subsystem<entity_component_system>();
	core::add_subsystem<scene_graph>();
	core::add_subsystem<bone_system>();
	core::add_subsystem<camera_system>();
	core::add_subsystem<deferred_rendering>();
}

void app::stop()
{
}

void poll_events()
{
	auto& renderer = core::get_subsystem<runtime::renderer>();
	const auto& windows = renderer.get_windows();

	std::uint32_t focused_id = 0;
	std::map<std::uint32_t, std::vector<mml::platform_event>> collected_events;
	for(const auto& window : windows)
	{
		const auto id = window->get_id();
		std::vector<mml::platform_event> events;
		mml::platform_event e;
		while(window->poll_event(e))
		{
			events.emplace_back(std::move(e));
		}

		if(window->has_focus())
		{
			focused_id = id;
		}

		if(!events.empty())
		{
			collected_events.emplace(id, std::move(events));
		}
	}

	for(const auto& event_pair : collected_events)
	{
		const auto id = event_pair.first;
		const auto& events = event_pair.second;
		std::pair<std::uint32_t, bool> info{id, (id == focused_id)};
		on_platform_events(info, events);
	}
}

void app::run_one_frame()
{
	auto& sim = core::get_subsystem<core::simulation>();
	auto& tasks = core::get_subsystem<core::task_system>();
	auto& renderer = core::get_subsystem<runtime::renderer>();

	sim.run_one_frame();
	tasks.run_on_owner_thread();

	auto dt = sim.get_delta_time();

	poll_events();

	renderer.process_pending_windows();

	const auto& windows = renderer.get_windows();
	bool should_quit = std::all_of(std::begin(windows), std::end(windows),
								   [](const auto& window) { return !window->is_visible(); });
	if(should_quit)
	{
		quit(0);
		return;
	}

	on_frame_begin(dt);

	on_frame_update(dt);

	on_frame_render(dt);

	on_frame_end(dt);
}

int app::run(int argc, char* argv[])
{
	core::details::initialize();
    
    cmd_line::options_parser parser("App");
    
	setup(parser);
	if(_exitcode != 0)
	{
		core::details::dispose();
		return _exitcode;
	}
    parser.help();
    try
    {
        parser.parse(argc, argv);        
    } 
    catch (const cmd_line::option_exception& e)
    {
        APPLOG_ERROR(e.what());        
    }
	start(parser);
	if(_exitcode != 0)
	{
		core::details::dispose();
		return _exitcode;
	}

	while(_running)
		run_one_frame();

	stop();

	core::details::dispose();
	return _exitcode;
}

void app::quit_with_error(const std::string& message)
{
	APPLOG_ERROR(message.c_str());
	quit(-1);
}

void app::quit(int exitcode)
{
	_running = false;
	_exitcode = exitcode;
}
}
