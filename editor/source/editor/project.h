#pragma once
#include "core/subsystem.h"
#include "core/math/math_includes.h"
#include "runtime/system/filesystem.h"
#include <deque>
#include <mutex>

namespace editor
{
	struct AssetFile
	{
		AssetFile(const fs::path& abs, const std::string& n, const std::string& ext, const fs::path& r);

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

	struct AssetFolder : std::enable_shared_from_this<AssetFolder>
	{
		AssetFolder(AssetFolder* p, const fs::path& abs, const std::string& n, const fs::path& r, bool recompile_assets);

		~AssetFolder();

		void populate(AssetFolder* p, const fs::path& abs, const std::string& n, const fs::path& r, bool recompile_assets);

		inline std::shared_ptr<AssetFolder> make_shared() { return shared_from_this(); }

		void watch(bool recompile_assets);

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
		std::vector<AssetFile> files;
		///
		AssetFolder* parent;
		///
		std::mutex directories_mutex;
		///
		std::vector<std::shared_ptr<AssetFolder>> directories;
		///
		static std::shared_ptr<AssetFolder> opened;
		///
		static std::shared_ptr<AssetFolder> root;
	};


	class ProjectManager : public core::Subsystem
	{
	public:
		struct Options
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
		void open_project(const fs::path& project_path, bool recompile_assets);

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
		inline Options& get_options() { return _options; }



	private:
		/// Project options
		Options _options;
		/// Current project name
		std::string _project_name;
	};
}