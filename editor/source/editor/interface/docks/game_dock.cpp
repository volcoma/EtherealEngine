#include "game_dock.h"
#include "../../editing/editing_system.h"
#include "runtime/ecs/ecs.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/rendering/camera.h"

void game_dock::render(const ImVec2& area)
{
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& editor_camera = es.camera;
	auto& dragged = es.drag_data.object;

	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	ecs.each<camera_component>([&editor_camera](
		runtime::entity e,
		camera_component& camera_comp
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

			gui::Image(surface->get_attachment(0).texture, size);
		}

	});
}

game_dock::game_dock(const std::string& dtitle, bool dcloseButton, ImVec2 dminSize)
{

	initialize(dtitle, dcloseButton, dminSize, std::bind(&game_dock::render, this, std::placeholders::_1));
}

