#include "project_manager.h"
#include "runtime/system/filesystem_watcher.hpp"
#include "runtime/assets/asset_manager.h"
#include "runtime/assets/asset_extensions.h"
#include "runtime/ecs/ecs.h"
#include "runtime/system/task.h"
#include "runtime/system/engine.h"
#include "core/serialization/archives.h"

#include "../editing/editing_system.h"
#include "../assets/asset_compiler.h"
#include "../meta/project_manager.hpp"
#include <fstream>

class mesh;
struct prefab;
struct scene;
struct texture;
struct shader;
class material;

namespace editor
{
	template<typename T>
	void watch_assets(const fs::path& protocol, const std::string& wildcard, bool initialList, bool reloadAsync)
	{
		auto& am = core::get_subsystem<runtime::asset_manager>();
		auto& ts = core::get_subsystem<runtime::task_system>();

		const fs::path dir = fs::resolve_protocol(protocol);
		fs::path watchDir = dir / wildcard;

		fs::watcher::watch(watchDir, initialList, [&am, &ts, protocol, reloadAsync](const std::vector<fs::watcher::Entry>& entries)
		{
			for (auto& entry : entries)
			{
				auto p = entry.path;
				auto key = (protocol / p.filename().replace_extension()).generic_string();

				if (entry.type == fs::file_type::regular)
				{
					if (entry.state == fs::watcher::Entry::Removed)
					{
						auto task = ts.create("Remove Asset", [entry, protocol, key, &am]()
						{
							am.clear_asset<T>(key);
						});
						ts.run(task, true);
					}
					else if (entry.state == fs::watcher::Entry::Renamed)
					{

					}
					else
					{
						//created or modified
						auto task = ts.create("Load Asset", [reloadAsync, key, &am]()
						{
							am.load<T>(key, reloadAsync, true);
						});
						ts.run(task, true);
					}
				}
				
			}
		});
	}

	template<typename T>
	void watch_raw_assets(const fs::path& protocol, const std::string& wildcard, bool initialList)
	{
		auto& am = core::get_subsystem<runtime::asset_manager>();
		auto& ts = core::get_subsystem<runtime::task_system>();

		const fs::path dir = fs::resolve_protocol(protocol);
		const fs::path watch_dir = dir / wildcard;

		fs::watcher::watch(watch_dir, initialList, [&am, &ts, protocol](const std::vector<fs::watcher::Entry>& entries)
		{
			for (auto& entry : entries)
			{
				const auto& p = entry.path;
				auto key = (protocol / p.stem()).generic_string();

				if (entry.type == fs::file_type::regular)
				{
					if (entry.state == fs::watcher::Entry::Removed)
					{
						auto task = ts.create("Remove Asset", [entry, protocol, key, &am]()
						{
							am.delete_asset<T>(key);
						});
						ts.run(task, true);
					}
					else
					{
						// created or modified or renamed
						auto task = ts.create("", [p]()
						{
							asset_compiler::compile<T>(p);
						});
						ts.run(task);
					}
				}
			}
		});
	}
	void asset_directory::watch(bool recompile_assets)
	{
		fs::watcher::watch(absolute / fs::path("*"), true, [this, recompile_assets](const std::vector<fs::watcher::Entry>& entries)
		{
			for (auto& entry : entries)
			{
				const auto& p = entry.path;

				if (entry.type == fs::file_type::directory)
				{
					if (entry.state == fs::watcher::Entry::New)
					{
						std::unique_lock<std::mutex> lock(directories_mutex);
						directories.emplace_back(std::make_shared<asset_directory>(this, p, p.filename().string(), root_path, recompile_assets));

					}
					else if (entry.state == fs::watcher::Entry::Modified)
					{

					}
					else if (entry.state == fs::watcher::Entry::Renamed)
					{
						std::unique_lock<std::mutex> lock(directories_mutex);
						auto it = std::find_if(std::begin(directories), std::end(directories),
							[&entry](const std::shared_ptr<asset_directory>& other) { return entry.last_path == other->absolute; }
						);

						if (it != std::end(directories))
						{
							auto e = *it;
							e->populate(this, p, p.filename().string(), root_path, false);
						}

					}
					else if (entry.state == fs::watcher::Entry::Removed)
					{
						std::unique_lock<std::mutex> lock(directories_mutex);
						directories.erase(std::remove_if(std::begin(directories), std::end(directories),
							[&entry](const std::shared_ptr<asset_directory>& other) { return entry.path.filename().string() == other->name; }
						), std::end(directories));
					}
				}
				else if (entry.type == fs::file_type::regular)
				{
					if (entry.state == fs::watcher::Entry::New)
					{
						std::unique_lock<std::mutex> lock(files_mutex);
						fs::path filename = p.stem();
						fs::path ext = p.extension();
						files.emplace_back(asset_file(p, filename.string(), ext.string(), root_path));
					}
					else if (entry.state == fs::watcher::Entry::Modified)
					{

					}
					else if (entry.state == fs::watcher::Entry::Renamed)
					{
						std::unique_lock<std::mutex> lock(files_mutex);
						auto it = std::find_if(std::begin(files), std::end(files),
							[&entry](const asset_file& other) { return entry.last_path == other.absolute; }
						);

						if (it != std::end(files))
						{
							auto& e = *it;
							fs::path filename = p.stem();
							fs::path ext = p.extension();
							e.populate(p, filename.string(), ext.string(), root_path);
						}
					}
					else if (entry.state == fs::watcher::Entry::Removed)
					{
						std::unique_lock<std::mutex> lock(files_mutex);
						files.erase(std::remove_if(std::begin(files), std::end(files),
							[&entry](const asset_file& other) { return entry.path == other.absolute; }
						), std::end(files));
					}
				}
			}
		});
		static const std::string wildcard = "*";

		watch_assets<material>(relative, wildcard + extensions::material, true, false);

		watch_assets<texture>(relative, wildcard + extensions::texture, !recompile_assets, true);
		static const std::array<std::string, 6>  raw_texture_formats = 
		{ 
			"*.png", "*.jpg", "*.tga", "*.dds", "*.ktx", "*.pvr" 
		};
		for (const auto& format : raw_texture_formats)
		{
			watch_raw_assets<texture>(relative, format, recompile_assets);
		}

		watch_assets<shader>(relative, wildcard + extensions::shader, !recompile_assets, true);
		watch_raw_assets<shader>(relative, "*.sc", recompile_assets);

		watch_assets<mesh>(relative, wildcard + extensions::mesh, !recompile_assets, true);
		static const std::array<std::string, 5>  raw_mesh_formats =
		{
			"*.obj", "*.fbx", "*.dae", "*.blend", "*.3ds"
		};
		for (const auto& format : raw_mesh_formats)
		{
			watch_raw_assets<mesh>(relative, format, recompile_assets);
		}
		watch_assets<prefab>(relative, wildcard + extensions::prefab, true, true);
		watch_assets<scene>(relative, wildcard + extensions::scene, true, true);
		
	}



