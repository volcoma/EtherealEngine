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
		auto& editorCamera = es->camera;

		auto ecs = core::get_subsystem<runtime::EntityComponentSystem>();
		ecs->each<CameraComponent>([&editorCamera](
			runtime::Entity e,
			CameraComponent& cameraComponent
			)
		{
			if (e == editorCamera)
				return;

			const auto surface = cameraComponent.get_output_buffer();
			auto size = gui::GetContentRegionAvail();

			cameraComponent.set_viewport_size({ static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y) });
			gui::Image(surface, size);
	
		});
	}

};