#pragma once

#include "imguidock.h"

class console_log;
struct console_dock : public imguidock::dock
{
	console_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size,
				 std::shared_ptr<console_log> log);

	void render(const ImVec2& area);

private:
	std::shared_ptr<console_log> _console_log;
};
