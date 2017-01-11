#include "docks.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/shader.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/system/filesystem.h"
#include "runtime/ecs/prefab.h"
#include "runtime/ecs/utils.h"
#include "runtime/input/input.h"
#include "../../edit_state.h"
#include "../../filedialog/filedialog.h"
#include <cstdio>

static float scale_icons = 0.7f;

template<typename T>
AssetHandle<Texture> get_asset_icon(AssetHandle<T> asset)
{
	auto es = core::get_subsystem<EditState>();
	return es->icons["prefab"];
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

namespace Docks
{
	template<typename T>
	bool list_items(std::shared_ptr<runtime::TStorage<T>> storage, runtime::AssetManager& manager, runtime::Input& input, editor::EditState& editState)
	{
		auto& selected = editState.selection_data.object;
		//copy for safe removal from original
		auto container = storage->container;
		bool openPopup = false;

		const float size = 88.0f * scale_icons;
		for (auto& asset : container)
		{
			auto& assetRelativeName = asset.first;
			auto path = fs::path(assetRelativeName);

			if (path.root_path().generic_string() != "data:/")
				continue;

			auto& assetHandle = asset.second.asset;
			const auto assetName = path.filename().string();
			const auto assetDir = path.remove_filename();

			bool alreadySelected = false;
			if (selected.is_type<AssetHandle<T>>())
			{
				if (selected.get_value<AssetHandle<T>>() == assetHandle)
				{
					alreadySelected = true;
				}
			}
			bool loading = !assetHandle;

			AssetHandle<Texture> icon;
			if (loading)
				icon = get_loading_icon();
			else
				icon = get_asset_icon(assetHandle);

			gui::PushID(assetRelativeName.c_str());

			if (gui::GetContentRegionAvailWidth() < size)
				gui::NewLine();

			std::string inputBuff = assetName;
			inputBuff.resize(64, 0);
			inputBuff.shrink_to_fit();


			ImVec2 texture_size = { float(icon->info.width), float(icon->info.height) };
			ImVec2 item_size = { size, size };
			ImVec2 uv0 = { 0.0f, 0.0f };
			ImVec2 uv1 = { 1.0f, 1.0f };

			int action = gui::ImageButtonWithAspectAndLabel(
				icon.link->asset,
				texture_size, item_size, uv0, uv1,
				alreadySelected,
				loading ? "Loading" : assetName.c_str(),
				&inputBuff[0],
				inputBuff.size(),
				ImGuiInputTextFlags_EnterReturnsTrue);
			
			if (loading)
			{
				gui::PopID();
				gui::SameLine();
				continue;
			}

			if (action == 1)
			{
				editState.select(assetHandle);
			}
			else if (action == 2)
			{
				std::string newName = std::string(inputBuff.c_str());
				if (newName != assetName && newName != "")
				{
					std::string newAssetRelativeName = (assetDir / newName).generic_string();
					manager.rename_asset<T>(assetRelativeName, newAssetRelativeName);
				}
			}

			if (gui::IsItemHoveredRect() &&
				gui::IsMouseClicked(gui::drag_button) && !editState.drag_data.object)
			{
				editState.drag(assetHandle, assetRelativeName);
			}
			if (gui::BeginPopupContextItem(assetRelativeName.c_str()))
			{
				openPopup = true;
				if (gui::Selectable("Delete"))
				{
					manager.delete_asset<T>(assetRelativeName);
					if (alreadySelected)
						editState.unselect();
				}
	
				gui::EndPopup();
			}
			if (alreadySelected && !gui::IsAnyItemActive() && input.is_key_pressed(sf::Keyboard::Delete))
			{
				manager.delete_asset<T>(assetRelativeName);
				editState.unselect();
			}
			gui::PopID();
			gui::SameLine();


		}
		if (!storage->container.empty())
			gui::NewLine();

		return openPopup;
	};

	void render_assets(ImVec2 area)
	{
		auto es = core::get_subsystem<editor::EditState>();
		auto am = core::get_subsystem<runtime::AssetManager>();
		auto input = core::get_subsystem<runtime::Input>();
		auto meshes = am->get_storage<Mesh>();
		auto textures = am->get_storage<Texture>();
		auto materials = am->get_storage<Material>();
		auto prefabs = am->get_storage<Prefab>();
		auto shaders = am->get_storage<Shader>();
		auto& icons = es->icons;

		float width = gui::GetContentRegionAvailWidth();
		static bool bigIcons = false;


		if (gui::Button("Import..."))
		{
			std::vector<std::string> paths;
			if (open_multiple_files_dialog("obj,png,tga,dds,ktx,pvr,sc", "", paths))
			{
				auto ts = core::get_subsystem<runtime::TaskSystem>();

				auto task = ts->create("Import Assets", [](const std::vector<std::string>& paths)
				{
					auto logger = logging::get("Log");
					for (auto& path : paths)
					{
						fs::path p = string_utils::to_lower(path);
						fs::path ext = p.extension().string();
						fs::path filename = p.filename();
						std::error_code error;
						if (ext == ".obj")
						{
							fs::path dir = fs::resolve_protocol("data://meshes") / filename;
							if (!fs::copy_file(path, dir, fs::copy_options::overwrite_existing, error))
							{
								logger->error().write("Failed to import file {0} with message {1}", p.string(), error.message());
							}
							else
							{
								fs::last_write_time(dir, fs::file_time_type::clock::now(), std::error_code{});
							}
						}
						else if (ext == ".png" || ext == ".tga" || ext == ".dds" || ext == ".ktx" || ext == ".pvr")
						{
							fs::path dir = fs::resolve_protocol("data://textures") / filename;
							if (!fs::copy_file(path, dir, fs::copy_options::overwrite_existing, error))
							{
								logger->error().write("Failed to import file {0} with message: {1}", p.string(), error.message());
							}
							else
							{
								fs::last_write_time(dir, fs::file_time_type::clock::now(), std::error_code{});
							}
						}
						else if (ext == ".sc")
						{
							fs::path dir = fs::resolve_protocol("data://shaders") / filename;
							if (!fs::copy_file(path, dir, fs::copy_options::overwrite_existing, error))
							{
								logger->error().write("Failed to import file {0} with message {1}", p.string(), error.message());
							}
							else
							{
								fs::last_write_time(dir, fs::file_time_type::clock::now(), std::error_code{});
							}
						}
						else
						{
							logger->error().write("Unsupported file format {0}", ext.string());
						}
					}

				}, paths);
				ts->run(task);
			}
		}
		gui::SameLine();
		gui::PushItemWidth(150.0f);
		gui::SliderFloat("Scale Icons", &scale_icons, 0.5f, 1.0f);
		gui::PopItemWidth();
		gui::Separator();

		static std::string selectedCategory;
		static const std::vector<std::string> categories = { "Materials", "Textures", "Prefabs", "Meshes", "Shaders" };

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings;
		gui::Columns(2, nullptr, true);

		static bool setOffset = true;

		if (setOffset)
		{
			setOffset = false;
			gui::SetColumnOffset(1, 300.0f);
		}
		if (gui::BeginChild("###assets_browser", gui::GetContentRegionAvail(), false, flags))
		{
			for (auto& category : categories)
			{
				if (gui::Selectable(category.c_str(), selectedCategory == category))
				{
					selectedCategory = category;
				}
			}
			gui::EndChild();
		}

		gui::NextColumn();

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
						ecs::utils::save_entity("data://prefabs", entity);
						es->drop();
					}
				}
			}
			

			if(selectedCategory == "Meshes")
				list_items(meshes, *am, *input, *es);

			if(selectedCategory == "Textures")
				list_items(textures, *am, *input, *es);

			if (selectedCategory == "Prefabs")
				list_items(prefabs, *am, *input, *es);


			if (selectedCategory == "Materials")
			{
				if (!list_items(materials, *am, *input, *es))
				{
					if (gui::BeginPopupContextWindow())
					{
						if (gui::Selectable("Create Material"))
						{
							AssetHandle<Material> asset;
							asset.link->id = string_utils::format("data://materials/new_material_{%s}", fs::path(std::tmpnam(nullptr)).filename().string().c_str());
							asset.link->asset = std::make_shared<StandardMaterial>();
							am->save<Material>(asset);
						}
						gui::EndPopup();
					}
				}
				
			}

			if (selectedCategory == "Shaders")
				list_items(shaders, *am, *input, *es);

			gui::EndChild();
		}

		gui::Columns(1);

		
	}

};