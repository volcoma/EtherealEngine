#pragma once
#include "core/filesystem/filesystem.h"
#include "core/math/math_includes.h"
#include <deque>
#include <mutex>

namespace editor
{

class project_manager
{
public:
	struct options
	{
		///
		std::deque<std::string> recent_project_paths;
	};

	project_manager();
	~project_manager();
	//-----------------------------------------------------------------------------
	//  Name : open_project ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void open_project(const fs::path& project_path);

	//-----------------------------------------------------------------------------
	//  Name : close_project ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void close_project();

	//-----------------------------------------------------------------------------
	//  Name : create_project ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void create_project(const fs::path& project_path);

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
	inline const std::string& get_current_project() const
	{
		return _project_name;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_current_project ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_current_project(const std::string& name)
	{
		_project_name = name;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_recent_projects ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline options& get_options()
	{
		return _options;
	}


private:
	/// Project options
	options _options;
	/// Current project name
	std::string _project_name;
	///
	std::vector<std::uint64_t> _watch_ids;
};
}
