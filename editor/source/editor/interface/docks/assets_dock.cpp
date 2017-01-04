#include "docks.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/texture.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/system/filesystem.h"
#include "runtime/ecs/prefab.h"
#include "runtime/ecs/utils.h"
#include "runtime/input/input.h"
#include "../../edit_state.h"
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
			auto& assetHandle = asset.second.asset;
			auto path = fs::path(assetRelativeName);

			if (path.root_path().generic_string() != "data:/")
				continue;

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

			gui::PushID(assetRelativeName.c_str());

			if (gui::GetContentRegionAvailWidth() < size)
				gui::NewLine();

			std::string inputBuff = assetName;
			inputBuff.resize(64, 0);

			int action = gui::ImageButtonWithLabel(
				get_asset_icon(assetHandle).link->asset,
				{ size, size },
				alreadySelected,
				assetName.c_str(),
				&inputBuff[0],
				inputBuff.size(),
				ImGuiInputTextFlags_EnterReturnsTrue);

			if (action == 1)
			{
				editState.select(assetHandle);
			}
			else if (action == 2)
			{
				std::string newName = std::string(inputBuff.c_str());
				if (newName != assetName)
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
			if (gui::BeginPopupContextItem(assetName.c_str()))
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
			if (alreadySelected && input.is_key_pressed(sf::Keyboard::Delete))
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
		auto& icons = es->icons;

		float width = gui::GetContentRegionAvailWidth();
		static bool bigIcons = false;


		if (gui::Button("Import..."))
		{
		
		}
		gui::SameLine();
		gui::PushItemWidth(150.0f);
		gui::SliderFloat("Scale Icons", &scale_icons, 0.5f, 1.0f);
		gui::PopItemWidth();
		gui::Separator();

		static std::string selectedCategory;
		static const std::vector<std::string> categories = { "Materials", "Textures", "Prefabs", "Meshes" };

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
			if (gui::IsWindowHovered() && gui::IsMouseReleased(gui::drag_button))
			{
				auto& dragged = es->drag_data.object;
				if (dragged && dragged.is_type<runtime::Entity>())
				{
					auto entity = dragged.get_value<runtime::Entity>();
					ecs::utils::save_entity("data://prefabs", entity);
					es->drop();
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

			gui::EndChild();
		}

		gui::Columns(1);

		
	}

};