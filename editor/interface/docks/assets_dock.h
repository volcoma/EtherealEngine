#pragma once
#include "imguidock.h"
#include <memory>

namespace editor
{
	struct asset_directory;
}

struct assets_dock : public imguidock::dock
{
	assets_dock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize);
	void render(const ImVec2& area);

private:
	std::weak_ptr<editor::asset_directory> opened_dir;
	float scale_icons = 0.7f;
};