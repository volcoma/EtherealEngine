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
	void update_lod_data(LodData& lodData, std::size_t totalLods, float minDist, float maxDist, float transTime, float distanceToCamera, float dt)
	{
		if (totalLods == 1)
			return;

		totalLods -= 1;
		if (totalLods < 0)
			totalLods = 0;

		const float factor = 1.0f - math::clamp((maxDist - distanceToCamera) / (maxDist - minDist), 0.0f, 1.0f);
		const int lod = (int)math::lerp(0.0f, static_cast<float>(totalLods), factor);
		if (lodData.target_lod_index != lod && lodData.target_lod_index == lodData.current_lod_index)
			lodData.target_lod_index = lod;

		if (lodData.current_lod_index != lodData.target_lod_index)
			lodData.current_time += dt;

		if (lodData.current_time >= transTime)
		{
			lodData.current_lod_index = lodData.target_lod_index;
			lodData.current_time = 0.0f;
		}
	}

	void RenderingSystem::frame_render(std::chrono::duration<float> dt)
	{
		auto ecs = core::get_subsystem<EntityComponentSystem>();
		ecs->each<CameraComponent>([this, ecs, dt](
			Entity ce,
			CameraComponent& cameraComponent
			)
		{
			const auto gBuffer = cameraComponent.get_g_buffer();
			auto& camera = cameraComponent.get_camera();

			RenderPass pass("GBufferFill");
			pass.bind(gBuffer.get());
			pass.clear();
			auto& cameraLods = _lod_data[ce];


			gfx::setViewTransform(pass.id, &camera.get_view(), &camera.get_projection());

			ecs->each<TransformComponent, ModelComponent>([this, &cameraLods, &camera, dt, &pass](
				Entity e,
				TransformComponent& transformComponent,
				ModelComponent& modelComponent
				)
			{
				const auto& model = modelComponent.get_model();
				if (!model.is_valid())
					return;

				const auto& worldTransform = transformComponent.get_transform();
				const auto clip_planes = math::vec2(camera.get_near_clip(), camera.get_far_clip());

				auto& lodData = cameraLods[e];
				const auto transitionTime = model.get_lod_transition_time();
				const auto minDistance = model.get_lod_min_distance();
				const auto maxDistance = model.get_lod_max_distance();
				const auto lodCount = model.get_lods().size();
				const auto currentTime = lodData.current_time;
				const auto currentLodIndex = lodData.current_lod_index;
				const auto targetLodIndex = lodData.target_lod_index;

				auto material = model.get_material_for_group({});
				if (!material)
					return;

				const auto hMeshCurr = model.get_lod(currentLodIndex);
				if (!hMeshCurr)
					return;

				const auto& frustum = camera.get_frustum();
				const auto& bounds = hMeshCurr->aabb;

				float t = 0.0f;
				const auto rayOrigin = camera.get_position();
				const auto invWorld = math::inverse(worldTransform);
				const auto objectRayOrigin = invWorld.transformCoord(rayOrigin);
				const auto objectRayDirection = math::normalize(bounds.getCenter() - objectRayOrigin);
				bounds.intersect(objectRayOrigin, objectRayDirection, t);

				// Compute final object space intersection point.
				auto intersectionPoint = objectRayOrigin + (objectRayDirection * t);

				// transform intersection point back into world space to compute
				// the final intersection distance.
				intersectionPoint = worldTransform.transformCoord(intersectionPoint);
				const float distance = math::length(intersectionPoint - rayOrigin);

				//Compute Lods
				update_lod_data(
					lodData,
					lodCount,
					minDistance,
					maxDistance,
					transitionTime,
					distance,
					dt.count());
				// Test the bounding box of the mesh
				if (!math::frustum::testOBB(frustum, bounds, worldTransform))
					return;

				const auto params = math::vec3{
					0.0f,
					-1.0f,
					(transitionTime - currentTime) / transitionTime
				};

				const auto paramsInv = math::vec3{
					1.0f,
					1.0f,
					currentTime / transitionTime
				};
				// Set render states.
				const auto states = material->get_render_states();
				material->begin_pass();
				material->set_uniform("u_camera_wpos", &camera.get_position());
				material->set_uniform("u_camera_clip_planes", &clip_planes);
				material->set_uniform("u_lod_params", &params);
				material->submit();
				hMeshCurr->submit(pass.id, material->get_program()->handle, worldTransform, states);

				if (currentTime != 0.0f)
				{
					material->set_uniform("u_lod_params", &paramsInv);
					material->submit();

					const auto hMeshTarget = model.get_lod(targetLodIndex);
					if (!hMeshTarget)
						return;
					hMeshTarget->submit(pass.id, material->get_program()->handle, worldTransform, states);
				}

			});


			const auto surface = cameraComponent.get_output_buffer();
			RenderPass passBlit("OutputBufferFill");
			passBlit.bind(surface.get());
			gfx::blit(passBlit.id, gfx::getTexture(surface->handle), 0, 0, gfx::getTexture(gBuffer->handle));


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
		onEntityDestroyed.addListener(this, &RenderingSystem::receive);
		on_frame_render.addListener(this, &RenderingSystem::frame_render);
		return true;
	}

	void RenderingSystem::dispose()
	{
		onEntityDestroyed.removeListener(this, &RenderingSystem::receive);
		on_frame_render.removeListener(this, &RenderingSystem::frame_render);
	}
}