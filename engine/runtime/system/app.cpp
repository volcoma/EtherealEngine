#include "app.h"
#include "engine.h"
#include "../rendering/render_window.h"

namespace runtime
{

	app::app() : _exitcode(0)
	{}


	void app::setup()
	{
		core::add_subsystem<engine>();
	}

	void app::start()
	{
		auto& eng = core::get_subsystem<engine>();

		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		desktop.width = 1280;
		desktop.height = 720;
		auto main_window = std::make_shared<render_window>(
			desktop,
			"App",
			sf::Style::Default);
		

		if(!eng.start(main_window))
		{
			_exitcode = -1;
			return;
		}
	}

	void app::stop()
	{
		auto& eng = core::get_subsystem<engine>();
		eng.destroy_windows();
	}

	int app::run()
	{
		core::details::initialize();

		setup();
		if (_exitcode != 0)
		{
			core::details::dispose();
			return _exitcode;
		}

		start();
		if (_exitcode != 0)
		{
			core::details::dispose();
			return _exitcode;
		}

		auto& eng = core::get_subsystem<engine>();
		while (eng.is_running())
			eng.run_one_frame();

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
		auto& eng = core::add_subsystem<engine>();
		eng.set_running(false);
		_exitcode = exitcode;
	}

}