#include "game_dock.h"
#include "../../edit_state.h"
#include "runtime/ecs/ecs.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/rendering/camera.h"

void GameDock::render(ImVec2 area)
{
	auto es = core::get_subsystem<editor::EditState>();
	auto& editor_camera = es->camera;
	auto& dragged = es->drag_data.object;

	auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
	ecs->each<CameraComponent>([&editor_camera](
		runtime::Entity e,
		CameraComponent& camera_comp
		)
	{
		if (e == editor_camera)
			return;

		auto size = gui::GetContentRegionAvail();
		if (size.x > 0.0f && size.y > 0.0f)
		{
			camera_comp.set_viewport_size({ static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y) });
			const auto& camera = camera_comp.get_camera();
			auto& render_view = camera_comp.get_render_view();
			const auto& viewport_size = camera.get_viewport_size();
			const auto surface = render_view.get_output_fbo(viewport_size);

			gui::Image(surface, size);
		}

	});
}

