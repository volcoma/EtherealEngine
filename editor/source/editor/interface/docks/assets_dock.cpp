#include "docks.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/shader.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/assets/asset_extensions.h"
#include "runtime/system/filesystem.h"
#include "runtime/ecs/prefab.h"
#include "runtime/ecs/scene.h"
#include "runtime/ecs/utils.h"
#include "runtime/input/input.h"
#include "../../edit_state.h"
#include "../../project.h"
#include "../../filedialog/filedialog.h"
#include <cstdio>

static float scale_icons = 0.7f;

template<typename T>
AssetHandle<Texture> get_asset_icon(T asset)
{
	auto es = core::get_subsystem<editor::EditState>();
	return es->icons["folder"];
}

template<>
AssetHandle<Texture> get_asset_icon(AssetHandle<Texture> asset)
{
	return asset;
}
template<>
AssetHandle<Texture> get_asset_icon(AssetHandle<Prefab> asset)
{
	auto es = core::get_subsystem<editor::EditState>();
	return es->icons["prefab"];
}

template<>
AssetHandle<Texture> get_asset_icon(AssetHandle<Scene> asset)
{
	auto es = core::get_subsystem<editor::EditState>();
	return es->icons["scene"];
}

template<>
AssetHandle<Texture> get_asset_icon(AssetHandle<Mesh> asset)
{
	auto es = core::get_subsystem<editor::EditState>();
	return es->icons["mesh"];
}
template<>
AssetHandle<Texture> get_asset_icon(AssetHandle<Material> asset)
{
	auto es = core::get_subsystem<editor::EditState>();
	return es->icons["material"];
}

AssetHandle<Texture> get_loading_icon()
{
	auto es = core::get_subsystem<editor::EditState>();
	return es->icons["loading"];
}

template<>
AssetHandle<Texture> get_asset_icon(AssetHandle<Shader> asset)
{
	auto es = core::get_subsystem<editor::EditState>();
	return es->icons["shader"];
}

AssetHandle<Texture>& get_icon()
{
	static AssetHandle<Texture> tex;
	return tex;
}

template<typename Wrapper, typename T>
int list_item(Wrapper& entry, const std::string& name, const std::string& relative, const fs::path& absolute, runtime::AssetManager& manager, runtime::Input& input, editor::EditState& edit_state)
{
	auto& selected = edit_state.selection_data.object;
	const float size = 88.0f * scale_icons;
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

	if (rttr::type::get<T>() == rttr::type::get<editor::AssetFolder>())
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
			}
			edit_state.unselect();
		}
	}

	bool loading = !entry;

	AssetHandle<Texture>& icon = get_icon();

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
		ImGuiInputTextFlags_EnterReturnsTrue);

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
		auto& dragged = edit_state.drag_data.object;
		if (is_directory && dragged)
		{
			if(dragged.is_type<AssetHandle<Texture>>() ||
			dragged.is_type<AssetHandle<Mesh>>() ||
			dragged.is_type<AssetHandle<Shader>>() ||
			dragged.is_type<AssetHandle<Material>>() ||
			dragged.is_type<AssetHandle<Prefab>>() ||
			dragged.is_type<AssetHandle<Scene>>())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
			}
		}

		if (gui::IsMouseClicked(gui::drag_button) && !edit_state.drag_data.object)
		{
			edit_state.drag(entry, relative);
		}
	}
	

	
	gui::PopID();
	gui::SameLine();

	return action;
};


