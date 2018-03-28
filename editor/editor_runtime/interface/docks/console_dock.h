#pragma once

#include "imguidock.h"
#include <memory>

class console_log;
struct console_dock : public imguidock::dock
{
	console_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size,
				 const std::shared_ptr<console_log>& log);

	void render(const ImVec2& area);

private:
	std::shared_ptr<console_log> console_log_;
};
