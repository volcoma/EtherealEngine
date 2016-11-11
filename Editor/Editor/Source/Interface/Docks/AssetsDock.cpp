#include "Docks.h"
#include "../../EditorApp.h"
#include "Runtime/Rendering/Mesh.h"
#include "Runtime/Rendering/Material.h"
#include "Runtime/Rendering/Texture.h"
#include "Runtime/Assets/AssetManager.h"
#include "Runtime/System/MessageBox.h"
#include "Runtime/System/FileSystem.h"

namespace Docks
{
	AssetHandle<Texture> getAssetIcon(AssetHandle<Texture> asset)
	{
		return asset;
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
		auto& icons = editState.icons;

		float width = gui::GetContentRegionAvailWidth();
		static bool bigIcons = false;
		static float scaleIcons = 1.0f;

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
		static const std::vector<std::string> categories = { "Materials", "Textures", "Meshes" };

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
			auto listItems = [](const auto& storage, auto& editState)
			{

				if (scaleIcons > 0.2f)
				{
					const float size = 74.0f * scaleIcons;
					for (auto& asset : storage->container)
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

						gui::PushID(assetRelativeName.c_str());

						if (gui::GetContentRegionAvailWidth() < size)
							gui::NewLine();

						gui::BeginGroup();
						{
							if (gui::ImageButtonEx(getAssetIcon(assetHandle).get(), { size, size }, assetRelativeName.c_str(), alreadySelected))
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
						gui::PopID();
						gui::SameLine();


					}
					if (!storage->container.empty())
						gui::NewLine();
				}
				else
				{
					const float size = gui::GetTextLineHeight();
					for (auto& asset : storage->container)
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

					}
				}

			};

			if(selectedCategory == "Meshes")
				listItems(meshes, editState);

			if(selectedCategory == "Textures")
				listItems(textures, editState);

			if (selectedCategory == "Materials")
			{
				listItems(materials, editState);

				if (gui::BeginPopupContextWindow())
				{
					if (gui::Selectable("Create Material"))
					{
					
					}

					gui::EndPopup();
				}
			}

			gui::EndChild();
		}

		gui::Columns(1);

		
	}

};