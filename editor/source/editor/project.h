#pragma once
#include "core/subsystem.h"
#include "runtime/system/filesystem.h"
#include <deque>

namespace editor
{
	class ProjectManager : public core::Subsystem
	{
	public:
		struct Options
		{
			std::deque<std::string> recent_project_paths;
		};

		//-----------------------------------------------------------------------------
		//  Name : initialize ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		bool initialize();

		//-----------------------------------------------------------------------------
		//  Name : dispose ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void dispose();

		//-----------------------------------------------------------------------------
		//  Name : open_project ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void open_project(const fs::path& projectPath);

		//-----------------------------------------------------------------------------
		//  Name : create_project ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void create_project(const fs::path& projectPath);

		//-----------------------------------------------------------------------------
		//  Name : open ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void open();

		//-----------------------------------------------------------------------------
		//  Name : save_config ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void save_config();

		//-----------------------------------------------------------------------------
		//  Name : load_config ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		void load_config();

		//-----------------------------------------------------------------------------
		//  Name : get_current_project ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		inline const std::string& get_current_project() const { return _project_name; }

		//-----------------------------------------------------------------------------
		//  Name : set_current_project ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		inline void set_current_project(const std::string& name) { _project_name = name; }

		//-----------------------------------------------------------------------------
		//  Name : get_recent_projects ()
		/// <summary>
		/// 
		/// 
		/// 
		/// </summary>
		//-----------------------------------------------------------------------------
		inline const std::deque<std::string>& get_recent_projects() const { return _options.recent_project_paths; }

	private:
		/// Project options
		Options _options;
		/// Current project name
		std::string _project_name;
	};
}