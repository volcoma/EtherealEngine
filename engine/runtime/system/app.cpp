#include "app.h"
#include "app_setup.h"
#include "events.h"

#include "../assets/asset_manager.h"
#include "../ecs/ecs.h"
#include "../ecs/systems/audio_system.h"
#include "../ecs/systems/bone_system.h"
#include "../ecs/systems/camera_system.h"
#include "../ecs/systems/deferred_rendering.h"
#include "../ecs/systems/reflection_probe_system.h"
#include "../ecs/systems/scene_graph.h"
#include "../input/input.h"
#include "../rendering/render_window.h"
#include "../rendering/renderer.h"

#include <core/audio/library.h>
#include <core/logging/logging.h>
#include <core/serialization/serialization.h>
#include <core/simulation/simulation.h>
#include <core/tasks/task_system.h>

#include <sstream>

namespace runtime
{

void app::setup(cmd_line::parser& parser)
{
	auto logging_container = logging::get_mutable_logging_container();
	logging_container->add_sink(std::make_shared<logging::sinks::platform_sink_mt>());
	logging_container->add_sink(std::make_shared<logging::sinks::daily_file_sink_mt>("Log", 23, 59));

	logging::create(APPLOG, logging_container);

	serialization::set_warning_logger([](const std::string& msg) { APPLOG_WARNING(msg); });

	gfx::set_info_logger([](const std::string& msg) { APPLOG_INFO(msg); });
	gfx::set_warning_logger([](const std::string& msg) { APPLOG_WARNING(msg); });
	gfx::set_error_logger([](const std::string& msg) { APPLOG_ERROR(msg); });

	audio::set_info_logger([](const std::string& msg) { APPLOG_INFO(msg); });
	audio::set_error_logger([](const std::string& msg) { APPLOG_ERROR(msg); });

	ecs::set_frame_getter([]() { return core::get_subsystem<core::simulation>().get_frame(); });

	parser.set_optional<std::string>("r", "renderer", "auto", "Select preferred renderer.");
	parser.set_optional<bool>("n", "novsync", false, "Disable vsync.");
}

void app::start(cmd_line::parser& parser)
{
	// this order is important
	core::add_subsystem<core::simulation>();
	core::add_subsystem<renderer>(parser);
	core::add_subsystem<input>();
	core::add_subsystem<audio::device>();
	core::add_subsystem<asset_manager>();
	core::add_subsystem<core::task_system>(false);
	setup_asset_manager();
	core::add_subsystem<entity_component_system>();
	core::add_subsystem<scene_graph>();
	core::add_subsystem<bone_system>();
	core::add_subsystem<camera_system>();
	core::add_subsystem<reflection_probe_system>();
	core::add_subsystem<deferred_rendering>();
	core::add_subsystem<audio_system>();
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
			events.emplace_back(e);
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
	using namespace std::literals;

	auto& sim = core::get_subsystem<core::simulation>();
	auto& tasks = core::get_subsystem<core::task_system>();
	auto& renderer = core::get_subsystem<runtime::renderer>();
	const bool is_active = renderer.get_focused_window() != nullptr;
	sim.run_one_frame(is_active);
	tasks.run_on_owner_thread(5ms);

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

	on_frame_ui_render(dt);

	on_frame_end(dt);
}

int app::run(int argc, char* argv[])
{
	core::details::initialize();

	cmd_line::parser parser(argc, argv);

	setup(parser);
	if(exitcode_ != 0)
	{
		core::details::dispose();
		return exitcode_;
	}

	std::stringstream out, err;
	if(!parser.run(out, err))
	{
		auto parse_error = out.str();
		if(parse_error.empty())
		{
			parse_error = "Failed to parse command line.";
		}
		APPLOG_ERROR(parse_error);
	}
	auto parse_info = out.str();
	if(!parse_info.empty())
	{
		APPLOG_INFO(parse_info);
	}

	APPLOG_INFO("Initializing...");
	start(parser);
	if(exitcode_ != 0)
	{
		core::details::dispose();
		return exitcode_;
	}

	APPLOG_INFO("Starting...");
	while(running_)
		run_one_frame();

	APPLOG_INFO("Deinitializing...");

	stop();

	APPLOG_INFO("Exiting...");

	core::details::dispose();
	return exitcode_;
}

void app::quit_with_error(const std::string& message)
{
	APPLOG_ERROR(message.c_str());
	quit(-1);
}

void app::quit(int exitcode)
{
	running_ = false;
	exitcode_ = exitcode;
}
} // namespace runtime
