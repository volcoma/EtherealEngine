#include "edit_state.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/material.h"
#include "runtime/system/sfml/Window.hpp"
#include "runtime/ecs/prefab.h"
#include "runtime/ecs/utils.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/components/camera_component.h"

namespace editor
{

	bool EditState::initialize()
	{
		auto am = core::get_subsystem<runtime::AssetManager>();

		am->load<Texture>("editor_data:/icons/translate", false)
			.then([this](auto asset) mutable
		{
			icons["translate"] = asset;
		});

		am->load<Texture>("editor_data:/icons/rotate", false)
			.then([this](auto asset) mutable
		{
			icons["rotate"] = asset;
		});

		am->load<Texture>("editor_data:/icons/scale", false)
			.then([this](auto asset) mutable
		{
			icons["scale"] = asset;
		});

		am->load<Texture>("editor_data:/icons/local", false)
			.then([this](auto asset) mutable
		{
			icons["local"] = asset;
		});

		am->load<Texture>("editor_data:/icons/global", false)
			.then([this](auto asset) mutable
		{
			icons["global"] = asset;
		});

		am->load<Texture>("editor_data:/icons/play", false)
			.then([this](auto asset) mutable
		{
			icons["play"] = asset;
		});

		am->load<Texture>("editor_data:/icons/pause", false)
			.then([this](auto asset) mutable
		{
			icons["pause"] = asset;
		});

		am->load<Texture>("editor_data:/icons/stop", false)
			.then([this](auto asset) mutable
		{
			icons["stop"] = asset;
		});

		am->load<Texture>("editor_data:/icons/next", false)
			.then([this](auto asset) mutable
		{
			icons["next"] = asset;
		});

		am->load<Texture>("editor_data:/icons/material", false)
			.then([this](auto asset) mutable
		{
			icons["material"] = asset;
		});

		am->load<Texture>("editor_data:/icons/mesh", false)
			.then([this](auto asset) mutable
		{
			icons["mesh"] = asset;
		});

		am->load<Texture>("editor_data:/icons/export", false)
			.then([this](auto asset) mutable
		{
			icons["import"] = asset;
		});

		am->load<Texture>("editor_data:/icons/grid", false)
			.then([this](auto asset) mutable
		{
			icons["grid"] = asset;
		});

		am->load<Texture>("editor_data:/icons/wireframe", false)
			.then([this](auto asset) mutable
		{
			icons["wireframe"] = asset;
		});

		am->load<Texture>("editor_data:/icons/prefab", false)
			.then([this](auto asset) mutable
		{
			icons["prefab"] = asset;
		});

		am->load<Texture>("editor_data:/icons/scene", false)
			.then([this](auto asset) mutable
		{
			icons["scene"] = asset;
		});

		am->load<Texture>("editor_data:/icons/shader", false)
			.then([this](auto asset) mutable
		{
			icons["shader"] = asset;
		});
		am->load<Texture>("editor_data:/icons/loading", false)
			.then([this](auto asset) mutable
		{
			icons["loading"] = asset;
		});
		am->load<Texture>("editor_data:/icons/folder", false)
			.then([this](auto asset) mutable
		{
			icons["folder"] = asset;
		});
		
		return true;
	}

	void EditState::save_editor_camera()
	{
		auto es = core::get_subsystem<editor::EditState>();
		if (es->camera)
			ecs::utils::save_data(fs::resolve_protocol("app:/settings/editor_camera.cfg"), { es->camera });
	}

	void EditState::load_editor_camera()
	{
		auto es = core::get_subsystem<editor::EditState>();
		runtime::Entity object;
		if (!ecs::utils::try_load_entity(fs::resolve_protocol("app:/settings/editor_camera.cfg"), object))
		{
			auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
			object = ecs->create();
			object.set_name("EDITOR CAMERA");
			object.assign<TransformComponent>().lock()
				->set_local_position({ 0.0f, 2.0f, -5.0f });
			object.assign<CameraComponent>();
		}

		es->camera = object;
	}

	void EditState::dispose()
	{
		drag_data = {};
		selection_data = {};
		icons.clear();
	}

	void EditState::select(rttr::variant object)
	{
		selection_data.object = object;
	}

	void EditState::unselect()
	{
		selection_data = {};
		imguizmo::enable(false);
		imguizmo::enable(true);
	}

	void EditState::drag(rttr::variant object, const std::string& description)
	{
		drag_data.object = object;
		drag_data.description = description;
	}

	void EditState::drop()
	{
		drag_data = {};
	}

}