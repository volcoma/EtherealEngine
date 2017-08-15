#include "debugdraw_system.h"
#include "../editing/editing_system.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/ecs/components/light_component.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/components/reflection_probe_component.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/rendering/camera.h"
#include "runtime/rendering/debugdraw/debugdraw.h"
#include "runtime/rendering/index_buffer.h"
#include "runtime/rendering/material.h"
#include "runtime/rendering/mesh.h"
#include "runtime/rendering/model.h"
#include "runtime/rendering/program.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/rendering/texture.h"
#include "runtime/rendering/vertex_buffer.h"
#include "runtime/system/engine.h"

namespace editor
{
void debugdraw_system::frame_render(std::chrono::duration<float>)
{
	auto& es = core::get_subsystem<editing_system>();
	auto& editor_camera = es.camera;
	auto& selected = es.selection_data.object;
	if(!editor_camera || !editor_camera.has_component<camera_component>())
		return;

	const auto camera_comp = editor_camera.get_component<camera_component>();
	const auto camera_comp_ptr = camera_comp.lock().get();
	auto& render_view = camera_comp_ptr->get_render_view();
	auto& camera = camera_comp_ptr->get_camera();
	const auto& view = camera.get_view();
	const auto& proj = camera.get_projection();
	const auto& viewport_size = camera.get_viewport_size();
	const auto surface = render_view.get_output_fbo(viewport_size);
	const auto camera_posiiton = camera.get_position();

	render_pass pass("debug_draw_pass");
	pass.bind(surface.get());
	pass.set_view_proj(view, proj);
	ddRAII dd(pass.id);

	if(es.show_grid)
	{
		auto draw_grid = [](std::uint32_t grid_color, float height, float height_intervals,
							std::uint32_t size_intervals, std::uint32_t iteration,
							std::uint32_t max_iterations) {

			bool should_render = true;
			if(iteration + 1 != max_iterations)
			{
				const auto iterationHeight = height_intervals * float(iteration + 1);
				const float factor = math::clamp(height, 0.0f, iterationHeight) / iterationHeight;
				std::uint32_t r = (grid_color)&0xff;
				std::uint32_t g = (grid_color >> 8) & 0xff;
				std::uint32_t b = (grid_color >> 16) & 0xff;
				std::uint32_t a = (grid_color >> 24) & 0xff;
				a = static_cast<std::uint32_t>(math::lerp(255.0f, 0.0f, factor));
				if(a < 10)
					should_render = false;

				grid_color = r + (g << 8) + (b << 16) + (a << 24);
			}

			if(should_render)
			{
				const auto step = static_cast<std::uint32_t>(
					math::pow<int>(static_cast<int>(size_intervals), static_cast<int>(iteration)));
				const auto grid_size = static_cast<std::uint32_t>(math::pow(size_intervals, max_iterations));
				const auto sz = grid_size / step;

				ddPush();
				ddSetState(true, false, true);
				ddSetColor(grid_color);
				math::vec3 center = {0.0f, 0.0f, 0.0f};
				math::vec3 normal = {0.0f, 1.0f, 0.0f};

				ddDrawGrid(&normal, &center, sz, float(step));
				ddPop();
			}

		};

		static const auto divison = 5;
		static const auto iterations = 3;
		static const auto height = 40.0f;

		for(std::uint32_t i = 0; i < iterations; ++i)
		{
			draw_grid(0xff808080, math::abs(camera_posiiton.y), height, divison, i, iterations);
		}
	}

	if(!selected || !selected.is_type<runtime::entity>())
		return;

	auto selected_entity = selected.get_value<runtime::entity>();

	if(!selected_entity || !selected_entity.has_component<transform_component>())
		return;

	const auto transform_comp = selected_entity.get_component<transform_component>().lock();
	const auto transform_comp_ptr = transform_comp.get();
	const auto& world_transform = transform_comp_ptr->get_transform();

	if(selected_entity.has_component<camera_component>() && selected_entity != editor_camera)
	{
		const auto selected_camera_comp = selected_entity.get_component<camera_component>();
		const auto selected_camera_comp_ptr = selected_camera_comp.lock().get();
		auto& selected_camera = selected_camera_comp_ptr->get_camera();
		const auto view_proj = selected_camera.get_view_projection();
		const auto bounds = selected_camera.get_local_bounding_box();
		ddPush();
		ddSetColor(0xffffffff);

		if(selected_camera.get_projection_mode() == projection_mode::perspective)
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
			ddSetTransform(nullptr);
		}

		ddPop();
	}

