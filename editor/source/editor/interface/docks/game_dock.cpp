#include "docks.h"
#include "../../edit_state.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/rendering/render_pass.h"
#include "core/ecs.h"

namespace Docks
{
	
	void renderGame(ImVec2 area)
	{
		auto es = core::get_subsystem<EditState>();
		auto& editorCamera = es->camera;

		auto ecs = core::get_subsystem<core::EntityComponentSystem>();
		ecs->each<CameraComponent>([&editorCamera](
			core::Entity e,
			CameraComponent& cameraComponent
			)
		{
			if (e == editorCamera)
				return;

			const auto surface = cameraComponent.getOutputBuffer();
			auto size = gui::GetContentRegionAvail();

			cameraComponent.setViewportSize({ static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y) });
			gui::Image(surface, size);
	
		});
	}

};