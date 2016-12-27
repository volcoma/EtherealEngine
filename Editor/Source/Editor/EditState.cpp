#include "EditState.h"
#include "Runtime/Assets/AssetManager.h"
#include "Runtime/Rendering/Texture.h"
#include "Runtime/Rendering/Mesh.h"
#include "Runtime/Rendering/Material.h"
#include "Runtime/System/SFML/Window.hpp"
#include "Runtime/Ecs/Prefab.h"

#include "Core/serialization/archives.h"
#include "Meta/EditorOptions.hpp"
void EditState::clear()
{
	dragData = {};
	selectionData = {};
	icons.clear();
}


void EditState::loadIcons(AssetManager& manager)
{
	manager.load<Texture>("editor_data://icons/translate", false)
		.then([this](auto asset) mutable
	{
		icons["translate"] = asset;
	});
	
	manager.load<Texture>("editor_data://icons/rotate", false)
		.then([this](auto asset) mutable
	{
		icons["rotate"] = asset;
	});

	manager.load<Texture>("editor_data://icons/scale", false)
		.then([this](auto asset) mutable
	{
		icons["scale"] = asset;
	});

	manager.load<Texture>("editor_data://icons/local", false)
		.then([this](auto asset) mutable
	{
		icons["local"] = asset;
	});

	manager.load<Texture>("editor_data://icons/global", false)
		.then([this](auto asset) mutable
	{
		icons["global"] = asset;
	});

	manager.load<Texture>("editor_data://icons/play", false)
		.then([this](auto asset) mutable
	{
		icons["play"] = asset;
	});

	manager.load<Texture>("editor_data://icons/pause", false)
		.then([this](auto asset) mutable
	{
		icons["pause"] = asset;
	});

	manager.load<Texture>("editor_data://icons/stop", false)
		.then([this](auto asset) mutable
	{
		icons["stop"] = asset;
	});

	manager.load<Texture>("editor_data://icons/next", false)
		.then([this](auto asset) mutable
	{
		icons["next"] = asset;
	});

	manager.load<Texture>("editor_data://icons/material", false)
		.then([this](auto asset) mutable
	{
		icons["material"] = asset;
	});

	manager.load<Texture>("editor_data://icons/mesh", false)
		.then([this](auto asset) mutable
	{
		icons["mesh"] = asset;
	});

	manager.load<Texture>("editor_data://icons/export", false)
		.then([this](auto asset) mutable
	{
		icons["import"] = asset;
	});

	manager.load<Texture>("editor_data://icons/grid", false)
		.then([this](auto asset) mutable
	{
		icons["grid"] = asset;
	});

	manager.load<Texture>("editor_data://icons/wireframe", false)
		.then([this](auto asset) mutable
	{
		icons["wireframe"] = asset;
	});
	manager.load<Texture>("editor_data://icons/prefab", false)
		.then([this](auto asset) mutable
	{
		icons["prefab"] = asset;
	});
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

void EditState::loadOptions()
{
	const fs::path absoluteKey = fs::resolve_protocol("editor_data://Config/Options.cfg");
	if (!fs::exists(absoluteKey, std::error_code{}))
	{
		saveOptions();
	}
	else
	{
		std::ifstream output(absoluteKey);
		cereal::IArchive_JSON ar(output);

		ar(
			cereal::make_nvp("options", options)
		);

		auto& items = options.recentProjects;
		auto iter = std::begin(items);
		while (iter != items.end())
		{
			auto& item = *iter;

			if (!fs::exists(item, std::error_code{}))
			{
				iter = items.erase(iter);				
			}
			else
			{
				++iter;
			}
		}
	}
}

void EditState::saveOptions()
{
	const std::string absoluteKey = fs::resolve_protocol("editor_data://Config/Options.cfg").string();
	std::ofstream output(absoluteKey);
	cereal::OArchive_JSON ar(output);

	ar(
		cereal::make_nvp("options", options)
	);
}