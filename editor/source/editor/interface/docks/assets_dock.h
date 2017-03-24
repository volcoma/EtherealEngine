#pragma once
#include "imguidock.h"
#include <memory>

namespace editor
{
	struct AssetFolder;
}

struct AssetsDock : public ImGuiDock::Dock
{
	AssetsDock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize);
	void render(const ImVec2& area);

private:
	std::weak_ptr<editor::AssetFolder> opened_folder;
	float scale_icons = 0.7f;
};