#include "assets_dock.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/shader.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/assets/asset_extensions.h"
#include "runtime/system/filesystem.h"
#include "runtime/system/filesystem_watcher.hpp"
#include "runtime/ecs/prefab.h"
#include "runtime/ecs/scene.h"
#include "runtime/ecs/utils.h"
#include "runtime/input/input.h"
#include "../../edit_state.h"
#include "../../project.h"
#include "../../filedialog/filedialog.h"
#include <cstdio>

template<typename T>
asset_handle<texture> get_asset_icon(T asset)
{
	auto es = core::get_subsystem<editor::editor_state>();
	return es->icons["folder"];
}

template<>
asset_handle<texture> get_asset_icon(asset_handle<texture> asset)
{
	return asset;
}
template<>
asset_handle<texture> get_asset_icon(asset_handle<prefab> asset)
{
	auto es = core::get_subsystem<editor::editor_state>();
	return es->icons["prefab"];
}

template<>
asset_handle<texture> get_asset_icon(asset_handle<scene> asset)
{
	auto es = core::get_subsystem<editor::editor_state>();
	return es->icons["scene"];
}

template<>
asset_handle<texture> get_asset_icon(asset_handle<mesh> asset)
{
	auto es = core::get_subsystem<editor::editor_state>();
	return es->icons["mesh"];
}
template<>
asset_handle<texture> get_asset_icon(asset_handle<material> asset)
{
	auto es = core::get_subsystem<editor::editor_state>();
	return es->icons["material"];
}

asset_handle<texture> get_loading_icon()
{
	auto es = core::get_subsystem<editor::editor_state>();
	return es->icons["loading"];
}

template<>
asset_handle<texture> get_asset_icon(asset_handle<shader> asset)
{
	auto es = core::get_subsystem<editor::editor_state>();
	return es->icons["shader"];
}

asset_handle<texture>& get_icon()
{
	static asset_handle<texture> tex;
	return tex;
}

template<typename Wrapper, typename T>
int list_item(Wrapper& entry,
	const std::string& name,
	const std::string& relative,
	const fs::path& absolute,
	const float size,
	std::weak_ptr<editor::asset_directory> opened_dir,
	runtime::asset_manager& manager,
	runtime::input& input, 
	editor::editor_state& edit_state)
{
	auto& selected = edit_state.selection_data.object;
	int action = 0;
	bool already_selected = false;
	if (selected.is_type<Wrapper>())
	{
		if (selected.get_value<Wrapper>() == entry)
		{
			already_selected = true;
		}
	}

	bool is_directory = false;

	if (rttr::type::get<T>() == rttr::type::get<editor::asset_directory>())
		is_directory = true;

	bool edit_label = false;
	if (already_selected && !gui::IsAnyItemActive())
	{
		if (input.is_key_pressed(sf::Keyboard::F2))
		{
			edit_label = true;
		}

		if (input.is_key_pressed(sf::Keyboard::Delete))
		{
			if (is_directory)
			{
				fs::remove_all(absolute, std::error_code{});
			}
			else
			{
				manager.delete_asset<T>(relative);

				if (!opened_dir.expired())
				{
					auto opened_folder_shared = opened_dir.lock();
					auto& files = opened_folder_shared->files;

					for (auto& file : files)
					{
						if (file.relative == relative)
						{
							fs::remove(file.absolute, std::error_code{});
						}
					}
				}
			}
			edit_state.unselect();
		}
	}

	bool loading = !entry;

	asset_handle<texture>& icon = get_icon();

	if (loading)
		icon = get_loading_icon();
	else
		icon = get_asset_icon(entry);

	gui::PushID(relative.c_str());

	if (gui::GetContentRegionAvailWidth() < size)
		gui::NewLine();

	static std::string inputBuff(64, 0);
	std::memset(&inputBuff[0], 0, 64);
	std::memcpy(&inputBuff[0], name.c_str(), name.size() < 64 ? name.size() : 64);

	ImVec2 item_size = { size, size };
	ImVec2 texture_size = item_size;
	if (icon)
		texture_size = { float(icon->info.width), float(icon->info.height) };
	ImVec2 uv0 = { 0.0f, 0.0f };
	ImVec2 uv1 = { 1.0f, 1.0f };

	bool* edit = edit_label ? &edit_label : nullptr;
	action = gui::ImageButtonWithAspectAndLabel(
		icon.link->asset,
		texture_size, item_size, uv0, uv1,
		already_selected,
		edit,
		loading ? "Loading" : name.c_str(),
		&inputBuff[0],
		inputBuff.size(),
		ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_AutoSelectAll);

	if (loading)
	{
		gui::PopID();
		gui::SameLine();
		return action;
	}

	if (action == 1)
	{
		edit_state.select(entry);
	}
	else if (action == 2)
	{
		std::string new_name = std::string(inputBuff.c_str());
		if (new_name != name && new_name != "")
		{
			if (is_directory)
			{
				fs::path new_absolute_path = absolute;
				new_absolute_path.remove_filename();
				new_absolute_path /= new_name;
				fs::rename(absolute, new_absolute_path, std::error_code{});
			}
			else
			{
				const auto asset_dir = fs::path(relative).remove_filename();
				std::string new_relative = (asset_dir / new_name).generic_string();
				manager.rename_asset<T>(relative, new_relative);
			}
		}
	}

	if (gui::IsItemHoveredRect())
	{
		if (gui::IsMouseClicked(gui::drag_button) && !edit_state.drag_data.object)
		{
			edit_state.drag(entry, relative);
		}

	}


	gui::PopID();
	gui::SameLine();

	return action;
};


