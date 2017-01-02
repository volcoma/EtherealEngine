#include "docks.h"
#include "../../edit_state.h"
#include "runtime/ecs/ecs.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/rendering/render_pass.h"

namespace Docks
{
	
	void render_game(ImVec2 area)
	{
		auto es = core::get_subsystem<editor::EditState>();
		auto& editor_camera = es->camera;
		auto& dragged = es->drag_data.object;

		auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
		ecs->each<CameraComponent>([&editor_camera](
			runtime::Entity e,
			CameraComponent& cameraComponent
			)
		{
			if (e == editor_camera)
				return;

			const auto surface = cameraComponent.get_output_buffer();
			auto size = gui::GetContentRegionAvail();

			cameraComponent.set_viewport_size({ static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y) });
			gui::Image(surface, size);
	
		});
	}

};