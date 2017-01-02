#pragma once
#include "core/subsystem.h"
#include "runtime/system/filesystem.h"
#include <deque>

namespace editor
{
	struct ProjectManager : core::Subsystem
	{
		bool initialize();
		void dispose();
		void open_project(const fs::path& projectPath);
		void create_project(const fs::path& projectPath);
		void open();
		void save_config();
		void load_config();
		const std::string& get_current_project() const { return _project_name; }
		void set_current_project(const std::string& name) { _project_name = name; }
		const std::deque<std::string>& get_recent_projects() const { return _options.recent_project_paths; }

		struct Options
		{
			std::deque<std::string> recent_project_paths;
		};

	private:
		Options _options;
		std::string _project_name;
	};
}