void list_dir(std::weak_ptr<editor::asset_directory>& opened_dir, const float size)
{
	if (opened_dir.expired())
		return;

	auto dir = opened_dir.lock().get();
	
	auto es = core::get_subsystem<editor::editor_state>();
	auto am = core::get_subsystem<runtime::asset_manager>();
	auto input = core::get_subsystem<runtime::input>();
	{
		std::unique_lock<std::mutex> lock(dir->directories_mutex);

		for (auto& entry : dir->directories)
		{
			int action = list_item<std::shared_ptr<editor::asset_directory>, editor::asset_directory>(
				entry,
				entry->name,
				entry->relative,
				entry->absolute,
				size,
				opened_dir,
				*am, *input, *es);
			if (action == 3)
			{
				opened_dir = entry;
			}
		}
	}
	{
		std::unique_lock<std::mutex> lock(dir->files_mutex);

		for (auto& file : dir->files)
		{
			if (file.extension == extensions::texture)
			{
				auto asset = am->find_or_create_asset_entry<texture>(file.relative).asset;

				list_item<asset_handle<texture>, texture>(
					asset,
					file.name,
					file.relative,
					file.absolute,
					size,
					opened_dir,
					*am, *input, *es);
			}
			if (file.extension == extensions::mesh)
			{

				auto asset = am->find_or_create_asset_entry<mesh>(file.relative).asset;

				list_item<asset_handle<mesh>, mesh>(
					asset,
					file.name,
					file.relative,
					file.absolute,
					size,
					opened_dir,
					*am, *input, *es);
			}
			if (file.extension == extensions::material)
			{
				auto asset = am->find_or_create_asset_entry<material>(file.relative).asset;

				list_item<asset_handle<material>, material>(
					asset,
					file.name,
					file.relative,
					file.absolute,
					size,
					opened_dir,
					*am, *input, *es);
			}
			if (file.extension == extensions::prefab)
			{
				auto asset = am->find_or_create_asset_entry<prefab>(file.relative).asset;

				list_item<asset_handle<prefab>, prefab>(
					asset,
					file.name,
					file.relative,
					file.absolute,
					size,
					opened_dir,
					*am, *input, *es);
			}
			if (file.extension == extensions::scene)
			{
				auto asset = am->find_or_create_asset_entry<scene>(file.relative).asset;

				int action = list_item<asset_handle<scene>, scene>(
					asset,
					file.name,
					file.relative,
					file.absolute,
					size,
					opened_dir,
					*am, *input, *es);

				if (action == 3)
				{
					if (asset)
					{
						auto es = core::get_subsystem<editor::editor_state>();
						auto ecs = core::get_subsystem<runtime::entity_component_system>();

						ecs->dispose();
						es->load_editor_camera();
						asset->instantiate();
						es->scene = fs::resolve_protocol(asset.id()).string() + extensions::scene;

					}

				}
			}
			if (file.extension == extensions::shader)
			{
				auto asset = am->find_or_create_asset_entry<shader>(file.relative).asset;

				list_item<asset_handle<shader>, shader>(
					asset,
					file.name,
					file.relative,
					file.absolute,
					size,
					opened_dir,
					*am, *input, *es);
			}
		}
	}

	if (gui::BeginPopupContextWindow())
	{
		if (gui::BeginMenu("Create"))
		{
			if (gui::MenuItem("Folder"))
			{
				auto opened_folder_shared = opened_dir.lock();
				auto dir = opened_folder_shared.get();
				if (dir)
				{
					int i = 0;
					while (!fs::create_directory(dir->absolute / string_utils::format("New Folder (%d)", i), std::error_code{}))
					{
						++i;
					}
				}
				
			}

			gui::Separator();

			if (gui::MenuItem("Material"))
			{
				auto opened_folder_shared = opened_dir.lock();
				auto dir = opened_folder_shared.get();
				if (dir)
				{
					asset_handle<material> asset;
					fs::path parent_dir = dir->relative;
					asset.link->id = (parent_dir / string_utils::format("New Material (%s)", fs::path(std::tmpnam(nullptr)).filename().string().c_str())).generic_string();
					asset.link->asset = std::make_shared<standard_material>();
					am->save<material>(asset);
				}
				
			}

			gui::EndMenu();
		}

		gui::Separator();

		if (gui::Selectable("Open In Explorer"))
		{
			auto opened_folder_shared = opened_dir.lock();
			auto dir = opened_folder_shared.get();
			if(dir)
				fs::show_in_graphical_env(dir->absolute);
		}

		gui::EndPopup();
	}

}

