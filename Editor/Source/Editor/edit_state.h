#pragma once
#include "core/subsystem.h"
#include "interface/docks/imguidock.h"
#include "interface/gizmos/imguizmo.h"
#include "core/ecs.h"
#include "runtime/assets/asset_handle.h"


struct Texture;

struct EditState : core::Subsystem
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

	bool initialize();
	void dispose();
	void select(rttr::variant object);
	void unselect();
	void drag(rttr::variant object, const std::string& description = "");
	void drop();
	void frameEnd();
	core::Entity camera;
	std::string scene;
	
	bool showGrid = true;
	bool wireframeSelection = true;
	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mode = ImGuizmo::LOCAL;

	DragData dragData;
	SelectionData selectionData;

	std::unordered_map<std::string, AssetHandle<Texture>> icons;
};