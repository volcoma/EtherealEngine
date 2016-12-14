#pragma once
#include "Interface/Docks/ImGuiDock.h"
#include "Interface/Gizmos/ImGuizmo.h"
#include "Runtime/Ecs/World.h"
#include "Runtime/Assets/AssetHandle.h"
#include "EditorOptions.h"

class AssetManager;
struct Texture;
struct EditState
{
	struct DragData
	{
		rttr::variant object;
		std::string description;
	};
	struct SelectionData
	{
		rttr::variant object;
	};

	void clear();
	void loadIcons(AssetManager& manager);
	void select(rttr::variant object);
	void unselect();
	void drag(rttr::variant object, const std::string& description = "");
	void drop();
	void frameEnd();
	void loadOptions();
	void saveOptions();
	ecs::Entity camera;
	std::string project;
	std::string scene;
	EditorOptions options;
	bool showGrid = true;
	bool wireframeSelection = true;
	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mode = ImGuizmo::LOCAL;

	DragData dragData;
	SelectionData selectionData;

	std::unordered_map<std::string, AssetHandle<Texture>> icons;
};