void assets_dock::render(const ImVec2& area)
{
	auto project = core::get_subsystem<editor::project_manager>();

	if (opened_dir.expired())
		opened_dir = project->get_root_directory();

	auto es = core::get_subsystem<editor::editor_state>();
	auto input = core::get_subsystem<runtime::input>();

	float width = gui::GetContentRegionAvailWidth();

	if (!gui::IsAnyItemActive())
	{
		if (input->is_key_pressed(sf::Keyboard::BackSpace) && !opened_dir.expired())
		{
			auto opened_folder_shared = opened_dir.lock();

			if (opened_folder_shared->parent)
				opened_dir = opened_folder_shared->parent->shared_from_this();
		}
	}


	if (gui::Button("Import..."))
	{
		std::vector<std::string> paths;
		if (open_multiple_files_dialog("obj,fbx,dae,blend,3ds,mtl,png,jpg,tga,dds,ktx,pvr,sc,io,sh", "", paths))
		{
			auto ts = core::get_subsystem<runtime::task_system>();

			auto opened_folder_shared = opened_dir.lock();

			fs::path opened_dir = opened_folder_shared->absolute;
			for (auto& path : paths)
			{
				fs::path p = path;
				fs::path ext = p.extension().string();
				fs::path filename = p.filename();

				auto task = ts->create("Import Asset", [opened_dir](const fs::path& path, const fs::path& p, const fs::path& filename)
				{
					std::error_code error;
					fs::path dir = opened_dir / filename;
					if (!fs::copy_file(path, dir, fs::copy_options::overwrite_existing, error))
					{
						APPLOG_ERROR("Failed to import file {0} with message {1}", p.string(), error.message());
					}
					else
					{
						fs::last_write_time(dir, fs::file_time_type::clock::now(), std::error_code{});
					}
				}, p, p, filename);

				ts->run(task);
			}
		}
	}
	gui::SameLine();
	gui::PushItemWidth(80.0f);
	gui::SliderFloat("", &scale_icons, 0.5f, 1.0f);
	if (gui::IsItemHovered())
	{
		gui::SetTooltip("Scale Icons");
	}
	gui::PopItemWidth();
	gui::SameLine();

	std::vector<editor::asset_directory*> hierarchy;
	auto dir = opened_dir.lock().get();
	auto f = dir;
	while (f)
	{
		hierarchy.push_back(f);
		f = f->parent;
	}

	for (auto rit = hierarchy.rbegin(); rit != hierarchy.rend(); ++rit)
	{
		if (rit != hierarchy.rbegin())
		{
			gui::Text(">");
			gui::SameLine();
		}

		if (gui::Button((*rit)->name.c_str()))
		{
			opened_dir = (*rit)->shared_from_this();
			break;
		}
		if (rit != hierarchy.rend() - 1)
			gui::SameLine();
	}
	gui::Separator();

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings;
	if (gui::BeginChild("assets_content", gui::GetContentRegionAvail(), false, flags))
	{
		if (gui::IsWindowHovered())
		{
			auto& dragged = es->drag_data.object;
			if (dragged && dragged.is_type<runtime::entity>())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				if (gui::IsMouseReleased(gui::drag_button))
				{
					auto entity = dragged.get_value<runtime::entity>();
					if (entity)
						ecs::utils::save_entity(dir->absolute, entity);
					es->drop();
				}
			}
		}

		get_icon() = asset_handle<texture>();
		list_dir(opened_dir, 88.0f * scale_icons);
		get_icon() = asset_handle<texture>();
		gui::EndChild();
	}

}

assets_dock::assets_dock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize)
{
	initialize(dtitle, dcloseButton, dminSize, std::bind(&assets_dock::render, this, std::placeholders::_1));
}
