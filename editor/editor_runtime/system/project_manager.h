#pragma once
#include "core/filesystem/filesystem_syncer.h"
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
	bool open_project(const fs::path& project_path);

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
	//  Name : get_name ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const std::string& get_name() const
	{
		return project_name_;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_current_project ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_name(const std::string& name)
	{
		project_name_ = name;
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
		return options_;
	}

private:
	void setup_directory(fs::syncer& syncer);
	void setup_meta_syncer(fs::syncer& syncer, const fs::path& data_dir, const fs::path& meta_dir);
	void setup_cache_syncer(fs::syncer& syncer, const fs::path& meta_dir, const fs::path& cache_dir);
	/// Project options
	options options_;
	/// Current project name
	std::string project_name_;

	fs::syncer app_meta_syncer_;
	fs::syncer app_cache_syncer_;

	fs::syncer editor_meta_syncer_;
	fs::syncer editor_cache_syncer_;

	fs::syncer engine_meta_syncer_;
	fs::syncer engine_cache_syncer_;
};
}
