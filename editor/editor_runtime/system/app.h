#pragma once

#include "runtime/system/app.h"
#include <string>

namespace imguidock
{
class dockspace;
}
class console_log;
class render_window;

namespace editor
{
class app : public runtime::app
{
public:
	virtual ~app() = default;

	virtual void setup(cmd_line::parser& parser);

	virtual void start(cmd_line::parser& parser);

	virtual void stop();

private:
    void create_docks();
    void register_console_commands();
	void draw_docks(delta_t dt);
	void draw_header(render_window& window);
	void draw_dockspace(bool is_main, render_window& window, imguidock::dockspace& dockspace);
	void draw_footer(render_window& window, imguidock::dockspace& dockspace);
	void draw_start_page(render_window& window);
	void draw_menubar(render_window& window);
	void draw_toolbar();
	void handle_drag_and_drop();

	bool show_start_page_ = true;
	///
	std::shared_ptr<console_log> console_log_;
	///
	std::string console_dock_name_;
};
}
