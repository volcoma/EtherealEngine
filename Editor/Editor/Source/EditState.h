#pragma once
#include "Interface/Docks/ImGuiDock.h"
#include "Interface/Gizmos/ImGuizmo.h"
#include "Runtime/Ecs/World.h"
#include "Runtime/Assets/AssetHandle.h"
class AssetManager;
struct Texture;
struct EditState
{
	void clear();
	void loadIcons(AssetManager& manager);
	void select(rttr::variant object);
	void unselect();
	void drag(rttr::variant object);
	void drop();
	void frameEnd();
	ecs::Entity camera;
	rttr::variant selected;
	rttr::variant dragged;

	std::string project;
	std::string scene;
	ImGuizmo::OPERATION operation = ImGuizmo::TRANSLATE;
	ImGuizmo::MODE mode = ImGuizmo::LOCAL;
	std::unordered_map<std::string, AssetHandle<Texture>> icons;
};