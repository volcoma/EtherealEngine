#include "scene_dock.h"
#include "../../editing/editing_system.h"
#include "../../system/project_manager.h"
#include "core/system/simulation.h"
#include "runtime/assets/asset_handle.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/prefab.h"
#include "runtime/ecs/utils.h"
#include "runtime/input/input.h"
#include "runtime/rendering/camera.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/rendering/render_window.h"
#include "runtime/rendering/renderer.h"

static bool show_gbuffer = false;

void show_statistics(const unsigned int fps)
{
	ImVec2 pos = gui::GetCursorScreenPos();
	gui::SetNextWindowPos(pos);
	gui::SetNextWindowCollapsed(true, ImGuiCond_FirstUseEver);
	gui::Begin("STATISTICS", nullptr,
			   ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize);

	gui::AlignFirstTextHeightToWidgets();
	gui::Text("FPS  : %u", fps);
	gui::Separator();
	gui::AlignFirstTextHeightToWidgets();
	gui::Text("MSPF : %.3f ms ", 1000.0 / double(fps));
	gui::Separator();

	auto stats = gfx::getStats();
	uint32_t num_draws = stats->numDraw;
	uint32_t num_computes = stats->numCompute;
	gui::AlignFirstTextHeightToWidgets();
	gui::Text("DRAW CALLS: %u", num_draws);
	gui::AlignFirstTextHeightToWidgets();
	gui::Text("COMPUTE CALLS: %u", num_computes);
	gui::AlignFirstTextHeightToWidgets();
	gui::Text("RENDER PASSES: %u", render_pass::get_pass());
	gui::Separator();
	gui::Checkbox("SHOW G-BUFFER", &show_gbuffer);
	gui::End();
}