	asset_file::asset_file(const fs::path& abs, const std::string& n, const std::string& ext, const fs::path& r)
	{
		populate(abs, n, ext, r);
	}

	void asset_file::populate(const fs::path& abs, const std::string& n, const std::string& ext, const fs::path& r)
	{
		absolute = abs;
		name = n;
		extension = ext;
		root_path = r;

		fs::path a = absolute;
		relative = string_utils::replace(a.replace_extension().generic_string(), root_path.generic_string(), "app:/data");
	}

	asset_directory::asset_directory(asset_directory* p, const fs::path& abs, const std::string& n, const fs::path& r, bool recompile_assets)
	{
		populate(p, abs, n, r, recompile_assets);
	}

	asset_directory::~asset_directory()
	{
		unwatch();
	}

	void asset_directory::populate(asset_directory* p, const fs::path& abs, const std::string& n, const fs::path& r, bool recompile_assets)
	{	
		if(!absolute.empty())
			unwatch();

		parent = p;
		absolute = abs;
		name = n;
		root_path = r;
		relative = string_utils::replace(absolute.generic_string(), root_path.generic_string(), "app:/data");
		
		watch(recompile_assets);
	}

	void asset_directory::unwatch()
	{
		fs::watcher::unwatch(absolute / fs::path("*"), true);
	}

