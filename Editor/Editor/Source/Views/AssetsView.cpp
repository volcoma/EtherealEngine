#include "AssetsView.h"
#include "../EditorApp.h"

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


AssetsView::AssetsView()
{
	mName = "Assets";
}

AssetsView::~AssetsView()
{
}

void AssetsView::render(AppWindow& window)
{

	ImGuiIO& io = gui::GetIO();
	ImVec2 sz = ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2);
	gui::SetNextWindowPos(ImVec2(sz.x - sz.x / 2, sz.y - sz.y / 2));
	gui::SetNextWindowSize(sz, ImGuiSetCond_FirstUseEver);
	if (!gui::BeginDock(mName.c_str(), &mOpen))
	{
		gui::EndDock();
		return;
	}

	auto& app = Singleton<EditorApp>::getInstance();
	auto& editState = app.getEditState();
	auto& manager = app.getAssetManager();
	auto& meshes = manager.getStorage<Mesh>();
	auto& textures = manager.getStorage<Texture>();
	auto& shaders = manager.getStorage<Shader>();
	auto& programs = manager.getStorage<Program>();
	auto& materials = manager.getStorage<Material>();

	static bool bigIcons = false;
	gui::Checkbox("Big Icons", &bigIcons);

	auto& listItems = [](const auto& storage, auto& selection)
	{
		if (bigIcons)
		{
			const float size = 64;
			for (auto& asset : storage.assets)
			{
				auto& assetRelativeName = asset.first;
				auto& assetHandle = asset.second;
				auto assetName = FileSystem::getFileName(assetRelativeName);
				bool alreadySelected = selection.selected == assetHandle;
				if(gui::ImageButtonEx(getAssetIcon(assetHandle).get(), { size, size }, assetName.c_str(), assetRelativeName.c_str(), alreadySelected))
				{
					selection.selected = assetHandle;
				}
				gui::SameLine();
// 				if (gui::Selectable(assetRelativeName.c_str(), alreadySelected))
// 				{
// 					selection.selected = assetHandle;
// 				}

			}
			if (!storage.assets.empty())
				gui::NewLine();
		}
		else
		{
			const float size = gui::GetTextLineHeight();
			for (auto& asset : storage.assets)
			{
				auto& assetRelativeName = asset.first;
				auto& assetHandle = asset.second;

				bool alreadySelected = selection.selected == assetHandle;
				gui::Image(getAssetIcon(assetHandle).get(), { size, size });
				gui::SameLine();
				if (gui::Selectable(assetRelativeName.c_str(), alreadySelected))
				{
					selection.selected = assetHandle;
				}

			}
		}
		
	};


	if (gui::TreeNode("Meshes"))
	{
		listItems(meshes, editState.meshSelection);

		gui::TreePop();
	}
	if (gui::TreeNode("Textures"))
	{
		listItems(textures, editState.textureSelection);

		gui::TreePop();
	}
	if (gui::TreeNode("Materials"))
	{
		listItems(materials, editState.materialSelection);

		gui::TreePop();
	}

	gui::EndDock();

}

void AssetsView::renderFunc(ImVec2 area)
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& editState = app.getEditState();
	auto& manager = app.getAssetManager();
	auto& meshes = manager.getStorage<Mesh>();
	auto& textures = manager.getStorage<Texture>();
	auto& shaders = manager.getStorage<Shader>();
	auto& programs = manager.getStorage<Program>();
	auto& materials = manager.getStorage<Material>();

	static bool bigIcons = false;
	gui::Checkbox("Big Icons", &bigIcons);

	auto& listItems = [](const auto& storage, auto& selection)
	{
		if (bigIcons)
		{
			const float size = 64;
			for (auto& asset : storage.assets)
			{
				auto& assetRelativeName = asset.first;
				auto& assetHandle = asset.second;
				auto assetName = FileSystem::getFileName(assetRelativeName);
				bool alreadySelected = selection.selected == assetHandle;
				if (gui::ImageButtonEx(getAssetIcon(assetHandle).get(), { size, size }, assetName.c_str(), assetRelativeName.c_str(), alreadySelected))
				{
					selection.selected = assetHandle;
				}
				gui::SameLine();
				// 				if (gui::Selectable(assetRelativeName.c_str(), alreadySelected))
				// 				{
				// 					selection.selected = assetHandle;
				// 				}

			}
			if (!storage.assets.empty())
				gui::NewLine();
		}
		else
		{
			const float size = gui::GetTextLineHeight();
			for (auto& asset : storage.assets)
			{
				auto& assetRelativeName = asset.first;
				auto& assetHandle = asset.second;

				bool alreadySelected = selection.selected == assetHandle;
				gui::Image(getAssetIcon(assetHandle).get(), { size, size });
				gui::SameLine();
				if (gui::Selectable(assetRelativeName.c_str(), alreadySelected))
				{
					selection.selected = assetHandle;
				}

			}
		}

	};


	if (gui::TreeNode("Meshes"))
	{
		listItems(meshes, editState.meshSelection);

		gui::TreePop();
	}
	if (gui::TreeNode("Textures"))
	{
		listItems(textures, editState.textureSelection);

		gui::TreePop();
	}
	if (gui::TreeNode("Materials"))
	{
		listItems(materials, editState.materialSelection);

		gui::TreePop();
	}
}
