#include "rendering_system.h"
#include "../components/transform_component.h"
#include "../components/camera_component.h"
#include "../components/model_component.h"
#include "../../rendering/render_pass.h"
#include "../../rendering/camera.h"
#include "../../rendering/mesh.h"
#include "../../rendering/model.h"
#include "../../rendering/vertex_buffer.h"
#include "../../rendering/index_buffer.h"
#include "../../rendering/program.h"
#include "../../rendering/texture.h"
#include "../../rendering/material.h"
#include "../../system/engine.h"


namespace runtime
{
	void update_lod_data(LodData& lod_data, std::size_t total_lods, float min_dist, float max_dist, float transition_time, float distance, float dt)
	{
		if (total_lods == 1)
			return;

		total_lods -= 1;
		if (total_lods < 0)
			total_lods = 0;

		const float factor = 1.0f - math::clamp((max_dist - distance) / (max_dist - min_dist), 0.0f, 1.0f);
		const int lod = (int)math::lerp(0.0f, static_cast<float>(total_lods), factor);
		if (lod_data.target_lod_index != lod && lod_data.target_lod_index == lod_data.current_lod_index)
			lod_data.target_lod_index = lod;

		if (lod_data.current_lod_index != lod_data.target_lod_index)
			lod_data.current_time += dt;

		if (lod_data.current_time >= transition_time)
		{
			lod_data.current_lod_index = lod_data.target_lod_index;
			lod_data.current_time = 0.0f;
		}
	}

	void RenderingSystem::frame_render(std::chrono::duration<float> dt)
	{
		auto ecs = core::get_subsystem<EntityComponentSystem>();
		ecs->each<CameraComponent>([this, ecs, dt](
			Entity ce,
			CameraComponent& camera_comp
			)
		{
			const auto g_buffer = camera_comp.get_g_buffer();
			auto& camera = camera_comp.get_camera();

			RenderPass pass("g_buffer_fill");
			pass.bind(g_buffer.get());
			pass.clear();
			auto& camera_lods = _lod_data[ce];

			gfx::setViewTransform(pass.id, &camera.get_view(), &camera.get_projection());

			ecs->each<TransformComponent, ModelComponent>([this, &camera_lods, &camera, dt, &pass](
				Entity e,
				TransformComponent& transformComponent,
				ModelComponent& modelComponent
				)
			{
				const auto& model = modelComponent.get_model();
				if (!model.is_valid())
					return;

				const auto& world_transform = transformComponent.get_transform();
				const auto clip_planes = math::vec2(camera.get_near_clip(), camera.get_far_clip());

				auto& lod_data = camera_lods[e];
				const auto transition_time = model.get_lod_transition_time();
				const auto min_distance = model.get_lod_min_distance();
				const auto max_distance = model.get_lod_max_distance();
				const auto lod_count = model.get_lods().size();
				const auto current_time = lod_data.current_time;
				const auto current_lod_index = lod_data.current_lod_index;
				const auto target_lod_index = lod_data.target_lod_index;

				const auto current_mesh = model.get_lod(current_lod_index);
				if (!current_mesh)
					return;

				const auto& frustum = camera.get_frustum();
				const auto& bounds = current_mesh->aabb;

				float t = 0.0f;
				const auto ray_origin = camera.get_position();
				const auto inv_world = math::inverse(world_transform);
				const auto object_ray_origin = inv_world.transform_coord(ray_origin);
				const auto object_ray_direction = math::normalize(bounds.get_center() - object_ray_origin);
				bounds.intersect(object_ray_origin, object_ray_direction, t);

				// Compute final object space intersection point.
				auto intersection_point = object_ray_origin + (object_ray_direction * t);

				// transform intersection point back into world space to compute
				// the final intersection distance.
				intersection_point = world_transform.transform_coord(intersection_point);
				const float distance = math::length(intersection_point - ray_origin);

				//Compute Lods
				update_lod_data(
					lod_data,
					lod_count,
					min_distance,
					max_distance,
					transition_time,
					distance,
					dt.count());

				// Test the bounding box of the mesh
				if (!math::frustum::test_obb(frustum, bounds, world_transform))
					return;

				const auto params = math::vec3{
					0.0f,
					-1.0f,
					(transition_time - current_time) / transition_time
				};

				const auto params_inv = math::vec3{
					1.0f,
					1.0f,
					current_time / transition_time
				};
				
				model.render(
					pass.id,
					world_transform,
					true,
					true,
					true,
					0,
					current_lod_index,
					nullptr,
					[&camera, &clip_planes, &params](Program& program)
				{
					program.set_uniform("u_camera_wpos", &camera.get_position());
					program.set_uniform("u_camera_clip_planes", &clip_planes);
					program.set_uniform("u_lod_params", &params);
				});

				if (current_time != 0.0f)
				{
					model.render(
						pass.id,
						world_transform,
						true,
						true,
						true,
						0, 
						target_lod_index,
						nullptr,
						[&camera, &clip_planes, &params_inv](Program& program)
					{
						program.set_uniform("u_lod_params", &params_inv);
					});
				}

			});

			const auto surface = camera_comp.get_output_buffer();
			RenderPass pass_blit("output_buffer_fill");
			pass_blit.bind(surface.get());
			gfx::blit(pass_blit.id, gfx::getTexture(surface->handle), 0, 0, gfx::getTexture(g_buffer->handle));
		});
	}

	void RenderingSystem::receive(Entity e)
	{
		_lod_data.erase(e);
		for (auto& pair : _lod_data)
		{
			pair.second.erase(e);
		}
	}



	bool RenderingSystem::initialize()
	{
		onEntityDestroyed.connect(this, &RenderingSystem::receive);
		on_frame_render.connect(this, &RenderingSystem::frame_render);
		return true;
	}

	void RenderingSystem::dispose()
	{
		onEntityDestroyed.disconnect(this, &RenderingSystem::receive);
		on_frame_render.disconnect(this, &RenderingSystem::frame_render);
	}
}