#include "project_manager.h"
#include "../assets/asset_compiler.h"
#include "../assets/asset_extensions.h"
#include "../editing/editing_system.h"
#include "../meta/system/project_manager.hpp"
#include "core/filesystem/filesystem_watcher.h"
#include "core/graphics/graphics.h"
#include "core/logging/logging.h"
#include "core/serialization/associative_archive.h"
#include "core/system/subsystem.h"
#include "core/tasks/task_system.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/ecs/ecs.h"
#include "runtime/system/events.h"
#include <fstream>

namespace editor
{
using namespace std::literals;

template <typename T>
std::uint64_t watch_assets(const fs::path& dir, const std::string& wildcard, bool reload_async)
{
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& ts = core::get_subsystem<core::task_system>();

	fs::path watch_dir = (dir / wildcard).make_preferred();

	return fs::watcher::watch(
		watch_dir, true, true, 500ms, [&am, &ts, reload_async](const auto& entries, bool is_initial_list) {
			for(const auto& entry : entries)
			{
				auto p = fs::reduce_trailing_extensions(entry.path);
				auto data_key = fs::convert_to_protocol(p);
				auto key = fs::replace(data_key.generic(), ":/cache", ":/data").string();

				if(entry.type == fs::file_type::regular_file)
				{
					if(entry.status == fs::watcher::entry_status::removed)
					{
						auto task =
							ts.push_on_owner_thread([reload_async, key, &am]() { am.clear_asset<T>(key); });
					}
					else if(entry.status == fs::watcher::entry_status::renamed)
					{
						auto old_p = fs::reduce_trailing_extensions(entry.last_path);
						auto old_data_key = fs::convert_to_protocol(old_p);
						auto old_key = fs::replace(old_data_key.generic(), ":/cache", ":/data").string();
						auto task = ts.push_on_owner_thread(
							[reload_async, old_key, key, &am]() { am.rename_asset<T>(old_key, key); });
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

		});
}

void project_manager::close_project()
{
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();
	auto& es = core::get_subsystem<editing_system>();
	es.close_project();
	ecs.dispose();
	am.clear("app:/data");
	_app_meta_syncer.unsync();
	_app_cache_syncer.unsync();
}

bool project_manager::open_project(const fs::path& project_path)
{
	close_project();

	fs::error_code err;
	if(!fs::exists(project_path, err))
	{
		APPLOG_ERROR("Project directory doesn't exist {0}", project_path.string());
		return false;
	}

	fs::add_path_protocol("app:", project_path);

	set_name(project_path.filename().string());

	save_config();

	setup_meta_syncer(_app_meta_syncer, fs::resolve_protocol("app:/data"), fs::resolve_protocol("app:/meta"));
	setup_cache_syncer(_app_cache_syncer, fs::resolve_protocol("app:/meta"),
					   fs::resolve_protocol("app:/cache"));

	auto& es = core::get_subsystem<editing_system>();
	es.load_editor_camera();
	return true;
}

void project_manager::create_project(const fs::path& project_path)
{
	fs::error_code err;
	fs::add_path_protocol("app:", project_path);
	fs::create_directory(fs::resolve_protocol("app:/data"), err);
	fs::create_directory(fs::resolve_protocol("app:/cache"), err);
	fs::create_directory(fs::resolve_protocol("app:/meta"), err);
	fs::create_directory(fs::resolve_protocol("app:/settings"), err);

	open_project(project_path);
}

void project_manager::load_config()
{
	fs::error_code err;
	const fs::path project_config_file = fs::resolve_protocol("editor:/config/project.cfg");
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

void project_manager::setup_directory(fs::syncer& syncer)
{
	const auto on_dir_modified = [](const auto& ref_path, const auto& synced_paths, bool is_initial_listing) {
		for(const auto& synced_path : synced_paths)
		{
		}
	};
	const auto on_dir_removed = [](const auto& ref_path, const auto& synced_paths) {
		for(const auto& synced_path : synced_paths)
		{
			fs::error_code err;
			fs::remove_all(synced_path, err);
		}
	};

	const auto on_dir_renamed = [](const auto& ref_path, const auto& synced_paths) {
		for(const auto& synced_path : synced_paths)
		{
			fs::error_code err;
			fs::rename(synced_path.first, synced_path.second, err);
		}
	};
	syncer.set_directory_mapping(on_dir_modified, on_dir_modified, on_dir_removed, on_dir_renamed);
}

void project_manager::setup_meta_syncer(fs::syncer& syncer, const fs::path& data_dir_protocol,
										const fs::path& meta_dir_protocol)
{

	setup_directory(syncer);

	const auto on_file_removed = [](const auto& ref_path, const auto& synced_paths) {
		for(const auto& synced_path : synced_paths)
		{
			fs::error_code err;
			fs::remove_all(synced_path, err);
		}
	};

	const auto on_file_renamed = [](const auto& ref_path, const auto& synced_paths) {
		for(const auto& synced_path : synced_paths)
		{
			fs::error_code err;
			fs::rename(synced_path.first, synced_path.second, err);
		}
	};

	const auto on_file_modified = [](const auto& ref_path, const auto& synced_paths,
									 bool is_initial_listing) {

		for(const auto& synced_path : synced_paths)
		{
			fs::error_code err;
			if(is_initial_listing && fs::exists(synced_path, err))
			{
				return;
			}
			std::ofstream output(synced_path.string(), std::ofstream::trunc);
			output.write("metadata", 8);
		}
	};

	static const std::vector<std::vector<std::string>> types = ex::get_all_formats();

	for(const auto& asset_set : types)
	{
		for(const auto& type : asset_set)
		{
			syncer.set_mapping(type, {".meta"}, on_file_modified, on_file_modified, on_file_removed,
							   on_file_renamed);
		}
	}

	syncer.sync(data_dir_protocol, meta_dir_protocol);
}

static std::vector<fs::path> remove_meta_tag(const std::vector<fs::path>& synced_paths)
{
	std::decay_t<decltype(synced_paths)> reduced;
	reduced.reserve(synced_paths.size());
	for(const auto& synced_path : synced_paths)
	{
		reduced.emplace_back(fs::replace(synced_path, ".meta", ""));
	}
	return reduced;
}

void project_manager::setup_cache_syncer(fs::syncer& syncer, const fs::path& meta_dir_protocol,
										 const fs::path& cache_dir_protocol)
{
	auto& ts = core::get_subsystem<core::task_system>();

	setup_directory(syncer);
	static const std::string wildcard = "*";

	auto on_removed = [](const auto& ref_path, const auto& synced_paths) {
		for(const auto& synced_path : synced_paths)
		{
			auto synced_asset = fs::replace(synced_path, ".meta", "");
			fs::error_code err;
			fs::remove_all(synced_asset, err);
		}
	};

	auto on_renamed = [](const auto& ref_path, const auto& synced_paths) {
		for(const auto& synced_path : synced_paths)
		{
			auto synced_old_asset = fs::replace(synced_path.first, ".meta", "");
			auto synced_new_asset = fs::replace(synced_path.second, ".meta", "");
			fs::error_code err;
			fs::rename(synced_old_asset, synced_new_asset, err);
		}
	};

	auto on_image_modified = [&ts](const auto& ref_path, const auto& synced_paths, bool is_initial_listing) {

		auto task = ts.push_on_worker_thread(
			[ ref_path, synced_paths = remove_meta_tag(synced_paths), is_initial_listing ]() {

				fs::path output = synced_paths.front();
				fs::error_code err;
				if(is_initial_listing && fs::exists(output, err))
				{
					return;
				}
				project_compiler::compile_texture(ref_path, output);
			});

	};

	for(const auto& type : ex::get_suported_formats<gfx::texture>())
	{
		syncer.set_mapping(type + ".meta", {".asset"}, on_image_modified, on_image_modified, on_removed,
						   on_renamed);
		watch_assets<gfx::texture>(cache_dir_protocol, wildcard + type, true);
	}

	auto on_mesh_modified = [&ts](const auto& ref_path, const auto& synced_paths, bool is_initial_listing) {
		auto task = ts.push_on_worker_thread(
			[ ref_path, synced_paths = remove_meta_tag(synced_paths), is_initial_listing ]() {
				fs::path output = synced_paths.front();
				fs::error_code err;
				if(is_initial_listing && fs::exists(output, err))
				{
					return;
				}
				project_compiler::compile_mesh(ref_path, output);
			});

	};
	for(const auto& type : ex::get_suported_formats<mesh>())
	{
		syncer.set_mapping(type + ".meta", {".asset"}, on_mesh_modified, on_mesh_modified, on_removed,
						   on_renamed);
		watch_assets<mesh>(cache_dir_protocol, wildcard + type, true);
	}

	auto on_sound_modified = [&ts](const auto& ref_path, const auto& synced_paths, bool is_initial_listing) {
		auto task = ts.push_on_worker_thread(
			[ ref_path, synced_paths = remove_meta_tag(synced_paths), is_initial_listing ]() {
				fs::path output = synced_paths.front();
				fs::error_code err;
				if(is_initial_listing && fs::exists(output, err))
				{
					return;
				}
				project_compiler::compile_sound(ref_path, output);
			});

	};
	for(const auto& type : ex::get_suported_formats<audio::sound>())
	{
		syncer.set_mapping(type + ".meta", {".asset"}, on_sound_modified, on_sound_modified, on_removed,
						   on_renamed);

		watch_assets<audio::sound>(cache_dir_protocol, wildcard + type, true);
	}

	auto on_shader_modified = [&ts](const auto& ref_path, const auto& synced_paths, bool is_initial_listing) {
		auto task = ts.push_on_worker_thread(
			[ ref_path, synced_paths = remove_meta_tag(synced_paths), is_initial_listing ]() {
				const auto& renderer_extension = gfx::get_renderer_filename_extension();
				auto it = std::find_if(std::begin(synced_paths), std::end(synced_paths),
									   [&renderer_extension](const auto& key) {
										   return key.stem().extension() == renderer_extension;
									   });

				if(it == std::end(synced_paths))
				{
					return;
				}

				fs::path output = *it;

				fs::error_code err;
				if(is_initial_listing && fs::exists(output, err))
				{
					return;
				}

				project_compiler::compile_shader(ref_path, output);
			});

	};

	for(const auto& type : ex::get_suported_formats<gfx::shader>())
	{
		syncer.set_mapping(type + ".meta", {".dx11.asset", ".dx12.asset", ".gl.asset"}, on_shader_modified,
						   on_shader_modified, on_removed, on_renamed);

		watch_assets<gfx::shader>(cache_dir_protocol, wildcard + type, true);
	}

	auto on_material_modified = [&ts](const auto& ref_path, const auto& synced_paths,
									  bool is_initial_listing) {
		auto task = ts.push_on_worker_thread(
			[ ref_path, synced_paths = remove_meta_tag(synced_paths), is_initial_listing ]() {
				fs::path output = synced_paths.front();
				fs::error_code err;
				if(is_initial_listing && fs::exists(output, err))
				{
					return;
				}
				project_compiler::compile_material(ref_path, output);
			});

	};

	for(const auto& type : ex::get_suported_formats<material>())
	{
		syncer.set_mapping(type + ".meta", {".asset"}, on_material_modified, on_material_modified, on_removed,
						   on_renamed);

		watch_assets<material>(cache_dir_protocol, wildcard + type, true);
	}

	auto on_anim_modified = [&ts](const auto& ref_path, const auto& synced_paths, bool is_initial_listing) {
		auto task = ts.push_on_worker_thread(
			[ ref_path, synced_paths = remove_meta_tag(synced_paths), is_initial_listing ]() {
				fs::path output = synced_paths.front();
				fs::error_code err;
				if(is_initial_listing && fs::exists(output, err))
				{
					return;
				}
				project_compiler::compile_animation(ref_path, output);
			});
	};
	for(const auto& type : ex::get_suported_formats<runtime::animation>())
	{
		syncer.set_mapping(type + ".meta", {".asset"}, on_anim_modified, on_anim_modified, on_removed,
						   on_renamed);

		watch_assets<runtime::animation>(cache_dir_protocol, wildcard + type, true);
	}

	auto on_prefab_modified = [&ts](const auto& ref_path, const auto& synced_paths, bool is_initial_listing) {
		auto task = ts.push_on_worker_thread(
			[ ref_path, synced_paths = remove_meta_tag(synced_paths), is_initial_listing ]() {
				fs::path output = synced_paths.front();
				fs::error_code err;
				if(is_initial_listing && fs::exists(output, err))
				{
					return;
				}
				project_compiler::compile_prefab(ref_path, output);
			});

	};

	for(const auto& type : ex::get_suported_formats<prefab>())
	{
		syncer.set_mapping(type + ".meta", {".asset"}, on_prefab_modified, on_prefab_modified, on_removed,
						   on_renamed);

		watch_assets<prefab>(cache_dir_protocol, wildcard + type, true);
	}

	auto on_scene_modified = [&ts](const auto& ref_path, const auto& synced_paths, bool is_initial_listing) {
		auto task = ts.push_on_worker_thread(
			[ ref_path, synced_paths = remove_meta_tag(synced_paths), is_initial_listing ]() {
				fs::path output = synced_paths.front();
				fs::error_code err;
				if(is_initial_listing && fs::exists(output, err))
				{
					return;
				}
				project_compiler::compile_scene(ref_path, output);
			});

	};
	for(const auto& type : ex::get_suported_formats<scene>())
	{
		syncer.set_mapping(type + ".meta", {".asset"}, on_scene_modified, on_scene_modified, on_removed,
						   on_renamed);

		watch_assets<scene>(cache_dir_protocol, wildcard + type, true);
	}
	syncer.sync(meta_dir_protocol, cache_dir_protocol);
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
	fs::create_directory(fs::resolve_protocol("editor:/config"), err);
	const std::string project_config_file = fs::resolve_protocol("editor:/config/project.cfg").string();
	std::ofstream output(project_config_file);
	cereal::oarchive_associative_t ar(output);

	try_save(ar, cereal::make_nvp("options", _options));
}

project_manager::project_manager()
{
	load_config();
	setup_meta_syncer(_engine_meta_syncer, fs::resolve_protocol("engine:/data"),
					  fs::resolve_protocol("engine:/meta"));
	setup_cache_syncer(_engine_cache_syncer, fs::resolve_protocol("engine:/meta"),
					   fs::resolve_protocol("engine:/cache"));
	setup_meta_syncer(_editor_meta_syncer, fs::resolve_protocol("editor:/data"),
					  fs::resolve_protocol("editor:/meta"));
	setup_cache_syncer(_editor_cache_syncer, fs::resolve_protocol("editor:/meta"),
					   fs::resolve_protocol("editor:/cache"));
}

project_manager::~project_manager()
{
	save_config();
}
}
