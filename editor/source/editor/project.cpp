#include "project.h"
#include "runtime/system/filesystem_watcher.hpp"
#include "runtime/assets/asset_manager.h"
#include "runtime/assets/asset_extensions.h"
#include "runtime/ecs/ecs.h"
#include "runtime/system/task.h"
#include "edit_state.h"
#include "editor_window.h"
#include "assets/asset_compiler.h"
#include "runtime/system/engine.h"
#include "core/serialization/archives.h"
#include "meta/project.hpp"
struct Mesh;
struct Prefab;
struct Texture;
struct Shader;
class Material;

namespace editor
{


	template<typename T>
	void watch_assets(const fs::path& protocol, const std::string& wildcard, bool initialList, bool reloadAsync)
	{
		auto am = core::get_subsystem<runtime::AssetManager>();
		auto ts = core::get_subsystem<runtime::TaskSystem>();

		const fs::path dir = fs::resolve_protocol(protocol);
		fs::path watchDir = dir / wildcard;

		fs::watcher::watch(watchDir, initialList, [am, ts, protocol, reloadAsync](const std::vector<fs::watcher::Entry>& entries)
		{
			for (auto& entry : entries)
			{
				auto p = entry.path;
				auto key = (protocol / p.filename().replace_extension()).generic_string();

				if (entry.type == fs::file_type::regular)
				{
					if (entry.state == fs::watcher::Entry::Removed)
					{
						auto task = ts->create("Remove Asset", [entry, protocol, key, am]()
						{
							am->clear_asset<T>(key);
						});
						ts->run_on_main(task);
					}
					else if (entry.state == fs::watcher::Entry::Renamed)
					{

					}
					else
					{
						//created or modified
						auto task = ts->create("Load Asset", [reloadAsync, key, am]()
						{
							am->load<T>(key, reloadAsync, true);
						});
						ts->run_on_main(task);
					}
				}
				
			}
		});
	}

