#include "project_manager.h"
#include "../assets/asset_compiler.h"
#include "../editing/editing_system.h"
#include "../meta/system/project_manager.hpp"
#include "core/filesystem/filesystem_watcher.h"
#include "core/logging/logging.h"
#include "core/serialization/associative_archive.h"
#include "core/system/subsystem.h"
#include "core/tasks/task_system.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/assets/impl/asset_extensions.h"
#include "runtime/ecs/ecs.h"
#include "runtime/system/events.h"
#include <fstream>

class mesh;
struct prefab;
struct scene;
class material;

namespace gfx
{
struct texture;
struct shader;
}

namespace audio
{
class sound;
}

namespace runtime
{
struct animation;
}

namespace editor
{
using namespace std::literals;

template <typename T>
static uint64_t watch_assets(const fs::path& protocol, const std::string& wildcard, bool reload_async,
				  bool force_initial_recompile)
{
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& ts = core::get_subsystem<core::task_system>();

	const fs::path dir = fs::resolve_protocol(protocol);
	fs::path watch_dir = (dir / wildcard).make_preferred();

	return fs::watcher::watch(
		watch_dir, true, true, 500ms,
		[&am, &ts, protocol, reload_async,
		 force_initial_recompile](const std::vector<fs::watcher::entry>& entries, bool is_initial_list) {
			for(const auto& entry : entries)
			{
				const auto& p = entry.path;
				const auto ext = p.extension().string();
				const bool is_compiled = extensions::is_compiled_format(ext);
				if(is_compiled)
				{
                    auto path = p;
                    if(ext == extensions::compiled)
                    {
                        path = fs::reduce_trailing_extensions(p);
                    }
					
					auto key = fs::convert_to_protocol(path).generic_string();

					if(entry.type == fs::file_type::regular_file)
					{
						if(entry.status == fs::watcher::entry_status::removed)
						{
							auto task = ts.push_on_owner_thread(
								[reload_async, key, &am]() { am.clear_asset<T>(key); });
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
							auto task = ts.push_on_worker_thread(
								[mode, flags, key, &am]() { am.load<T>(key, mode, flags); });
						}
					}
				}
				else
				{
					auto key = fs::convert_to_protocol(p).generic_string();
					if(entry.type == fs::file_type::regular_file)
					{
						if(entry.status == fs::watcher::entry_status::removed)
						{
							auto task = ts.push_on_owner_thread([p, key, &am]() {
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
								auto task = ts.push_on_worker_thread(
									[](const fs::path& p) { asset_compiler::compile<T>(p); }, p);
							}
						}
					}
				}
			}
		});
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
	
	for(const auto& id : _watch_ids)
	{
        fs::watcher::unwatch(id);
	}
	_watch_ids.clear();
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

    static const std::string wildcard = "*";
    static const fs::path root_path = "app:/data";
    _watch_ids.clear();
	for(const auto& format : ex::get_suported_texture_formats())
	{
		_watch_ids.push_back(watch_assets<gfx::texture>(root_path, wildcard + format, true, false));
	}
    for(const auto& format : ex::get_suported_shader_formats())
	{
		_watch_ids.push_back(watch_assets<gfx::shader>(root_path, wildcard + format, true, false));
	}
	for(const auto& format : ex::get_suported_mesh_formats())
	{
		_watch_ids.push_back(watch_assets<mesh>(root_path, wildcard + format, true, false));
	}
	for(const auto& format : ex::get_suported_sound_formats())
	{
		_watch_ids.push_back(watch_assets<audio::sound>(root_path, wildcard + format, true, false));
	}
    for(const auto& format : ex::get_suported_animation_formats())
	{
		_watch_ids.push_back(watch_assets<runtime::animation>(root_path, wildcard + format, true, false));
	}
	for(const auto& format : ex::get_suported_material_formats())
	{
		_watch_ids.push_back(watch_assets<material>(root_path, wildcard + format, true, false));
	}
	for(const auto& format : ex::get_suported_prefab_formats())
	{
		_watch_ids.push_back(watch_assets<prefab>(root_path, wildcard + format, true, false));
	}
	for(const auto& format : ex::get_suported_scene_formats())
	{
		_watch_ids.push_back(watch_assets<scene>(root_path, wildcard + format, true, false));
	}
	/// for debug purposes
	for(const auto& format : ex::get_suported_shader_formats())
	{
		_watch_ids.push_back(watch_assets<gfx::shader>("engine_data:/shaders", wildcard + format, true, false));
        _watch_ids.push_back(watch_assets<gfx::shader>("editor_data:/shaders", wildcard + format, true, false));
	}

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

project_manager::project_manager()
{
	load_config();
}

project_manager::~project_manager()
{
    close_project();
	save_config();
}
}
