#include "docks.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/texture.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/system/filesystem.h"
#include "runtime/ecs/prefab.h"
#include "runtime/ecs/utils.h"
#include "../../edit_state.h"
#include <cstdio>

static float scaleIcons = 1.0f;

template<typename T>
AssetHandle<Texture> getAssetIcon(AssetHandle<T> asset)
{
	auto es = core::get_subsystem<EditState>();
	return es->icons["prefab"];
}

template<>
AssetHandle<Texture> getAssetIcon(AssetHandle<Texture> asset)
{
	return asset;
}
template<>
AssetHandle<Texture> getAssetIcon(AssetHandle<Prefab> asset)
{
	auto es = core::get_subsystem<EditState>();
	return es->icons["prefab"];
}
template<>
AssetHandle<Texture> getAssetIcon(AssetHandle<Mesh> asset)
{
	auto es = core::get_subsystem<EditState>();
	return es->icons["mesh"];
}
template<>
AssetHandle<Texture> getAssetIcon(AssetHandle<Material> asset)
{
	auto es = core::get_subsystem<EditState>();
	return es->icons["material"];
}
namespace Docks
{
	template<typename T>
	bool listItems(std::shared_ptr<TStorage<T>> storage, AssetManager& manager, EditState& editState)
	{
		auto& selected = editState.selectionData.object;
		//copy for safe removal from original
		auto container = storage->container;
		bool openPopup = false;
		if (scaleIcons > 0.2f)
		{
			const float size = 50.0f * scaleIcons;
			for (auto& asset : container)
			{
				auto& assetRelativeName = asset.first;
				auto& assetHandle = asset.second.asset;

				if(!string_utils::beginsWith(assetRelativeName, "data://", true))
					continue;
				
				const auto assetName = fs::path(assetRelativeName).filename().string();
				bool alreadySelected = false;
				if (selected.is_type<std::decay<decltype(assetHandle)>::type>())
				{
					if (selected.get_value<std::decay<decltype(assetHandle)>::type>() == assetHandle)
					{
						alreadySelected = true;
					}
				}

				gui::PushID(assetRelativeName.c_str());

				if (gui::GetContentRegionAvailWidth() < size)
					gui::NewLine();

				gui::BeginGroup();
				{
					if (gui::ImageButtonEx(getAssetIcon(assetHandle).link->asset, { size, size }, assetRelativeName.c_str(), alreadySelected))
					{
						editState.select(assetHandle);
						gui::SetWindowFocus();
					}
					gui::PushStyleColor(ImGuiCol_Button, ImVec4(0.0f, 0.0f, 0.0f, 0.0f));
					gui::ButtonEx(assetName.c_str(), { size, gui::GetTextLineHeight() }, ImGuiButtonFlags_Disabled);
					gui::PopStyleColor();
				}
				gui::EndGroup();
				if (gui::IsItemHovered())
				{
					if (gui::IsMouseClicked(2))
					{
						editState.drag(assetHandle, assetRelativeName);
					}
				}
				if (gui::BeginPopupContextItem(assetName.c_str()))
				{
					openPopup = true;
					if (gui::Selectable("Delete"))
					{
						manager.deleteAsset<T>(assetRelativeName);
						if (alreadySelected)
							editState.unselect();
					}
					gui::EndPopup();
				}
		
				gui::PopID();
				gui::SameLine();


			}
			if (!storage->container.empty())
				gui::NewLine();
		}
		else
		{
			const float size = gui::GetTextLineHeight();
			for (auto& asset : container)
			{
				auto& assetRelativeName = asset.first;
				auto& assetHandle = asset.second.asset;

				if (!string_utils::beginsWith(assetRelativeName, "data://", true))
					continue;

				const auto assetName = fs::path(assetRelativeName).filename().string();
				bool alreadySelected = false;
				if (selected.is_type<std::decay<decltype(assetHandle)>::type>())
				{
					if (selected.get_value<std::decay<decltype(assetHandle)>::type>() == assetHandle)
					{
						alreadySelected = true;
					}
				}


				gui::Image(getAssetIcon(assetHandle).link->asset, { size, size });
				gui::SameLine();
				if (gui::Selectable(assetName.c_str(), alreadySelected))
				{
					editState.select(assetHandle);
				}
				if (gui::IsItemHovered())
				{
					if (gui::IsMouseClicked(2))
					{
						editState.drag(assetHandle, assetRelativeName);
					}
				}
				if (gui::BeginPopupContextItem(assetName.c_str()))
				{
					openPopup = true;
					if (gui::Selectable("Delete"))
					{
						manager.deleteAsset<T>(assetRelativeName);
						editState.unselect();
					}
					gui::EndPopup();
				}
			}
		}
		return openPopup;
	};

	void renderAssets(ImVec2 area)
	{
		auto am = core::get_subsystem<AssetManager>();
		auto es = core::get_subsystem<EditState>();
		auto meshes = am->getStorage<Mesh>();
		auto textures = am->getStorage<Texture>();
		auto materials = am->getStorage<Material>();
		auto prefabs = am->getStorage<Prefab>();
		auto& icons = es->icons;

		float width = gui::GetContentRegionAvailWidth();
		static bool bigIcons = false;


		if (gui::Button("Import..."))
		{
		
		}

		gui::SameLine();
		gui::PushItemWidth(150.0f);
		gui::SliderFloat("Scale Icons", &scaleIcons, 0.0f, 1.0f);
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
			if (gui::IsWindowHovered() && gui::IsMouseReleased(2))
			{
				auto& dragged = es->dragData.object;
				if (dragged && dragged.is_type<core::Entity>())
				{
					auto entity = dragged.get_value<core::Entity>();
					ecs::utils::saveEntity("data://prefabs", entity);
					es->drop();
				}
			}
			

			if(selectedCategory == "Meshes")
				listItems(meshes, *am, *es);

			if(selectedCategory == "Textures")
				listItems(textures, *am, *es);

			if (selectedCategory == "Prefabs")
				listItems(prefabs, *am, *es);

			if (selectedCategory == "Materials")
			{
				if (!listItems(materials, *am, *es))
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