#include "project_manager.h"
#include "../assets/asset_compiler.h"
#include "../editing/editing_system.h"
#include "../meta/system/project_manager.hpp"
#include "core/filesystem/filesystem_watcher.hpp"
#include "core/serialization/associative_archive.h"
#include "core/system/task_system.h"
#include "runtime/assets/asset_extensions.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/ecs/ecs.h"
#include "runtime/system/engine.h"
#include <fstream>

class mesh;
struct prefab;
struct scene;
struct texture;
struct shader;
class material;

namespace editor
{

template <typename T>
void watch_assets(const fs::path& protocol, const std::string& wildcard, bool reload_async,
				  bool force_initial_recompile)
{
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& ts = core::get_subsystem<core::task_system>();

	const fs::path dir = fs::resolve_protocol(protocol);
	fs::path watch_dir = dir / wildcard;

	fs::watcher::watch(watch_dir, false, true, [&am, &ts, protocol, reload_async, force_initial_recompile](
												   const std::vector<fs::watcher::entry>& entries,
												   bool is_initial_list) {
		for(const auto& entry : entries)
		{
			const auto& p = entry.path;
			const auto ext = p.extension().string();
			const bool is_compiled = extensions::is_compiled_format(ext);
			if(is_compiled)
			{
				auto filename = p.filename();

				// remove the compiled extension if we have one
				if(ext == extensions::compiled)
				{
					for(auto temp = filename; temp.has_extension(); temp = filename.stem())
					{
						filename = temp;
					}
				}
				auto path = (protocol / filename);
				auto key = path.generic_string();

				if(entry.type == fs::file_type::regular_file)
				{
					if(entry.status == fs::watcher::entry_status::removed)
					{
						auto task =
							ts.push_on_main([reload_async, key, &am]() { am.clear_asset<T>(key); });
					}
					else if(entry.status == fs::watcher::entry_status::renamed)
					{
					}
					else
					{
						using namespace runtime;
						load_mode mode = reload_async ? load_mode::async : load_mode::sync;
						load_flags flags = is_initial_list ? load_flags::standard : load_flags::reload;

						// created or modified
						auto task =
							ts.push([mode, flags, key, &am]() 
                        {
                            am.load<T>(key, mode, flags); 
                        });
					}
				}
			}
			else
			{
				auto key = (protocol / p.filename()).generic_string();
				if(entry.type == fs::file_type::regular_file)
				{
					if(entry.status == fs::watcher::entry_status::removed)
					{
						auto task = ts.push_on_main([p, ext, protocol, key, &am]() {
							am.delete_asset<T>(key);

							// always add the extensions since we want the compiled asset version to be
							// removed too.
							auto path_to_remove = p;
							path_to_remove.concat(extensions::get_compiled_format<T>());
							fs::error_code err;
							fs::remove(path_to_remove, err);

						});
					}
					else if(entry.status == fs::watcher::entry_status::renamed)
					{
					}
					else
					{
						// created or modified or renamed

						bool compile = true;

						if(is_initial_list && !force_initial_recompile)
						{
							fs::path compiled_file = p.string() + extensions::get_compiled_format<T>();
							fs::error_code err;
							compile = !fs::exists(compiled_file, err);
						}

						if(compile)
						{
							auto task =
								ts.push([](const fs::path& p) { asset_compiler::compile<T>(p); }, p);
						}
					}
				}
			}
		}
	});
}

void asset_directory::watch()
{
	fs::watcher::watch(
		absolute / fs::path("*"), false, true,
		[this](const std::vector<fs::watcher::entry>& entries, bool is_initial_list) {
			for(auto& entry : entries)
			{
				const auto& p = entry.path;

				if(entry.type == fs::file_type::directory_file)
				{
					if(entry.status == fs::watcher::entry_status::created)
					{
						std::unique_lock<std::mutex> lock(directories_mutex);
						directories.emplace_back(
							std::make_shared<asset_directory>(this, p, p.filename().string(), root_path));
					}
					else if(entry.status == fs::watcher::entry_status::modified)
					{
					}
					else if(entry.status == fs::watcher::entry_status::renamed)
					{
						std::unique_lock<std::mutex> lock(directories_mutex);
						auto it = std::find_if(std::begin(directories), std::end(directories),
											   [&entry](const std::shared_ptr<asset_directory>& other) {
												   return entry.last_path == other->absolute;
											   });

						if(it != std::end(directories))
						{
							auto e = *it;
							e->populate(this, p, p.filename().string(), root_path);
						}
					}
					else if(entry.status == fs::watcher::entry_status::removed)
					{
						std::unique_lock<std::mutex> lock(directories_mutex);
						directories.erase(
							std::remove_if(std::begin(directories), std::end(directories),
										   [&entry](const std::shared_ptr<asset_directory>& other) {
											   return entry.path.filename().string() == other->name;
										   }),
							std::end(directories));
					}
				}
				else if(entry.type == fs::file_type::regular_file)
				{
					if(entry.status == fs::watcher::entry_status::created)
					{
						std::unique_lock<std::mutex> lock(files_mutex);
						fs::path filename = p.stem();
						fs::path ext = p.extension();
						files.emplace_back(asset_file(p, filename.string(), ext.string(), root_path));
					}
					else if(entry.status == fs::watcher::entry_status::modified)
					{
					}
					else if(entry.status == fs::watcher::entry_status::renamed)
					{
						std::unique_lock<std::mutex> lock(files_mutex);
						auto it = std::find_if(
							std::begin(files), std::end(files),
							[&entry](const asset_file& other) { return entry.last_path == other.absolute; });

						if(it != std::end(files))
						{
							auto& e = *it;
							fs::path filename = p.stem();
							fs::path ext = p.extension();
							e.populate(p, filename.string(), ext.string(), root_path);
						}
					}
					else if(entry.status == fs::watcher::entry_status::removed)
					{
						std::unique_lock<std::mutex> lock(files_mutex);
						files.erase(std::remove_if(std::begin(files), std::end(files),
												   [&entry](const asset_file& other) {
													   return entry.path == other.absolute;
												   }),
									std::end(files));
					}
				}
			}
		});
	static const std::string wildcard = "*";

	for(const auto& format : extensions::texture)
	{
		watch_assets<texture>(relative, wildcard + format, true, false);
	}

	watch_assets<shader>(relative, wildcard + extensions::shader, true, false);

	for(const auto& format : extensions::mesh)
	{
		watch_assets<mesh>(relative, wildcard + format, true, false);
	}

	watch_assets<material>(relative, wildcard + extensions::material, true, false);
	watch_assets<prefab>(relative, wildcard + extensions::prefab, true, false);
	watch_assets<scene>(relative, wildcard + extensions::scene, true, false);
}

asset_file::asset_file(const fs::path& abs, const std::string& n, const std::string& ext, const fs::path& r)
{
	populate(abs, n, ext, r);
}

void asset_file::populate(const fs::path& abs, const std::string& n, const std::string& ext,
						  const fs::path& r)
{
	absolute = abs;
	name = n;
	extension = ext;
	root_path = r;

	fs::path a = absolute;
	relative = string_utils::replace(a.generic_string(), root_path.generic_string(), "app:/data");
}

asset_directory::asset_directory(asset_directory* p, const fs::path& abs, const std::string& n,
								 const fs::path& r)
{
	populate(p, abs, n, r);
}

asset_directory::~asset_directory()
{
	unwatch();
}

void asset_directory::populate(asset_directory* p, const fs::path& abs, const std::string& n,
							   const fs::path& r)
{
	if(!absolute.empty())
		unwatch();

	parent = p;
	absolute = abs;
	name = n;
	root_path = r;
	relative = string_utils::replace(absolute.generic_string(), root_path.generic_string(), "app:/data");

	watch();
}

void asset_directory::unwatch()
{
	fs::watcher::unwatch(absolute / fs::path("*"), true);
}

void project_manager::close_project()
{
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& es = core::get_subsystem<editing_system>();
	es.save_editor_camera();
	ecs.dispose();
	es.unselect();
	es.scene.clear();
	am.clear("app:/data");
	fs::watcher::unwatch_all();
	root_directory.reset();
}

void project_manager::open_project(const fs::path& project_path)
{
	close_project();

	fs::error_code err;
	if(!fs::exists(project_path, err))
	{
		APPLOG_ERROR("Project directory doesn't exist {0}", project_path.string());
		return;
	}

	fs::add_path_protocol("app:", project_path);

	set_current_project(project_path.filename().string());

	save_config();

	/// for debug purposes
	//watch_assets<shader>("engine_data:/shaders", "*.sc", true, true);
	//watch_assets<shader>("editor_data:/shaders", "*.sc", true, true);
	// watch_assets<texture>("engine_data:/textures", "*.png", true);
	// watch_assets<texture>("engine_data:/textures", "*.tga", true);
	// watch_assets<texture>("engine_data:/textures", "*.dds", true);
	// watch_assets<texture>("engine_data:/textures", "*.ktx", true);
	// watch_assets<texture>("engine_data:/textures", "*.pvr", true);
	// watch_assets<texture>("editor_data:/icons", "*.png", true);
	// watch_assets<texture>("editor_data:/icons", "*.tga", true);
	// watch_assets<texture>("editor_data:/icons", "*.dds", true);
	// watch_assets<texture>("editor_data:/icons", "*.ktx", true);
	// watch_assets<texture>("editor_data:/icons", "*.pvr", true);
	// watch_assets<mesh>("engine_data:/meshes", "*.obj", true);
	// watch_assets<mesh>("engine_data:/meshes", "*.fbx", true);
	// watch_assets<mesh>("engine_data:/meshes", "*.dae", true);
	// watch_assets<mesh>("engine_data:/meshes", "*.blend", true);
	// watch_assets<mesh>("engine_data:/meshes", "*.3ds", true);

	auto root = fs::resolve_protocol("app:/data");
	root_directory.reset();
	root_directory = std::make_shared<asset_directory>(nullptr, root, root.filename().string(), root);

	auto& es = core::get_subsystem<editing_system>();
	es.load_editor_camera();
}

void project_manager::create_project(const fs::path& project_path)
{
	fs::error_code err;
	fs::add_path_protocol("app:", project_path);
	fs::create_directory(fs::resolve_protocol("app:/data"), err);
	fs::create_directory(fs::resolve_protocol("app:/settings"), err);

	open_project(project_path);
}

void project_manager::load_config()
{
	fs::error_code err;
	const fs::path project_config_file = fs::resolve_protocol("editor_data:/config/project.cfg");
	if(!fs::exists(project_config_file, err))
	{
		save_config();
	}
	else
	{
		std::ifstream output(project_config_file.string());
		cereal::iarchive_associative_t ar(output);

		try_load(ar, cereal::make_nvp("options", _options));

		auto& items = _options.recent_project_paths;
		auto iter = std::begin(items);
		while(iter != items.end())
		{
			auto& item = *iter;

			if(!fs::exists(item, err))
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
	auto project_path = fs::resolve_protocol("app:/");
	if(project_path != "" &&
	   std::find(std::begin(rp), std::end(rp), project_path.generic_string()) == std::end(rp))
	{
		rp.push_back(project_path.generic_string());
	}

	fs::error_code err;
	fs::create_directory(fs::resolve_protocol("editor_data:/config"), err);
	const std::string project_config_file = fs::resolve_protocol("editor_data:/config/project.cfg").string();
	std::ofstream output(project_config_file);
	cereal::oarchive_associative_t ar(output);

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
