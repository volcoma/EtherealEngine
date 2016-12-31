#include "edit_state.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/material.h"
#include "runtime/system/SFML/Window.hpp"
#include "runtime/ecs/prefab.h"

void EditState::dispose()
{
	dragData = {};
	selectionData = {};
	icons.clear();
}


bool EditState::initialize()
{
	auto am = core::get_subsystem<AssetManager>();

	am->load<Texture>("editor_data://icons/translate", false)
		.then([this](auto asset) mutable
	{
		icons["translate"] = asset;
	});
	
	am->load<Texture>("editor_data://icons/rotate", false)
		.then([this](auto asset) mutable
	{
		icons["rotate"] = asset;
	});

	am->load<Texture>("editor_data://icons/scale", false)
		.then([this](auto asset) mutable
	{
		icons["scale"] = asset;
	});

	am->load<Texture>("editor_data://icons/local", false)
		.then([this](auto asset) mutable
	{
		icons["local"] = asset;
	});

	am->load<Texture>("editor_data://icons/global", false)
		.then([this](auto asset) mutable
	{
		icons["global"] = asset;
	});

	am->load<Texture>("editor_data://icons/play", false)
		.then([this](auto asset) mutable
	{
		icons["play"] = asset;
	});

	am->load<Texture>("editor_data://icons/pause", false)
		.then([this](auto asset) mutable
	{
		icons["pause"] = asset;
	});

	am->load<Texture>("editor_data://icons/stop", false)
		.then([this](auto asset) mutable
	{
		icons["stop"] = asset;
	});

	am->load<Texture>("editor_data://icons/next", false)
		.then([this](auto asset) mutable
	{
		icons["next"] = asset;
	});

	am->load<Texture>("editor_data://icons/material", false)
		.then([this](auto asset) mutable
	{
		icons["material"] = asset;
	});

	am->load<Texture>("editor_data://icons/mesh", false)
		.then([this](auto asset) mutable
	{
		icons["mesh"] = asset;
	});

	am->load<Texture>("editor_data://icons/export", false)
		.then([this](auto asset) mutable
	{
		icons["import"] = asset;
	});

	am->load<Texture>("editor_data://icons/grid", false)
		.then([this](auto asset) mutable
	{
		icons["grid"] = asset;
	});

	am->load<Texture>("editor_data://icons/wireframe", false)
		.then([this](auto asset) mutable
	{
		icons["wireframe"] = asset;
	});
	am->load<Texture>("editor_data://icons/prefab", false)
		.then([this](auto asset) mutable
	{
		icons["prefab"] = asset;
	});

	return true;
}

void EditState::select(rttr::variant object)
{
	selectionData.object = object;
}

void EditState::unselect()
{
	selectionData = {};
}

void EditState::drag(rttr::variant object, const std::string& description)
{
	dragData.object = object;
	dragData.description = description;
	gui::SetWindowFocus();
}

void EditState::drop()
{
	dragData = {};
}

void EditState::frameEnd()
{
	if (dragData.object)
	{
		
		gui::SetTooltip(dragData.description.c_str());
	}

	if (!gui::IsAnyItemHovered())
	{
		if (gui::IsMouseDoubleClicked(0) && !ImGuizmo::IsOver())
		{
			unselect();
			drop();
		}
	}	
	if (gui::IsMouseReleased(2))
	{
		drop();
	}
}