void list_dir(editor::AssetFolder* dir, 
	std::shared_ptr<runtime::TStorage<Mesh>> meshes, 
	std::shared_ptr<runtime::TStorage<Texture>> textures, 
	std::shared_ptr<runtime::TStorage<Shader>> shaders, 
	std::shared_ptr<runtime::TStorage<Material>> materials,
	std::shared_ptr<runtime::TStorage<Prefab>> prefabs,
	std::shared_ptr<runtime::TStorage<Scene>> scenes)
{
	if (!dir)
		return;


	auto es = core::get_subsystem<editor::EditState>();
	auto am = core::get_subsystem<runtime::AssetManager>();
	auto input = core::get_subsystem<runtime::Input>();
	{
		std::unique_lock<std::mutex> lock(dir->directories_mutex);

		for (auto& entry : dir->directories)
		{
			int action = list_item<std::shared_ptr<editor::AssetFolder>, editor::AssetFolder>(
				entry,
				entry->name, 
				entry->relative,
				entry->absolute,
				*am, *input, *es);
			if (action == 3)
			{
				editor::AssetFolder::opened = entry;
			}
		}
	}
	{
		std::unique_lock<std::mutex> lock(dir->files_mutex);

		for (auto& file : dir->files)
		{
			if (file.extension == extensions::texture)
			{

				auto& request = textures->container[file.relative];
				AssetHandle<Texture> asset = request.asset;

				list_item<AssetHandle<Texture>, Texture>(
					asset,
					file.name,
					file.relative,
					file.absolute,
					*am, *input, *es);
			}
			if (file.extension == extensions::mesh)
			{

				auto& request = meshes->container[file.relative];
				AssetHandle<Mesh> asset = request.asset;

				list_item<AssetHandle<Mesh>, Mesh>(
					asset,
					file.name, 
					file.relative, 
					file.absolute,
					*am, *input, *es);
			}
			if (file.extension == extensions::material)
			{

				auto& request = materials->container[file.relative];
				AssetHandle<Material> asset = request.asset;
				
				list_item<AssetHandle<Material>, Material>(
					asset,
					file.name,
					file.relative,
					file.absolute, 
					*am, *input, *es);
			}
			if (file.extension == extensions::prefab)
			{

				auto& request = prefabs->container[file.relative];
				AssetHandle<Prefab> asset = request.asset;

				list_item<AssetHandle<Prefab>, Prefab>(
					asset, 
					file.name,
					file.relative,
					file.absolute, 
					*am, *input, *es);
			}
			if (file.extension == extensions::scene)
			{

				auto& request = scenes->container[file.relative];
				AssetHandle<Scene> asset = request.asset;

				int action = list_item<AssetHandle<Scene>, Scene>(
					asset,
					file.name,
					file.relative,
					file.absolute,
					*am, *input, *es);

				if (action == 3)
				{
					if (asset)
					{
						auto es = core::get_subsystem<editor::EditState>();
						auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();

						ecs->dispose();
						es->load_editor_camera();
						asset->instantiate();
						es->scene = fs::resolve_protocol(asset.id()).string() + extensions::scene;

					}
					
				}
			}
			if (file.extension == extensions::shader)
			{

				auto& request = shaders->container[file.relative];
				AssetHandle<Shader> asset = request.asset;

				list_item<AssetHandle<Shader>, Shader>(
					asset,
					file.name,
					file.relative,
					file.absolute,
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
				editor::AssetFolder* folder = editor::AssetFolder::opened.get();

				int i = 0;
				fs::path parent_dir = folder->absolute;
				while (!fs::create_directory(parent_dir / string_utils::format("New Folder (%d)", i), std::error_code{}))
				{
					++i;
				}
			}

			gui::Separator();

			if (gui::MenuItem("Material"))
			{
				editor::AssetFolder* folder = editor::AssetFolder::opened.get();

				AssetHandle<Material> asset;
				fs::path parent_dir = folder->relative;
				asset.link->id = (parent_dir / string_utils::format("New Material (%s)", fs::path(std::tmpnam(nullptr)).filename().string().c_str())).generic_string();
				asset.link->asset = std::make_shared<StandardMaterial>();
				am->save<Material>(asset);
			}

			gui::EndMenu();
		}

		gui::Separator();

		if (gui::Selectable("Open In Explorer"))
		{
			editor::AssetFolder* folder = editor::AssetFolder::opened.get();
			fs::show_in_graphical_env(folder->absolute);
		}

		gui::EndPopup();
	}

}

namespace Docks
{
	void render_assets(ImVec2 area)
	{
		auto es = core::get_subsystem<editor::EditState>();
		auto am = core::get_subsystem<runtime::AssetManager>();
		auto input = core::get_subsystem<runtime::Input>();
		auto meshes = am->get_storage<Mesh>();
		auto textures = am->get_storage<Texture>();
		auto materials = am->get_storage<Material>();
		auto prefabs = am->get_storage<Prefab>();
		auto scenes = am->get_storage<Scene>();
		auto shaders = am->get_storage<Shader>();

		float width = gui::GetContentRegionAvailWidth();

		if (!gui::IsAnyItemActive())
		{
			if (input->is_key_pressed(sf::Keyboard::BackSpace))
			{
				if (editor::AssetFolder::opened && editor::AssetFolder::opened->parent)
					editor::AssetFolder::opened = editor::AssetFolder::opened->parent->make_shared();
			}
		}
			

		if (gui::Button("Import..."))
		{
			std::vector<std::string> paths;
			if (open_multiple_files_dialog("obj,png,tga,dds,ktx,pvr,sc,io,sh", "", paths))
			{
				auto ts = core::get_subsystem<runtime::TaskSystem>();
				auto logger = logging::get("Log");
				editor::AssetFolder* folder = editor::AssetFolder::opened.get();

				fs::path opened_dir = folder->absolute;
				for (auto& path : paths)
				{
					fs::path p = path;
					fs::path ext = p.extension().string();
					fs::path filename = p.filename();

					if (ext == ".obj")
					{
						auto task = ts->create("Import Asset", [opened_dir](const fs::path& path, const fs::path& p, const fs::path& filename)
						{
							std::error_code error;
							fs::path dir = opened_dir / filename;
							if (!fs::copy_file(path, dir, fs::copy_options::overwrite_existing, error))
							{
								auto logger = logging::get("Log");
								logger->error().write("Failed to import file {0} with message {1}", p.string(), error.message());
							}
							else
							{
								fs::last_write_time(dir, fs::file_time_type::clock::now(), std::error_code{});
							}
						}, p, p, filename);

						ts->run(task);
					}
					else if (ext == ".png" || ext == ".tga" || ext == ".dds" || ext == ".ktx" || ext == ".pvr")
					{
						auto task = ts->create("Import Asset", [opened_dir](const fs::path& path, const fs::path& p, const fs::path& filename)
						{
							std::error_code error;
							fs::path dir = opened_dir / filename;
							if (!fs::copy_file(path, dir, fs::copy_options::overwrite_existing, error))
							{
								auto logger = logging::get("Log");
								logger->error().write("Failed to import file {0} with message {1}", p.string(), error.message());
							}
							else
							{
								fs::last_write_time(dir, fs::file_time_type::clock::now(), std::error_code{});
							}
						}, p, p, filename);

						ts->run(task);
					}
					else if (ext == ".sc" || ext == ".io" || ext == ".sh")
					{
						auto task = ts->create("Import Asset", [opened_dir](const fs::path& path, const fs::path& p, const fs::path& filename)
						{
							std::error_code error;
							fs::path dir = opened_dir / filename;
							if (!fs::copy_file(path, dir, fs::copy_options::overwrite_existing, error))
							{
								auto logger = logging::get("Log");
								logger->error().write("Failed to import file {0} with message {1}", p.string(), error.message());
							}
							else
							{
								fs::last_write_time(dir, fs::file_time_type::clock::now(), std::error_code{});
							}
						}, p, p, filename);

						ts->run(task);
					}
					else
					{
						logger->error().write("Unsupported file format {0}", ext.string());
					}
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

		std::vector<editor::AssetFolder*> hierarchy;
		editor::AssetFolder* folder = editor::AssetFolder::opened.get();

		editor::AssetFolder* f = folder;
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
				editor::AssetFolder::opened = (*rit)->make_shared();
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
		if (gui::BeginChild("###assets_content", gui::GetContentRegionAvail(), false, flags))
		{
			if (gui::IsWindowHovered())
			{
				auto& dragged = es->drag_data.object;
				if (dragged && dragged.is_type<runtime::Entity>())
				{
					gui::SetMouseCursor(ImGuiMouseCursor_Move);
					if (gui::IsMouseReleased(gui::drag_button))
					{
						auto entity = dragged.get_value<runtime::Entity>();
						ecs::utils::save_entity(folder->absolute, entity);
						es->drop();
					}
				}
			}
			
			get_icon() = AssetHandle<Texture>();

			auto current_folder = editor::AssetFolder::opened;	
			list_dir(current_folder.get(), meshes, textures, shaders, materials, prefabs, scenes);
			get_icon() = AssetHandle<Texture>();
			gui::EndChild();
		}

	}

};