	void project_manager::open_project(const fs::path& project_path, bool recompile_project_assets, bool recompile_engine_assets)
	{
		if (!fs::exists(project_path, std::error_code{}))
		{
			APPLOG_ERROR("Project directory doesn't exist {0}", project_path.string());
			return;
		}

		fs::add_path_protocol("app:", project_path);
		
		auto& ecs = core::get_subsystem<runtime::entity_component_system>();
		auto& am = core::get_subsystem<runtime::asset_manager>();
		auto& es = core::get_subsystem<editing_system>();
		ecs.dispose();
		es.unselect();
		es.scene.clear();
		am.clear("app:/data");
		set_current_project(project_path.filename().string());
		save_config();

		fs::watcher::unwatch_all();

		static const std::string wildcard = "*";
		
		/// for debug purposes
		watch_assets<shader>("engine_data:/shaders", wildcard + extensions::shader, !recompile_engine_assets, true);
		watch_raw_assets<shader>("engine_data:/shaders", "*.sc", recompile_engine_assets);
		watch_assets<shader>("editor_data:/shaders", wildcard + extensions::shader, !recompile_engine_assets, true);
		watch_raw_assets<shader>("editor_data:/shaders", "*.sc", recompile_engine_assets);

//  	watch_assets<mesh>("engine_data:/meshes", wildcard + extensions::mesh, !recompile_engine_assets, true);
//  	watch_raw_assets<mesh>("engine_data:/meshes", "*.obj", recompile_engine_assets);
// 		watch_raw_assets<mesh>("engine_data:/meshes", "*.fbx", recompile_engine_assets);
// 		watch_raw_assets<mesh>("engine_data:/meshes", "*.dae", recompile_engine_assets);
// 		watch_raw_assets<mesh>("engine_data:/meshes", "*.blend", recompile_engine_assets);
// 		watch_raw_assets<mesh>("engine_data:/meshes", "*.3ds", recompile_engine_assets);
// 
// 		watch_assets<texture>("engine_data:/textures", wildcard + extensions::texture, !recompile_engine_assets, true);
// 		watch_raw_assets<texture>("engine_data:/textures", "*.png", recompile_engine_assets);
// 		watch_raw_assets<texture>("engine_data:/textures", "*.tga", recompile_engine_assets);
// 		watch_raw_assets<texture>("engine_data:/textures", "*.dds", recompile_engine_assets);
// 		watch_raw_assets<texture>("engine_data:/textures", "*.ktx", recompile_engine_assets);
// 		watch_raw_assets<texture>("engine_data:/textures", "*.pvr", recompile_engine_assets);
// 
// 		watch_assets<texture>("editor_data:/icons", wildcard + extensions::texture, !recompile_engine_assets, true);
// 		watch_raw_assets<texture>("editor_data:/icons", "*.png", recompile_engine_assets);
// 		watch_raw_assets<texture>("editor_data:/icons", "*.tga", recompile_engine_assets);
// 		watch_raw_assets<texture>("editor_data:/icons", "*.dds", recompile_engine_assets);
// 		watch_raw_assets<texture>("editor_data:/icons", "*.ktx", recompile_engine_assets);
// 		watch_raw_assets<texture>("editor_data:/icons", "*.pvr", recompile_engine_assets);
// 

		auto& root = fs::resolve_protocol("app:/data");
		root_directory.reset();
		root_directory = std::make_shared<asset_directory>(nullptr, root, root.filename().string(), root, recompile_project_assets);
	}

	void project_manager::create_project(const fs::path& project_path)
	{
		fs::add_path_protocol("app:", project_path);
		fs::create_directory(fs::resolve_protocol("app:/data"), std::error_code{});
		fs::create_directory(fs::resolve_protocol("app:/settings"), std::error_code{});

		open_project(project_path, false, false);
	}

	void project_manager::load_config()
	{
		const fs::path project_config_file = fs::resolve_protocol("editor_data:/config/project.cfg");
		if (!fs::exists(project_config_file, std::error_code{}))
		{
			save_config();
		}
		else
		{
			std::ifstream output(project_config_file);
			cereal::iarchive_json_t ar(output);

			try_load(ar, cereal::make_nvp("options", _options));

			auto& items = _options.recent_project_paths;
			auto iter = std::begin(items);
			while (iter != items.end())
			{
				auto& item = *iter;

				if (!fs::exists(item, std::error_code{}))
				{
					iter = items.erase(iter);
				}
				else
				{
					++iter;
				}
			}
		}
	}

	void project_manager::save_config()
	{
		auto& rp = _options.recent_project_paths;
		auto project_path = fs::resolve_protocol("app:");
		if (std::find(std::begin(rp), std::end(rp), project_path.generic_string()) == std::end(rp))
		{
			rp.push_back(project_path.generic_string());
		}
		fs::create_directory(fs::resolve_protocol("editor_data:/config"), std::error_code{});
		const std::string project_config_file = fs::resolve_protocol("editor_data:/config/project.cfg").string();
		std::ofstream output(project_config_file);
		cereal::oarchive_json_t ar(output);

		try_save(ar, cereal::make_nvp("options", _options));
	}

	bool project_manager::initialize()
	{
		load_config();
		return true;
	}

	void project_manager::dispose()
	{
		save_config();
		fs::watcher::unwatch_all();
		root_directory.reset();
	}

}