	if(selected_entity.has_component<light_component>())
	{
		const auto light_comp = selected_entity.get_component<light_component>();
		const auto light_comp_ptr = light_comp.lock().get();
		const auto& light = light_comp_ptr->get_light();
		if(light.type == light_type::spot)
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
		else if(light.type == light_type::point)
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
		else if(light.type == light_type::directional)
		{
			ddPush();
			ddSetLod(255);
			ddSetColor(0xff00ff00);
			ddSetWireframe(true);
			math::vec3 from1 = transform_comp_ptr->get_position();
			math::vec3 to1 = from1 + transform_comp_ptr->get_z_axis() * 2.0f;
			ddDrawCylinder(&from1, &to1, 0.1f);
			math::vec3 from2 = to1;
			math::vec3 to2 = from2 + transform_comp_ptr->get_z_axis() * 1.5f;
			;
			ddDrawCone(&from2, &to2, 0.5f);
			ddPop();
		}
	}

	if(selected_entity.has_component<reflection_probe_component>())
	{
		const auto probe_comp = selected_entity.get_component<reflection_probe_component>();
		const auto probe_comp_ptr = probe_comp.lock().get();
		const auto& probe = probe_comp_ptr->get_probe();
		if(probe.type == probe_type::box)
		{
			ddPush();
			ddSetColor(0xff00ff00);
			ddSetWireframe(true);
			ddSetTransform(&world_transform);
			Aabb aabb;
			aabb.m_min[0] = -probe.box_data.extents.x;
			aabb.m_min[1] = -probe.box_data.extents.y;
			aabb.m_min[2] = -probe.box_data.extents.z;
			aabb.m_max[0] = probe.box_data.extents.x;
			aabb.m_max[1] = probe.box_data.extents.y;
			aabb.m_max[2] = probe.box_data.extents.z;
			ddDraw(aabb);
			ddSetTransform(nullptr);
			ddPop();
		}
		else
		{
			auto radius = probe.sphere_data.range;
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

	if(selected_entity.has_component<model_component>())
	{
		const auto model_comp = selected_entity.get_component<model_component>();
		const auto model_comp_ptr = model_comp.lock().get();
		const auto& model = model_comp_ptr->get_model();
		if(!model.is_valid())
			return;

		const auto mesh = model.get_lod(0);
		if(!mesh)
			return;
		const auto& frustum = camera.get_frustum();
		const auto& bounds = mesh->get_bounds();
		// Test the bounding box of the mesh
		if(math::frustum::test_obb(frustum, bounds, world_transform))
		{
			// if(es->wireframe_selection)
			//{
			//	const float u_params[8] =
			//	{
			//		1.0f, 1.0f, 0.0f, 0.7f, //r,g,b,a
			//		1.0f, 0.0f, 0.0f, 0.0f  //thickness, unused, unused, unused
			//	};
			//	if (!_program)
			//		return;
			//
			//	model.render(
			//		pass.id,
			//		world_transform,
			//		false,
			//		false,
			//		false,
			//		BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA),
			//		0,
			//		_program.get(), [&u_params](program& p)
			//	{
			//		p.set_uniform("u_params", &u_params, 2);
			//	});
			//}
			// else
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
				ddSetTransform(nullptr);
				ddPop();
			}
		}
	}
}

bool debugdraw_system::initialize()
{
	runtime::on_frame_render.connect(this, &debugdraw_system::frame_render);

	auto& ts = core::get_subsystem<core::task_system>();
	auto& am = core::get_subsystem<runtime::asset_manager>();

	auto vs_wf_wireframe = am.load<shader>("editor_data:/shaders/vs_wf_wireframe.sc");
	auto fs_wf_wireframe = am.load<shader>("editor_data:/shaders/fs_wf_wireframe.sc");

	ts.push_on_main(
		[this](asset_handle<shader> vs, asset_handle<shader> fs) {
			_program = std::make_unique<program>(vs, fs);

		},
		vs_wf_wireframe, fs_wf_wireframe);

	ddInit();
	return true;
}

void debugdraw_system::dispose()
{
	ddShutdown();
	runtime::on_frame_render.disconnect(this, &debugdraw_system::frame_render);
}
}
