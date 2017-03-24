#pragma once

#include "imguidock.h"

class ConsoleLog;
struct ConsoleDock : public ImGuiDock::Dock
{
	ConsoleDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize, std::shared_ptr<ConsoleLog> log)
	{	
		_console_log = log;
		initialize(dtitle, dcloseButton, dminSize, std::bind(&ConsoleDock::render, this, std::placeholders::_1));
	}

	void render(const ImVec2& area);

private:
	std::shared_ptr<ConsoleLog> _console_log;
};