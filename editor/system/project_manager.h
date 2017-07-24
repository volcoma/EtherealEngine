#pragma once
#include "core/filesystem/filesystem.h"
#include "core/math/math_includes.h"
#include "core/system/subsystem.h"
#include <deque>
#include <mutex>

namespace editor
{
struct asset_file
{
	asset_file(const fs::path& abs, const std::string& n, const std::string& ext, const fs::path& r);

	void populate(const fs::path& abs, const std::string& n, const std::string& ext, const fs::path& r);

	///
	fs::path absolute;
	///
	fs::path root_path;
	///
	std::string relative;
	///
	std::string name;
	///
	std::string extension;
};

struct asset_directory : std::enable_shared_from_this<asset_directory>
{
	asset_directory(asset_directory* p, const fs::path& abs, const std::string& n, const fs::path& r);

	~asset_directory();

	void populate(asset_directory* p, const fs::path& abs, const std::string& n, const fs::path& r);

	void watch();

	void unwatch();
	///
	fs::path absolute;
	///
	fs::path root_path;
	///
	std::string relative;
	///
	std::string name;
	///
	std::mutex files_mutex;
	///
	std::vector<asset_file> files;
	///
	asset_directory* parent = nullptr;
	///
	std::mutex directories_mutex;
	///
	std::vector<std::shared_ptr<asset_directory>> directories;
};

class project_manager : public core::subsystem
{
public:
	struct options
	{
		///
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

	//-----------------------------------------------------------------------------
	//  Name : get_root_directory ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::weak_ptr<asset_directory> get_root_directory()
	{
		return root_directory;
	}

private:
	/// Project options
	options _options;
	/// Current project name
	std::string _project_name;
	///
	std::shared_ptr<asset_directory> root_directory = nullptr;
};
}
