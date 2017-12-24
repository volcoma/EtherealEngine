#include "editing_system.h"
#include "core/graphics/texture.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/ecs/components/audio_listener_component.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/constructs/utils.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/render_window.h"
#include "runtime/system/events.h"

namespace editor
{

bool editing_system::initialize()
{
	auto& am = core::get_subsystem<runtime::asset_manager>();

	icons["translate"] = am.load<gfx::texture>("editor_data:/icons/translate.png").get();
	icons["rotate"] = am.load<gfx::texture>("editor_data:/icons/rotate.png").get();
	icons["scale"] = am.load<gfx::texture>("editor_data:/icons/scale.png").get();
	icons["local"] = am.load<gfx::texture>("editor_data:/icons/local.png").get();
	icons["global"] = am.load<gfx::texture>("editor_data:/icons/global.png").get();
	icons["play"] = am.load<gfx::texture>("editor_data:/icons/play.png").get();
	icons["pause"] = am.load<gfx::texture>("editor_data:/icons/pause.png").get();
	icons["stop"] = am.load<gfx::texture>("editor_data:/icons/stop.png").get();
	icons["next"] = am.load<gfx::texture>("editor_data:/icons/next.png").get();
	icons["material"] = am.load<gfx::texture>("editor_data:/icons/material.png").get();
	icons["mesh"] = am.load<gfx::texture>("editor_data:/icons/mesh.png").get();
	icons["export"] = am.load<gfx::texture>("editor_data:/icons/export.png").get();
	icons["grid"] = am.load<gfx::texture>("editor_data:/icons/grid.png").get();
	icons["wireframe"] = am.load<gfx::texture>("editor_data:/icons/wireframe.png").get();
	icons["prefab"] = am.load<gfx::texture>("editor_data:/icons/prefab.png").get();
	icons["scene"] = am.load<gfx::texture>("editor_data:/icons/scene.png").get();
	icons["shader"] = am.load<gfx::texture>("editor_data:/icons/shader.png").get();
	icons["loading"] = am.load<gfx::texture>("editor_data:/icons/loading.png").get();
	icons["folder"] = am.load<gfx::texture>("editor_data:/icons/folder.png").get();
	icons["animation"] = am.load<gfx::texture>("editor_data:/icons/animation.png").get();
	icons["sound"] = am.load<gfx::texture>("editor_data:/icons/sound.png").get();

	return true;
}

void editing_system::dispose()
{
	drag_data = {};
	selection_data = {};
	icons.clear();
}

void editing_system::save_editor_camera()
{
	auto& es = core::get_subsystem<editor::editing_system>();
	if(es.camera)
		ecs::utils::save_entity_to_file(fs::resolve_protocol("app:/settings/editor_camera.cfg"), es.camera);
}

void editing_system::load_editor_camera()
{
	auto& es = core::get_subsystem<editor::editing_system>();
	runtime::entity object;
	if(!ecs::utils::try_load_entity_from_file(fs::resolve_protocol("app:/settings/editor_camera.cfg"),
											  object))
	{
		auto& ecs = core::get_subsystem<runtime::entity_component_system>();
		object = ecs.create();
	}
	object.set_name("EDITOR CAMERA");

	if(!object.has_component<transform_component>())
	{
		object.assign<transform_component>().lock()->set_local_position({0.0f, 2.0f, -5.0f});
	}
	if(!object.has_component<camera_component>())
	{
		object.assign<camera_component>();
	}
	if(!object.has_component<audio_listener_component>())
	{
		object.assign<audio_listener_component>();
	}

	es.camera = object;
}

void editing_system::select(rttr::variant object)
{
	selection_data.object = object;
}

void editing_system::unselect()
{
	selection_data = {};
	imguizmo::enable(false);
	imguizmo::enable(true);
}

void editing_system::drag(rttr::variant object, const std::string& description)
{
	drag_data.object = object;
	drag_data.description = description;
}

void editing_system::drop()
{
	drag_data = {};
}
}
