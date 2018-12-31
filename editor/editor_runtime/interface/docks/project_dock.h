#pragma once
#include "imguidock.h"

#include <core/filesystem/filesystem_cache.hpp>

class project_dock : public imguidock::dock
{
public:
	project_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size);
	void render(const ImVec2& area);

private:

	void context_menu();
	void context_create_menu();
	void set_cache_path(const fs::path& path);
	void import();

	fs::directory_cache cache_;
	fs::path cache_path_with_protocol_;
	fs::path root_;
	float scale_ = 0.85f;
};
