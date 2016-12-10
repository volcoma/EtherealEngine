#include "Docks.h"
#include "../../EditorApp.h"
#include "Runtime/Rendering/Mesh.h"
#include "Runtime/Rendering/Material.h"
#include "Runtime/Rendering/Texture.h"
#include "Runtime/Assets/AssetManager.h"
#include "Runtime/System/MessageBox.h"
#include "Runtime/System/FileSystem.h"
#include "Runtime/Ecs/Prefab.h"
#include "Runtime/Ecs/Utils.h"
#include <cstdio>

static float scaleIcons = 1.0f;

namespace Docks
{
	template<typename T>
	bool listItems(std::shared_ptr<TStorage<T>> storage, AssetManager& manager, EditState& editState)
	{
		//copy for safe removal from original
		auto container = storage->container;
		bool openPopup = false;
		if (scaleIcons > 0.2f)
		{
			const float size = 74.0f * scaleIcons;
			for (auto& asset : container)
			{
				auto& assetRelativeName = asset.first;
				auto& assetHandle = asset.second.asset;
				auto dir = fs::getDirectoryName(assetRelativeName);
				if(!string_utils::beginsWith(dir, "data://", true))
					continue;

				auto assetName = fs::getFileName(assetRelativeName);
				bool alreadySelected = false;
				if (editState.selected.is_type<std::decay<decltype(assetHandle)>::type>())
				{
					if (editState.selected.get_value<std::decay<decltype(assetHandle)>::type>() == assetHandle)
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
						editState.drag(assetHandle);
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
				auto assetName = fs::getFileName(assetRelativeName);


				bool alreadySelected = false;
				if (editState.selected.is_type<std::decay<decltype(assetHandle)>::type>())
				{
					if (editState.selected.get_value<std::decay<decltype(assetHandle)>::type>() == assetHandle)
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
						editState.drag(assetHandle);
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

	AssetHandle<Texture> getAssetIcon(AssetHandle<Texture> asset)
	{
		return asset;
	}
	AssetHandle<Texture> getAssetIcon(AssetHandle<Prefab> asset)
	{
		auto& app = Singleton<EditorApp>::getInstance();
		auto& editState = app.getEditState();
		return editState.icons["prefab"];
	}

	AssetHandle<Texture> getAssetIcon(AssetHandle<Mesh> asset)
	{
		auto& app = Singleton<EditorApp>::getInstance();
		auto& editState = app.getEditState();
		return editState.icons["mesh"];
	}
	AssetHandle<Texture> getAssetIcon(AssetHandle<Material> asset)
	{
		auto& app = Singleton<EditorApp>::getInstance();
		auto& editState = app.getEditState();
		return editState.icons["material"];
	}

	void renderAssets(ImVec2 area)
	{
		auto& app = Singleton<EditorApp>::getInstance();
		auto& editState = app.getEditState();
		auto& manager = app.getAssetManager();
		auto meshes = manager.getStorage<Mesh>();
		auto textures = manager.getStorage<Texture>();
		auto materials = manager.getStorage<Material>();
		auto prefabs = manager.getStorage<Prefab>();
		auto& icons = editState.icons;

		float width = gui::GetContentRegionAvailWidth();
		static bool bigIcons = false;


		if (gui::Button("Import..."))
		{
			misc::messageBox("Not Implemented!", "Note", misc::Style::Info, misc::Buttons::OK);
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
				auto& dragged = editState.dragged;
				if (dragged && dragged.is_type<ecs::Entity>())
				{
					auto entity = dragged.get_value<ecs::Entity>();
					ecs::utils::saveEntity(fs::resolveFileLocation("data://prefabs/" + entity.getName() + ".asset"), entity);
					editState.drop();
				}
			}
			

			if(selectedCategory == "Meshes")
				listItems(meshes, manager, editState);

			if(selectedCategory == "Textures")
				listItems(textures, manager, editState);

			if (selectedCategory == "Prefabs")
				listItems(prefabs, manager, editState);

			if (selectedCategory == "Materials")
			{
				if (!listItems(materials, manager, editState))
				{
					if (gui::BeginPopupContextWindow())
					{
						if (gui::Selectable("Create Material"))
						{
							AssetHandle<Material> asset;
							asset.link->id = string_utils::format("data://materials/new_material_{%s}", fs::getFileName(std::tmpnam(nullptr)).c_str());
							asset.link->asset = std::make_shared<StandardMaterial>();
							manager.save<Material>(asset);
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