void draw_selected_camera(const ImVec2& size)
{
	auto& input = core::get_subsystem<runtime::input>();
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& selected = es.selection_data.object;
	auto& editor_camera = es.camera;

	if(selected.is_type<runtime::entity>())
	{
		auto sel = selected.get_value<runtime::entity>();

		if(sel && (editor_camera != sel) && sel.has_component<camera_component>())
		{
			const auto selected_camera = sel.get_component<camera_component>().lock();
			const auto& camera = selected_camera->get_camera();
			auto& render_view = selected_camera->get_render_view();
			const auto& viewport_size = camera.get_viewport_size();
			const auto surface = render_view.get_output_fbo(viewport_size);

			float factor =
				std::min(size.x / float(viewport_size.width), size.y / float(viewport_size.height)) / 4.0f;
			ImVec2 bounds(viewport_size.width * factor, viewport_size.height * factor);
			auto p = gui::GetWindowPos();
			p.x += size.x - bounds.x - 20.0f;
			p.y += size.y - bounds.y - 40.0f;
			gui::SetNextWindowPos(p);
			if(gui::Begin("Camera Preview", nullptr,
						  ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_ShowBorders |
							  ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove |
							  ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
			{
				gui::Image(surface->get_attachment(0).texture, bounds);
			}
			gui::End();

			if(input.is_key_pressed(mml::keyboard::F) && sel.has_component<transform_component>())
			{
				auto transform = editor_camera.get_component<transform_component>().lock();
				auto transform_selected = sel.get_component<transform_component>().lock();
				transform_selected->set_transform(transform->get_transform());
			}
		}
	}
}

void manipulation_gizmos()
{
	auto& input = core::get_subsystem<runtime::input>();
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& selected = es.selection_data.object;
	auto& editor_camera = es.camera;
	auto& operation = es.operation;
	auto& mode = es.mode;

	if(!input.is_mouse_button_down(mml::mouse::right) && !gui::IsAnyItemActive() && !imguizmo::is_using())
	{
		if(input.is_key_pressed(mml::keyboard::W))
		{
			operation = imguizmo::operation::translate;
		}
		if(input.is_key_pressed(mml::keyboard::E))
		{
			operation = imguizmo::operation::rotate;
		}
		if(input.is_key_pressed(mml::keyboard::R))
		{
			operation = imguizmo::operation::scale;
			mode = imguizmo::mode::local;
		}
		if(input.is_key_pressed(mml::keyboard::T))
		{
			mode = imguizmo::mode::local;
		}
		if(input.is_key_pressed(mml::keyboard::Y) && operation != imguizmo::operation::scale)
		{
			mode = imguizmo::mode::world;
		}
	}

	if(selected && selected.is_type<runtime::entity>())
	{
		auto sel = selected.get_value<runtime::entity>();
		if(sel && sel != editor_camera && sel.has_component<transform_component>())
		{
			auto p = gui::GetItemRectMin();
			auto s = gui::GetItemRectSize();
			imguizmo::set_view_rect(p.x, p.y, s.x, s.y);
			auto camera_comp = editor_camera.get_component<camera_component>().lock();
			auto transform_comp = sel.get_component<transform_component>().lock();
			transform_comp->resolve(true);
			auto transform = transform_comp->get_transform();
			math::transform delta;
			math::transform inputTransform = transform;
			float* snap = nullptr;
			if(input.is_key_down(mml::keyboard::LControl))
			{
				if(operation == imguizmo::operation::translate)
					snap = &es.snap_data.translation_snap[0];
				else if(operation == imguizmo::operation::rotate)
					snap = &es.snap_data.rotation_degree_snap;
				else if(operation == imguizmo::operation::scale)
					snap = &es.snap_data.scale_snap;
			}
			const auto& camera = camera_comp->get_camera();
			imguizmo::manipulate(camera.get_view(), camera.get_projection(), operation, mode, transform,
								 nullptr, snap);

			transform_comp->set_transform(transform);

//			if(sel.has_component<model_component>())
//			{
//				const auto model_comp = sel.get_component<model_component>();
//				const auto model_comp_ptr = model_comp.lock().get();
//				const auto& model = model_comp_ptr->get_model();
//				if(!model.is_valid())
//					return;

//				const auto mesh = model.get_lod(0);
//				if(!mesh)
//					return;

//				irect rect = mesh->calculate_screen_rect(transform, camera);
           
//				gui::GetCurrentWindow()->DrawList->AddRect(ImVec2(rect.left, rect.top),
//														   ImVec2(rect.right, rect.bottom),
//														   gui::GetColorU32(ImVec4(1.0f, 0.0f, 0.0f, 1.0f)));
//			}
		}
	}
}

void handle_camera_movement()
{
	if(!gui::IsWindowFocused())
		return;

	auto& es = core::get_subsystem<editor::editing_system>();
	auto& input = core::get_subsystem<runtime::input>();
	auto& sim = core::get_subsystem<core::simulation>();

	auto& editor_camera = es.camera;
	auto dt = sim.get_delta_time().count();

	auto transform = editor_camera.get_component<transform_component>().lock();
	float movement_speed = 5.0f;
	float rotation_speed = 0.2f;
	float multiplier = 5.0f;
	auto delta_move = input.get_cursor_delta_move();

	if(input.is_mouse_button_down(mml::mouse::middle))
	{
		if(input.is_key_down(mml::keyboard::LShift))
		{
			movement_speed *= multiplier;
		}

		if(delta_move.x != 0)
		{
			transform->move_local({-1 * delta_move.x * movement_speed * dt, 0.0f, 0.0f});
		}
		if(delta_move.y != 0)
		{
			transform->move_local({0.0f, delta_move.y * movement_speed * dt, 0.0f});
		}
	}

	if(input.is_mouse_button_down(mml::mouse::right))
	{
		if(input.is_key_down(mml::keyboard::LShift))
		{
			movement_speed *= multiplier;
		}

		if(input.is_key_down(mml::keyboard::W))
		{
			transform->move_local({0.0f, 0.0f, movement_speed * dt});
		}

		if(input.is_key_down(mml::keyboard::S))
		{
			transform->move_local({0.0f, 0.0f, -movement_speed * dt});
		}

		if(input.is_key_down(mml::keyboard::A))
		{
			transform->move_local({-movement_speed * dt, 0.0f, 0.0f});
		}

		if(input.is_key_down(mml::keyboard::D))
		{
			transform->move_local({movement_speed * dt, 0.0f, 0.0f});
		}
		if(input.is_key_down(mml::keyboard::Up))
		{
			transform->move_local({0.0f, 0.0f, movement_speed * dt});
		}

		if(input.is_key_down(mml::keyboard::Down))
		{
			transform->move_local({0.0f, 0.0f, -movement_speed * dt});
		}

		if(input.is_key_down(mml::keyboard::Left))
		{
			transform->move_local({-movement_speed * dt, 0.0f, 0.0f});
		}

		if(input.is_key_down(mml::keyboard::Right))
		{
			transform->move_local({movement_speed * dt, 0.0f, 0.0f});
		}

		if(input.is_key_down(mml::keyboard::Space))
		{
			transform->move_local({0.0f, movement_speed * dt, 0.0f});
		}

		if(input.is_key_down(mml::keyboard::LControl))
		{
			transform->move_local({0.0f, -movement_speed * dt, 0.0f});
		}

		float x = static_cast<float>(delta_move.x);
		float y = static_cast<float>(delta_move.y);

		// Make each pixel correspond to a quarter of a degree.
		float dx = x * rotation_speed;
		float dy = y * rotation_speed;

		transform->resolve(true);
		transform->rotate(0.0f, dx, 0.0f);
		transform->rotate_local(dy, 0.0f, 0.0f);

		float delta_wheel = input.get_mouse_wheel_scroll_delta_move();
		transform->move_local({0.0f, 0.0f, 14.0f * movement_speed * delta_wheel * dt});
	}
}

void scene_dock::render(const ImVec2&)
{
	auto& es = core::get_subsystem<editor::editing_system>();
	auto& renderer = core::get_subsystem<runtime::renderer>();
	auto& ecs = core::get_subsystem<runtime::entity_component_system>();
	auto& input = core::get_subsystem<runtime::input>();
	auto& sim = core::get_subsystem<core::simulation>();

	auto window = renderer.get_focused_window();
	auto& editor_camera = es.camera;
	auto& selected = es.selection_data.object;
	auto& dragged = es.drag_data.object;

	bool has_edit_camera = editor_camera && editor_camera.has_component<camera_component>() &&
						   editor_camera.has_component<transform_component>();

	show_statistics(sim.get_fps());

	if(!has_edit_camera)
		return;

	auto size = gui::GetContentRegionAvail();
	auto pos = gui::GetCursorScreenPos();
	draw_selected_camera(size);

	auto camera_comp = editor_camera.get_component<camera_component>().lock();
	if(size.x > 0 && size.y > 0)
	{
		camera_comp->get_camera().set_viewport_pos(
			{static_cast<std::uint32_t>(pos.x), static_cast<std::uint32_t>(pos.y)});
		camera_comp->set_viewport_size(
			{static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y)});

		const auto& camera = camera_comp->get_camera();
		auto& render_view = camera_comp->get_render_view();
		const auto& viewport_size = camera.get_viewport_size();
		const auto surface = render_view.get_output_fbo(viewport_size);
		gui::Image(surface->get_attachment(0).texture, size);

		if(gui::IsItemClicked(1) || gui::IsItemClicked(2))
		{
			gui::SetWindowFocus();
			if(window)
				window->set_mouse_cursor_visible(false);
		}

		manipulation_gizmos();
		handle_camera_movement();

		if(gui::IsWindowFocused())
		{
			ImGui::PushStyleColor(ImGuiCol_Border, gui::GetStyle().Colors[ImGuiCol_Button]);
			ImGui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
			ImGui::PopStyleColor();

			if(input.is_key_pressed(mml::keyboard::Delete))
			{
				if(selected && selected.is_type<runtime::entity>())
				{
					auto sel = selected.get_value<runtime::entity>();
					if(sel && sel != editor_camera)
					{
						sel.destroy();
						es.unselect();
					}
				}
			}

			if(input.is_key_pressed(mml::keyboard::D))
			{
				if(input.is_key_down(mml::keyboard::LControl))
				{
					if(selected && selected.is_type<runtime::entity>())
					{
						auto sel = selected.get_value<runtime::entity>();
						if(sel && sel != editor_camera)
						{
							auto clone = ecs::utils::clone_entity(sel);
							clone.get_component<transform_component>().lock()->set_parent(
								sel.get_component<transform_component>().lock()->get_parent(), false, true);
							es.select(clone);
						}
					}
				}
			}
		}

		if(gui::IsMouseReleased(1) || gui::IsMouseReleased(2))
		{
			if(window)
				window->set_mouse_cursor_visible(true);
		}

		if(show_gbuffer)
		{
			auto g_buffer_fbo = render_view.get_g_buffer_fbo(viewport_size).get();
			for(std::uint32_t i = 0; i < g_buffer_fbo->get_attachment_count(); ++i)
			{
				const auto attachment = g_buffer_fbo->get_attachment(i).texture;
				gui::Image(attachment, size);

				if(gui::IsItemClicked(1) || gui::IsItemClicked(2))
				{
					gui::SetWindowFocus();
					if(window)
						window->set_mouse_cursor_visible(false);
				}
			}
		}
	}

	if(gui::IsWindowHovered())
	{
		if(dragged)
		{
			math::vec3 projected_pos;

			if(gui::IsMouseReleased(gui::drag_button))
			{
				auto cursor_pos = gui::GetMousePos();
				camera_comp->get_camera().viewport_to_world(
					math::vec2{cursor_pos.x, cursor_pos.y},
					math::plane::fromPointNormal(math::vec3{0.0f, 0.0f, 0.0f}, math::vec3{0.0f, 1.0f, 0.0f}),
					projected_pos, false);
			}

			if(dragged.is_type<runtime::entity>())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				if(gui::IsMouseReleased(gui::drag_button))
				{
					auto dragged_entity = dragged.get_value<runtime::entity>();
					if(dragged_entity)
					{
						dragged_entity.get_component<transform_component>().lock()->set_parent({});
					}

					es.drop();
				}
			}
			if(dragged.is_type<asset_handle<prefab>>())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				if(gui::IsMouseReleased(gui::drag_button))
				{
					auto pfab = dragged.get_value<asset_handle<prefab>>();
					auto object = pfab->instantiate();
					object.get_component<transform_component>().lock()->set_position(projected_pos);
					es.drop();
					es.select(object);
				}
			}
			if(dragged.is_type<asset_handle<mesh>>())
			{
				gui::SetMouseCursor(ImGuiMouseCursor_Move);
				if(gui::IsMouseReleased(gui::drag_button))
				{
					auto hmesh = dragged.get_value<asset_handle<mesh>>();
					model mdl;
					mdl.set_lod(hmesh, 0);

					auto object = ecs.create();
					// Add component and configure it.
					object.assign<transform_component>().lock()->set_position(projected_pos);
					// Add component and configure it.
					object.assign<model_component>()
						.lock()
						->set_casts_shadow(true)
						.set_casts_reflection(false)
						.set_model(mdl);

					es.drop();
					es.select(object);
				}
			}
		}
	}
}

scene_dock::scene_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size)
{

	initialize(dtitle, close_button, min_size, std::bind(&scene_dock::render, this, std::placeholders::_1));
}
