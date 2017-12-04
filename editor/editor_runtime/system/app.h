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
    
    virtual void setup(cmd_line::options_parser& parser);

	virtual void start(cmd_line::options_parser& parser);
    
    virtual void stop();
    
    void draw_docks(std::chrono::duration<float>);
    
    void draw_header(render_window& window);
    void draw_dockspace(bool is_main, render_window& window, imguidock::dockspace& dockspace);    
    void draw_footer(render_window& window, imguidock::dockspace& dockspace);
    void draw_start_page(render_window& window);
    void draw_menubar(render_window& window);
    void draw_toolbar();
    void handle_drag_and_drop();
private:
    bool _show_start_page = true;
    ///
    std::shared_ptr<console_log> _console_log;
    ///
    std::string _console_dock_name;
};
}