	template<typename T>
	void watch_raw_assets(const fs::path& protocol, const std::string& wildcard, bool initialList)
	{
		auto am = core::get_subsystem<runtime::AssetManager>();
		auto ts = core::get_subsystem<runtime::TaskSystem>();

		const fs::path dir = fs::resolve_protocol(protocol);
		const fs::path watchDir = dir / wildcard;

		fs::watcher::watch(watchDir, initialList, [am, ts, protocol](const std::vector<fs::watcher::Entry>& entries)
		{
			for (auto& entry : entries)
			{
				const auto& p = entry.path;
				auto key = (protocol / p.filename().replace_extension()).generic_string();


				if (entry.type == fs::file_type::regular)
				{
					if (entry.state == fs::watcher::Entry::Removed)
					{
						auto task = ts->create("Remove Asset", [entry, protocol, key, am]()
						{
							am->delete_asset<T>(key);
						});
						ts->run_on_main(task);
					}
					else
					{
						// created or modified or renamed
						auto task = ts->create("", [p]()
						{
							AssetCompiler<T>::compile(p);
						});
						ts->run(task);

						auto task1 = ts->create("", [p, am, key]()
						{
							am->create_asset_entry<T>(key);
						});
						ts->run_on_main(task1);
					}
				}
			}
		});
	}
	void AssetFolder::watch(bool recompile_assets)
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
						directories.emplace_back(std::make_shared<AssetFolder>(this, p, p.filename().string(), root_path, recompile_assets));

					}
					else if (entry.state == fs::watcher::Entry::Modified)
					{

					}
					else if (entry.state == fs::watcher::Entry::Renamed)
					{
						std::unique_lock<std::mutex> lock(directories_mutex);
						auto it = std::find_if(std::begin(directories), std::end(directories),
							[&entry](const std::shared_ptr<AssetFolder>& other) { return entry.last_path == other->absolute; }
						);

						if (it != std::end(directories))
						{
							auto e = *it;
							e->populate(this, p, p.filename().string(), root_path, false);
						}

					}
					else if (entry.state == fs::watcher::Entry::Removed)
					{
						if (p == AssetFolder::opened->absolute)
							AssetFolder::opened = AssetFolder::root;

						std::unique_lock<std::mutex> lock(directories_mutex);
						directories.erase(std::remove_if(std::begin(directories), std::end(directories),
							[&entry](const std::shared_ptr<AssetFolder>& other) { return entry.path.filename().string() == other->name; }
						), std::end(directories));
					}
				}
				else if (entry.type == fs::file_type::regular)
				{
					if (entry.state == fs::watcher::Entry::New)
					{
						std::unique_lock<std::mutex> lock(files_mutex);
						fs::path filename = p.filename();
						fs::path ext = filename.extension();
						filename.replace_extension();
						files.emplace_back(AssetFile(p, filename.string(), ext.string(), root_path));
					}
					else if (entry.state == fs::watcher::Entry::Modified)
					{

					}
					else if (entry.state == fs::watcher::Entry::Renamed)
					{
						std::unique_lock<std::mutex> lock(files_mutex);
						auto it = std::find_if(std::begin(files), std::end(files),
							[&entry](const AssetFile& other) { return entry.last_path == other.absolute; }
						);

						if (it != std::end(files))
						{
							auto& e = *it;
							fs::path filename = p.filename();
							fs::path ext = filename.extension();
							filename.replace_extension();
							e.populate(p, filename.string(), ext.string(), root_path);
						}
					}
					else if (entry.state == fs::watcher::Entry::Removed)
					{
						std::unique_lock<std::mutex> lock(files_mutex);
						files.erase(std::remove_if(std::begin(files), std::end(files),
							[&entry](const AssetFile& other) { return entry.path == other.absolute; }
						), std::end(files));
					}
				}
			}
		});
		static const std::string wildcard = "*";
		watch_assets<Texture>(relative, wildcard + extensions::texture, !recompile_assets, true);
		watch_raw_assets<Texture>(relative, "*.png", recompile_assets);
		watch_raw_assets<Texture>(relative, "*.tga", recompile_assets);
		watch_raw_assets<Texture>(relative, "*.dds", recompile_assets);
		watch_raw_assets<Texture>(relative, "*.ktx", recompile_assets);
		watch_raw_assets<Texture>(relative, "*.pvr", recompile_assets);

		watch_assets<Shader>(relative, wildcard + extensions::shader, !recompile_assets, true);
		watch_raw_assets<Shader>(relative, "*.sc", recompile_assets);

		watch_assets<Mesh>(relative, wildcard + extensions::mesh, !recompile_assets, true);
		watch_raw_assets<Mesh>(relative, "*.obj", recompile_assets);

		watch_assets<Prefab>(relative, wildcard + extensions::prefab, true, true);
		watch_assets<Material>(relative, wildcard + extensions::material, true, false);
	}

	AssetFile::AssetFile(const fs::path& abs, const std::string& n, const std::string& ext, const fs::path& r)
	{
		populate(abs, n, ext, r);
	}


	void AssetFile::populate(const fs::path& abs, const std::string& n, const std::string& ext, const fs::path& r)
	{
		absolute = abs;
		name = n;
		extension = ext;
		root_path = r;

		fs::path a = absolute;
		relative = string_utils::replace(a.replace_extension().generic_string(), root_path.generic_string(), "app:/data");
	}

	std::shared_ptr<AssetFolder> AssetFolder::opened;
	std::shared_ptr<AssetFolder> AssetFolder::root;

	AssetFolder::AssetFolder(AssetFolder* p, const fs::path& abs, const std::string& n, const fs::path& r, bool recompile_assets)
	{
		populate(p, abs, n, r, recompile_assets);
	}

	AssetFolder::~AssetFolder()
	{
		unwatch();
	}

	void AssetFolder::populate(AssetFolder* p, const fs::path& abs, const std::string& n, const fs::path& r, bool recompile_assets)
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
	

	void AssetFolder::unwatch()
	{
		fs::watcher::unwatch(absolute / fs::path("*"), true);
	}


	void ProjectManager::open_project(const fs::path& project_path, bool recompile_assets)
	{
		if (!fs::exists(project_path, std::error_code{}))
		{
			auto logger = logging::get("Log");
			logger->error().write("Project directory doesn't exist {0}", project_path.string());
			return;
		}
		fs::add_path_protocol("app:", project_path);
		
		auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
		auto am = core::get_subsystem<runtime::AssetManager>();
		auto es = core::get_subsystem<EditState>();
		ecs->dispose();
		es->unselect();
		es->scene.clear();
		am->clear("app:/data");
		set_current_project(project_path.filename().string());
		auto& rp = _options.recent_project_paths;
		if (std::find(std::begin(rp), std::end(rp), project_path.generic_string()) == std::end(rp))
		{
			rp.push_back(project_path.generic_string());
			save_config();
		}
		fs::watcher::unwatch_all();
		static const std::string wildcard = "*";
		/// for debug purposes
		watch_assets<Shader>("engine_data:/shaders", wildcard + extensions::shader, !recompile_assets, true);
		watch_raw_assets<Shader>("engine_data:/shaders", "*.sc", recompile_assets);
// 
// 		watch_assets<Mesh>("engine_data:/meshes", wildcard + extensions::mesh, !recompile_assets, true);
// 		watch_raw_assets<Mesh>("engine_data:/meshes", "*.obj", recompile_assets);
// 
// 		watch_assets<Texture>("engine_data:/textures", wildcard + extensions::texture, true, true);
// 		watch_raw_assets<Texture>("engine_data:/textures", "*.png", recompile_assets);
// 		watch_raw_assets<Texture>("engine_data:/textures", "*.tga", recompile_assets);
// 		watch_raw_assets<Texture>("engine_data:/textures", "*.dds", recompile_assets);
// 		watch_raw_assets<Texture>("engine_data:/textures", "*.ktx", recompile_assets);
// 		watch_raw_assets<Texture>("engine_data:/textures", "*.pvr", recompile_assets);
// 
// 		watch_assets<Texture>("editor_data:/icons", wildcard + extensions::texture, true, true);
// 		watch_raw_assets<Texture>("editor_data:/icons", "*.png", recompile_assets);
// 		watch_raw_assets<Texture>("editor_data:/icons", "*.tga", recompile_assets);
// 		watch_raw_assets<Texture>("editor_data:/icons", "*.dds", recompile_assets);
// 		watch_raw_assets<Texture>("editor_data:/icons", "*.ktx", recompile_assets);
// 		watch_raw_assets<Texture>("editor_data:/icons", "*.pvr", recompile_assets);
// 
		watch_assets<Shader>("editor_data:/shaders", wildcard + extensions::shader, !recompile_assets, true);
		watch_raw_assets<Shader>("editor_data:/shaders", "*.sc", recompile_assets);

		auto& root = fs::resolve_protocol("app:/data");
		AssetFolder::opened.reset();
		AssetFolder::root.reset();
		AssetFolder::root = std::make_shared<AssetFolder>(nullptr, root, root.filename().string(), root, recompile_assets);
		AssetFolder::opened = AssetFolder::root;

	}

	void ProjectManager::create_project(const fs::path& project_path)
	{
		fs::add_path_protocol("app:", project_path);
		fs::create_directory(fs::resolve_protocol("app:/data"), std::error_code{});
		fs::create_directory(fs::resolve_protocol("app:/settings"), std::error_code{});

		open_project(project_path, false);
	}

	void ProjectManager::open()
	{
		auto engine = core::get_subsystem<runtime::Engine>();
		const auto& windows = engine->get_windows();
		auto& mainWindow = *windows[0];

		mainWindow.setVisible(false);

		sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
		desktop.width = 500;
		desktop.height = 300;;
		auto projectManagerWnd = std::make_shared<ProjectManagerWindow>(desktop, "Project Manager", sf::Style::Titlebar);
		projectManagerWnd->on_closed.connect([&mainWindow](RenderWindow& window)
		{
			mainWindow.setVisible(true);
		});
		engine->register_window(projectManagerWnd);
	}


	void ProjectManager::load_config()
	{
		const fs::path absoluteKey = fs::resolve_protocol("editor_data:/config/project.cfg");
		if (!fs::exists(absoluteKey, std::error_code{}))
		{
			save_config();
		}
		else
		{
			std::ifstream output(absoluteKey);
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

	void ProjectManager::save_config()
	{
		fs::create_directory(fs::resolve_protocol("editor_data:/config"), std::error_code{});
		const std::string absoluteKey = fs::resolve_protocol("editor_data:/config/project.cfg").string();
		std::ofstream output(absoluteKey);
		cereal::oarchive_json_t ar(output);

		try_save(ar, cereal::make_nvp("options", _options));
	}

	bool ProjectManager::initialize()
	{
		load_config();
		return true;
	}

	void ProjectManager::dispose()
	{
		save_config();
		fs::watcher::unwatch_all();
		AssetFolder::opened.reset();
		AssetFolder::root.reset();
	}

}