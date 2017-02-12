#include "debugdraw_system.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/components/light_component.h"
#include "runtime/ecs/components/reflection_probe_component.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/rendering/camera.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/model.h"
#include "runtime/rendering/vertex_buffer.h"
#include "runtime/rendering/index_buffer.h"
#include "runtime/rendering/program.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/debugdraw/debugdraw.h"
#include "runtime/assets/asset_manager.h"
#include "../edit_state.h"
#include "runtime/system/engine.h"

namespace editor
{
	void DebugDrawSystem::frame_render(std::chrono::duration<float> dt)
	{
		auto es = core::get_subsystem<EditState>();
		auto& editor_camera = es->camera;
		auto& selected = es->selection_data.object;
		if (!editor_camera ||
			!editor_camera.has_component<CameraComponent>())
			return;

		const auto camera_comp = editor_camera.component<CameraComponent>();
		const auto camera_comp_ptr = camera_comp.lock().get();
		auto& render_view = camera_comp_ptr->get_render_view();
		auto& camera = camera_comp_ptr->get_camera();
		const auto& view = camera.get_view();
		const auto& proj = camera.get_projection();
		const auto& viewport_size = camera.get_viewport_size();
		const auto surface = render_view.get_output_fbo(viewport_size);
		const auto camera_posiiton = camera.get_position();

		RenderPass pass("debug_draw_pass");
		pass.bind(surface.get());
		pass.set_view_proj(view, proj);
		ddRAII dd(pass.id);

		auto draw_grid = [](std::uint32_t grid_color, float height, float height_intervals, std::uint32_t grid_size, std::uint32_t size_intervals, std::uint32_t iteration, std::uint32_t max_iterations)
		{
	
			bool should_render = true;
			if (iteration + 1 != max_iterations)
			{
				const auto iterationHeight = height_intervals * float(iteration + 1);
				const float factor = math::clamp(height, 0.0f, iterationHeight) / iterationHeight;
				std::uint32_t r = (grid_color) & 0xff;
				std::uint32_t g = (grid_color >> 8) & 0xff;
				std::uint32_t b = (grid_color >> 16) & 0xff;
				std::uint32_t a = (grid_color >> 24) & 0xff;
				a = static_cast<std::uint32_t>(math::lerp(255.0f, 0.0f, factor));
				if (a < 10)
					should_render = false;

				grid_color = r + (g << 8) + (b << 16) + (a << 24);
			}


			if (should_render)
			{
				auto step = (size_intervals * iteration);
				step = step ? step : 1;
				ddPush();
				ddSetState(true, false, true);
				ddSetColor(grid_color);
				math::vec3 center = { 0.0f, 0.0f, 0.0f };
				math::vec3 normal = { 0.0f, 1.0f, 0.0f };
				ddDrawGrid(&normal, &center, grid_size / step, float(step));
				ddPop();
			}

		};

		if (es->show_grid)
		{
			static const auto far_clip = 200;
			static const auto height = 40.0f;
			static const auto divison = 10;
			const auto iterations = math::power_of_n_round_down(far_clip, divison);
			for (std::uint32_t i = 0; i < iterations; ++i)
			{
				draw_grid(0xff606060, math::abs(camera_posiiton.y), height, far_clip, divison, i, iterations);
			}
		}


		if (!selected || !selected.is_type<runtime::Entity>())
			return;

		auto selected_entity = selected.get_value<runtime::Entity>();

		if (!selected_entity ||
			!selected_entity.has_component<TransformComponent>())
			return;


		const auto transform_comp = selected_entity.component<TransformComponent>().lock();
		const auto transform_comp_ptr = transform_comp.get();
		const auto& world_transform = transform_comp_ptr->get_transform();

		if (selected_entity.has_component<CameraComponent>() && selected_entity != editor_camera)
		{
			const auto selected_camera_comp = selected_entity.component<CameraComponent>();
			const auto selected_camera_comp_ptr = selected_camera_comp.lock().get();
			auto& selected_camera = selected_camera_comp_ptr->get_camera();
			const auto view_proj = selected_camera.get_view_projection();
			const auto bounds = selected_camera.get_local_bounding_box();
			ddPush();
			ddSetColor(0xffffffff);
			
			if (selected_camera.get_projection_mode() == ProjectionMode::Perspective)
			{
				ddSetTransform(nullptr);
				ddDrawFrustum(&view_proj);
			}
			else
			{
				Aabb aabb;
				aabb.m_min[0] = bounds.min.x;
				aabb.m_min[1] = bounds.min.y;
				aabb.m_min[2] = bounds.min.z;
				aabb.m_max[0] = bounds.max.x;
				aabb.m_max[1] = bounds.max.y;
				aabb.m_max[2] = bounds.max.z;
				ddSetTransform(&world_transform);
				ddDraw(aabb);
			}
			
			ddPop();
		}

		if (selected_entity.has_component<LightComponent>())
		{
			const auto light_comp = selected_entity.component<LightComponent>();
			const auto light_comp_ptr = light_comp.lock().get();
			const auto& light = light_comp_ptr->get_light();
			if (light.light_type == LightType::Spot)
			{
				auto adjacent = light.spot_data.get_range();
				{
					auto tan_angle = math::tan(math::radians(light.spot_data.get_outer_angle() * 0.5f));
					// oposite = tan * adjacent
					auto oposite = tan_angle * adjacent;
					ddPush();
					ddSetColor(0xff00ff00);
					ddSetWireframe(true);
					ddSetLod(3);
					math::vec3 from = transform_comp_ptr->get_position();
					math::vec3 to = from + transform_comp_ptr->get_z_axis() * adjacent;
					ddDrawCone(&to, &from, oposite);
					ddPop();
				}
				{
					auto tan_angle = math::tan(math::radians(light.spot_data.get_inner_angle() * 0.5f));
					// oposite = tan * adjacent
					auto oposite = tan_angle * adjacent;
					ddPush();
					ddSetColor(0xff00ffff);
					ddSetWireframe(true);
					ddSetLod(3);
					math::vec3 from = transform_comp_ptr->get_position();
					math::vec3 to = from + transform_comp_ptr->get_z_axis() * adjacent;
					ddDrawCone(&to, &from, oposite);
					ddPop();
				}
			}
			else if (light.light_type == LightType::Point)
			{
				auto radius = light.point_data.range;
				ddPush();
				ddSetColor(0xff00ff00);
				ddSetWireframe(true);
				math::vec3 center = transform_comp_ptr->get_position();
				ddDrawCircle(Axis::X, center.x, center.y, center.z, radius);
				ddDrawCircle(Axis::Y, center.x, center.y, center.z, radius);
				ddDrawCircle(Axis::Z, center.x, center.y, center.z, radius);
				ddPop();
			}
			else if (light.light_type == LightType::Directional)
			{
				ddPush();
				ddSetLod(UINT8_MAX);
				ddSetColor(0xff00ff00);
				ddSetWireframe(true);
				math::vec3 from1 = transform_comp_ptr->get_position();
				math::vec3 to1 = from1 + transform_comp_ptr->get_z_axis() * 2.0f;
				ddDrawCylinder(&from1, &to1, 0.1f);
				math::vec3 from2 = to1;
				math::vec3 to2 = from2 + transform_comp_ptr->get_z_axis() * 1.5f;;
				ddDrawCone(&from2, &to2, 0.5f);
				ddPop();
			}
		}

		if (selected_entity.has_component<ReflectionProbeComponent>())
		{
			const auto probe_comp = selected_entity.component<ReflectionProbeComponent>();
			const auto probe_comp_ptr = probe_comp.lock().get();
			const auto& probe = probe_comp_ptr->get_probe();
			if (probe.probe_type == ProbeType::Box)
			{
				ddPush();
				ddSetColor(0xff00ff00);
				ddSetWireframe(true);
				ddSetTransform(&world_transform);
				Aabb aabb;
				aabb.m_min[0] = -probe.extents.x;
				aabb.m_min[1] = -probe.extents.y;
				aabb.m_min[2] = -probe.extents.z;
				aabb.m_max[0] = probe.extents.x;
				aabb.m_max[1] = probe.extents.y;
				aabb.m_max[2] = probe.extents.z;
				ddDraw(aabb);
				ddPop();
			}
			else
			{
				auto radius = probe.extents.x;
				ddPush();
				ddSetColor(0xff00ff00);
				ddSetWireframe(true);
				math::vec3 center = transform_comp_ptr->get_position();
				ddDrawCircle(Axis::X, center.x, center.y, center.z, radius);
				ddDrawCircle(Axis::Y, center.x, center.y, center.z, radius);
				ddDrawCircle(Axis::Z, center.x, center.y, center.z, radius);
				ddPop();
			}
		}

		if (selected_entity.has_component<ModelComponent>())
		{
			const auto model_comp = selected_entity.component<ModelComponent>();
			const auto model_comp_ptr = model_comp.lock().get();
			const auto& model = model_comp_ptr->get_model();
			if (!model.is_valid())
				return;

			const auto mesh = model.get_lod(0);
			if (!mesh)
				return;
			const auto& frustum = camera.get_frustum();
			const auto& bounds = mesh->get_bounds();
			// Test the bounding box of the mesh
			if (math::frustum::test_obb(frustum, bounds, world_transform))
			{
				if(false)//if (es->wireframe_selection)
				{
					const float u_params[8] =
					{
						1.0f, 1.0f, 0.0f, 0.7f, //r,g,b,a
						1.0f, 0.0f, 0.0f, 0.0f  //thickness, unused, unused, unused
					};
					if (!_program)
						return;

					model.render(
						pass.id,
						world_transform,
						false,
						false,
						false,
						BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA),
						0,
						_program.get(), [&u_params](Program& program)
					{
						program.set_uniform("u_params", &u_params, 2);
					});
				}
				else
				{
					ddPush();
					ddSetColor(0xff00ff00);
					ddSetWireframe(true);
					ddSetTransform(&world_transform);
					Aabb aabb;
					aabb.m_min[0] = bounds.min.x;
					aabb.m_min[1] = bounds.min.y;
					aabb.m_min[2] = bounds.min.z;
					aabb.m_max[0] = bounds.max.x;
					aabb.m_max[1] = bounds.max.y;
					aabb.m_max[2] = bounds.max.z;
					ddDraw(aabb);
					ddPop();
				}
			}
		}
	}


	bool DebugDrawSystem::initialize()
	{
		runtime::on_frame_render.connect(this, &DebugDrawSystem::frame_render);

		auto am = core::get_subsystem<runtime::AssetManager>();
		am->load<Shader>("editor_data:/shaders/vs_wf_wireframe", false)
			.then([this, am](auto vs)
		{
			am->load<Shader>("editor_data:/shaders/fs_wf_wireframe", false)
				.then([this, vs](auto fs)
			{
				_program = std::make_unique<Program>(vs, fs);
			});
		});

		ddInit();
		return true;
	}

	void DebugDrawSystem::dispose()
	{
		ddShutdown();
		runtime::on_frame_render.disconnect(this, &DebugDrawSystem::frame_render);
	}

}