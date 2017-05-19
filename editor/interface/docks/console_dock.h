#pragma once

#include "imguidock.h"

class console_log;
struct console_dock : public imguidock::dock
{
	console_dock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize, std::shared_ptr<console_log> log)
	{	
		_console_log = log;
		initialize(dtitle, dcloseButton, dminSize, std::bind(&console_dock::render, this, std::placeholders::_1));
	}

	void render(const ImVec2& area);

private:
	std::shared_ptr<console_log> _console_log;
};