#include "EditState.h"
#include "Runtime/Assets/AssetManager.h"
#include "Runtime/Rendering/Texture.h"
#include "Runtime/Rendering/Mesh.h"
#include "Runtime/Rendering/Material.h"
#include "Runtime/System/SFML/Window.hpp"

#include "Core/serialization/archives.h"
#include "Meta/EditorOptions.hpp"
void EditState::clear()
{
	selected = {};
	dragged = {};
	icons.clear();
}


void EditState::loadIcons(AssetManager& manager)
{
	manager.load<Texture>("editor://icons/translate", false)
		.then([this](auto asset) mutable
	{
		icons["translate"] = asset;
	});
	
	manager.load<Texture>("editor://icons/rotate", false)
		.then([this](auto asset) mutable
	{
		icons["rotate"] = asset;
	});

	manager.load<Texture>("editor://icons/scale", false)
		.then([this](auto asset) mutable
	{
		icons["scale"] = asset;
	});

	manager.load<Texture>("editor://icons/local", false)
		.then([this](auto asset) mutable
	{
		icons["local"] = asset;
	});

	manager.load<Texture>("editor://icons/global", false)
		.then([this](auto asset) mutable
	{
		icons["global"] = asset;
	});

	manager.load<Texture>("editor://icons/play", false)
		.then([this](auto asset) mutable
	{
		icons["play"] = asset;
	});

	manager.load<Texture>("editor://icons/pause", false)
		.then([this](auto asset) mutable
	{
		icons["pause"] = asset;
	});

	manager.load<Texture>("editor://icons/stop", false)
		.then([this](auto asset) mutable
	{
		icons["stop"] = asset;
	});

	manager.load<Texture>("editor://icons/next", false)
		.then([this](auto asset) mutable
	{
		icons["next"] = asset;
	});

	manager.load<Texture>("editor://icons/material", false)
		.then([this](auto asset) mutable
	{
		icons["material"] = asset;
	});

	manager.load<Texture>("editor://icons/mesh", false)
		.then([this](auto asset) mutable
	{
		icons["mesh"] = asset;
	});

	manager.load<Texture>("editor://icons/export", false)
		.then([this](auto asset) mutable
	{
		icons["import"] = asset;
	});

	manager.load<Texture>("editor://icons/grid", false)
		.then([this](auto asset) mutable
	{
		icons["grid"] = asset;
	});

	manager.load<Texture>("editor://icons/wireframe", false)
		.then([this](auto asset) mutable
	{
		icons["wireframe"] = asset;
	});
}

void EditState::select(rttr::variant object)
{
	selected = object;
}

void EditState::unselect()
{
	selected = {};
}

void EditState::drag(rttr::variant object)
{
	dragged = object;
	gui::SetWindowFocus();
}

void EditState::drop()
{
	dragged = {};
}

void EditState::frameEnd()
{
	if (dragged)
	{
		std::string tooltip;
		if (dragged.is_type<ecs::Entity>())
		{
			tooltip = dragged.get_value<ecs::Entity>().to_string();
		}
		if (dragged.is_type<AssetHandle<Mesh>>())
		{
			tooltip = dragged.get_value<AssetHandle<Mesh>>().id();
		}
		if (dragged.is_type<AssetHandle<Texture>>())
		{
			tooltip = dragged.get_value<AssetHandle<Texture>>().id();
		}
		if (dragged.is_type<AssetHandle<Material>>())
		{
			tooltip = dragged.get_value<AssetHandle<Material>>().id();
		}
			
		gui::SetTooltip(tooltip.c_str());
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
	const std::string absoluteKey = fs::resolveFileLocation("editor://Config/Options.cfg");
	if (!fs::fileExists(absoluteKey))
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
	}
}

void EditState::saveOptions()
{
	const std::string absoluteKey = fs::resolveFileLocation("editor://Config/Options.cfg");
	std::ofstream output(absoluteKey);
	cereal::OArchive_JSON ar(output);

	ar(
		cereal::make_nvp("options", options)
	);
}