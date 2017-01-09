#include "project.h"
#include "runtime/system/filesystem_watcher.hpp"
#include "runtime/assets/asset_manager.h"
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
		auto storage = am->get_storage<T>();

		const fs::path dir = fs::resolve_protocol(protocol);
		fs::path watchDir = dir / storage->subdir / storage->platform / wildcard;

		fs::watcher::watch(watchDir, initialList, [am, ts, protocol, reloadAsync](const std::vector<fs::watcher::Entry>& entries)
		{
			for (auto& entry : entries)
			{
				auto p = entry.path;
				auto key = (protocol / p.filename().replace_extension()).generic_string();

				if (entry.state == fs::watcher::Entry::Removed)
				{
					auto task = ts->create("", [key, am]()
					{
						am->clear_asset<T>(key);
					});
					ts->run_on_main(task);
				}
				else
				{
					//created or modified
					if (fs::is_regular_file(p, std::error_code{}))
					{
						
						auto task = ts->create("", [reloadAsync, key, am]()
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
	void watch_raw_assets(const fs::path& protocol, const std::string& wildcard, bool initialList, const std::string& ignoreFile)
	{
		auto am = core::get_subsystem<runtime::AssetManager>();
		auto ts = core::get_subsystem<runtime::TaskSystem>();

		const fs::path dir = fs::resolve_protocol(protocol);
		const fs::path watchDir = dir / wildcard;

		fs::watcher::watch(watchDir, initialList, [am, ts, protocol, ignoreFile](const std::vector<fs::watcher::Entry>& entries)
		{
			for (auto& entry : entries)
			{
				const auto& p = entry.path;
				auto key = (protocol / p.filename().replace_extension()).generic_string();

				if (!ignoreFile.empty())
				{
					if(p.filename() == ignoreFile)
						continue;
				}

				if (entry.state == fs::watcher::Entry::Removed)
				{

				}
				else
				{
					//created or modified
					if (fs::is_regular_file(p, std::error_code{}))
					{
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

	void ProjectManager::open_project(const fs::path& project_path, bool recompile_assets)
	{
		if (!fs::exists(project_path, std::error_code{}))
		{
			auto logger = logging::get("Log");
			logger->error().write("Project directory doesn't exist {0}", project_path.string());
			return;
		}
		fs::add_path_protocol("app:", project_path);
		fs::add_path_protocol("data:", fs::resolve_protocol("app://data"));
		fs::watcher::unwatch_all();
		auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
		auto am = core::get_subsystem<runtime::AssetManager>();
		auto es = core::get_subsystem<EditState>();
		ecs->dispose();
		es->unselect();
		es->scene.clear();
		am->clear("data://");
		set_current_project(project_path.filename().string());
		auto& rp = _options.recent_project_paths;
		if (std::find(std::begin(rp), std::end(rp), project_path.generic_string()) == std::end(rp))
		{
			rp.push_back(project_path.generic_string());
			save_config();
		}

		watch_assets<Texture>("data://textures", "*.asset", !recompile_assets, true);
		watch_raw_assets<Texture>("data://textures", "*.png", recompile_assets, "");
		watch_raw_assets<Texture>("data://textures", "*.tga", recompile_assets, "");
		watch_raw_assets<Texture>("data://textures", "*.dds", recompile_assets, "");
		watch_raw_assets<Texture>("data://textures", "*.ktx", recompile_assets, "");
		watch_raw_assets<Texture>("data://textures", "*.pvr", recompile_assets, "");

		watch_assets<Shader>("data://shaders", "*.asset", !recompile_assets, true);
		watch_raw_assets<Shader>("data://shaders", "*.sc", recompile_assets, "varying.def.sc");

		watch_assets<Mesh>("data://meshes", "*.asset", !recompile_assets, true);
		watch_raw_assets<Mesh>("data://meshes", "*.obj", recompile_assets, "");

		watch_assets<Prefab>("data://prefabs", "*.asset", true, true);
		watch_assets<Material>("data://materials", "*.asset", true, false);

		/// for debug purposes
// 		watch_assets<Shader>("engine_data://shaders", "*.asset", true, true);
// 		watch_raw_assets<Shader>("engine_data://shaders", "*.sc", recompile_assets, "varying.def.sc");
// 
// 		watch_assets<Texture>("engine_data://textures", "*.asset", true, true);
// 		watch_raw_assets<Texture>("engine_data://textures", "*.png", recompile_assets, "");
// 		watch_raw_assets<Texture>("engine_data://textures", "*.tga", recompile_assets, "");
// 		watch_raw_assets<Texture>("engine_data://textures", "*.dds", recompile_assets, "");
// 		watch_raw_assets<Texture>("engine_data://textures", "*.ktx", recompile_assets, "");
// 		watch_raw_assets<Texture>("engine_data://textures", "*.pvr", recompile_assets, "");
// 
// 		watch_assets<Texture>("editor_data://icons", "*.asset", true, true);
// 		watch_raw_assets<Texture>("editor_data://icons", "*.png", recompile_assets, "");
// 		watch_raw_assets<Texture>("editor_data://icons", "*.tga", recompile_assets, "");
// 		watch_raw_assets<Texture>("editor_data://icons", "*.dds", recompile_assets, "");
// 		watch_raw_assets<Texture>("editor_data://icons", "*.ktx", recompile_assets, "");
// 		watch_raw_assets<Texture>("editor_data://icons", "*.pvr", recompile_assets, "");
// 
// 		watch_assets<Shader>("editor_data://shaders", "*.asset", true, true);
// 		watch_raw_assets<Shader>("editor_data://shaders", "*.sc", recompile_assets, "varying.def.sc");
// 
// 		watch_assets<Mesh>("editor_data://meshes", "*.asset", !recompile_assets, true);
// 		watch_raw_assets<Mesh>("editor_data://meshes", "*.obj", recompile_assets, "");
	}

	void ProjectManager::create_project(const fs::path& project_path)
	{
		fs::add_path_protocol("app:", project_path);
		fs::create_directory(fs::resolve_protocol("app://data"), std::error_code{});
		fs::create_directory(fs::resolve_protocol("app://data/shaders"), std::error_code{});
		fs::create_directory(fs::resolve_protocol("app://data/textures"), std::error_code{});
		fs::create_directory(fs::resolve_protocol("app://data/materials"), std::error_code{});
		fs::create_directory(fs::resolve_protocol("app://data/meshes"), std::error_code{});
		fs::create_directory(fs::resolve_protocol("app://data/scenes"), std::error_code{});
		fs::create_directory(fs::resolve_protocol("app://data/prefabs"), std::error_code{});
		fs::create_directory(fs::resolve_protocol("app://settings"), std::error_code{});

		fs::copy(fs::resolve_protocol("engine_data://meshes"), fs::resolve_protocol("app://data/meshes"), std::error_code{});
		fs::remove(fs::resolve_protocol("app://data/meshes/_compile_.bat"), std::error_code{});
		fs::remove(fs::resolve_protocol("app://data/meshes/makefile"), std::error_code{});
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
		const fs::path absoluteKey = fs::resolve_protocol("editor_data://config/project.cfg");
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
		fs::create_directory(fs::resolve_protocol("editor_data://config"), std::error_code{});
		const std::string absoluteKey = fs::resolve_protocol("editor_data://config/project.cfg").string();
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
	}

}