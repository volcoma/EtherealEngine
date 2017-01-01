#include "app.h"
#include "engine.h"
#include "rendering/renderer.h"
#include "assets/asset_manager.h"

namespace runtime
{

	App::App() : _exitcode(0)
	{}


	void App::start()
	{
		auto engine = core::get_subsystem<Engine>();
		auto renderer = core::get_subsystem<Renderer>();
		auto am = core::get_subsystem<AssetManager>();

		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		desktop.width = 1280;
		desktop.height = 720;
		auto main_window = std::make_shared<RenderWindow>(
			desktop,
			"App",
			sf::Style::Default);

		if (!renderer->init_backend(*main_window))
			_exitcode = -1;

		engine->register_window(main_window);
		am->setup();
	}

	int App::run()
	{
		core::details::initialize();

		setup();
		if (_exitcode != 0)
			return _exitcode;

		auto engine = core::add_subsystem<Engine>();
		start();
		if (_exitcode != 0)
			return _exitcode;

		while (engine->is_running())
			engine->run_one_frame();

		stop();

		core::details::dispose();
		return _exitcode;
	}

	void App::terminate_with_error(const std::string& message)
	{
		logging::get("Log")->error() << message;
		_exitcode = -1;
	}

}