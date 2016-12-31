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

void updateLodData(LodData& lodData, std::size_t totalLods, float minDist, float maxDist, float transTime, float distanceToCamera, float dt)
{
	if (totalLods == 1)
		return;

	totalLods -= 1;
	if (totalLods < 0)
		totalLods = 0;

	const float factor = 1.0f - math::clamp((maxDist - distanceToCamera) / (maxDist - minDist), 0.0f, 1.0f);
	const int lod = (int)math::lerp(0.0f, static_cast<float>(totalLods), factor);
	if (lodData.targetLodIndex != lod && lodData.targetLodIndex == lodData.currentLodIndex)
		lodData.targetLodIndex = lod;

	if (lodData.currentLodIndex != lodData.targetLodIndex)
		lodData.currentTime += dt;

	if (lodData.currentTime >= transTime)
	{
		lodData.currentLodIndex = lodData.targetLodIndex;
		lodData.currentTime = 0.0f;
	}
}

void RenderingSystem::frame_render(std::chrono::duration<float> dt)
{
	auto ecs = core::get_subsystem<core::EntityComponentSystem>();
	ecs->each<CameraComponent>([this, ecs, dt](
		core::Entity ce,
		CameraComponent& cameraComponent
		)
	{
		const auto gBuffer = cameraComponent.getGBuffer();
		auto& camera = cameraComponent.getCamera();

		RenderPass pass("GBufferFill");
		pass.bind(gBuffer.get());
		pass.clear();
		auto& cameraLods = _lod_data[ce];

		
		gfx::setViewTransform(pass.id, &camera.getView(), &camera.getProj());

		ecs->each<TransformComponent, ModelComponent>([this, &cameraLods, &camera, dt, &pass](
			core::Entity e,
			TransformComponent& transformComponent,
			ModelComponent& modelComponent
			)
		{
			const auto& model = modelComponent.getModel();
			if (!model.isValid())
				return;

			const auto& worldTransform = transformComponent.getTransform();
			const auto clip_planes = math::vec2(camera.getNearClip(), camera.getFarClip());

			auto& lodData = cameraLods[e];
			const auto transitionTime = model.getTransitionTime();
			const auto minDistance = model.getMinDistance();
			const auto maxDistance = model.getMaxDistance();
			const auto lodCount = model.getLods().size();
			const auto currentTime = lodData.currentTime;
			const auto currentLodIndex = lodData.currentLodIndex;
			const auto targetLodIndex = lodData.targetLodIndex;

			auto material = model.getMaterialForGroup({});
			if (!material)
				return;

			const auto hMeshCurr = model.getLod(currentLodIndex);
			if (!hMeshCurr)
				return;

			const auto& frustum = camera.getFrustum();
			const auto& bounds = hMeshCurr->aabb;

			float t = 0.0f;
			const auto rayOrigin = camera.getPosition();
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
			updateLodData(
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
			const auto states = material->getRenderStates();
			material->beginPass();
			material->setUniform("u_camera_wpos", &camera.getPosition());
			material->setUniform("u_camera_clip_planes", &clip_planes);
			material->setUniform("u_lod_params", &params);
			material->submit();
			hMeshCurr->submit(pass.id, material->getProgram()->handle, worldTransform, states);

			if (currentTime != 0.0f)
			{
				material->setUniform("u_lod_params", &paramsInv);
				material->submit();

				const auto hMeshTarget = model.getLod(targetLodIndex);
				if (!hMeshTarget)
					return;
				hMeshTarget->submit(pass.id, material->getProgram()->handle, worldTransform, states);
			}

		});

		
		const auto surface = cameraComponent.getOutputBuffer();
		RenderPass passBlit("OutputBufferFill");
		passBlit.bind(surface.get());
		gfx::blit(passBlit.id, gfx::getTexture(surface->handle), 0, 0, gfx::getTexture(gBuffer->handle));
		
		
	});

}

void RenderingSystem::receive(core::Entity e)
{
	_lod_data.erase(e);
	for (auto& pair : _lod_data)
	{
		pair.second.erase(e);
	}
}



bool RenderingSystem::initialize()
{
	core::onEntityDestroyed.addListener(this, &RenderingSystem::receive);
	runtime::on_frame_render.addListener(this, &RenderingSystem::frame_render);
	return true;
}

void RenderingSystem::dispose()
{
	core::onEntityDestroyed.removeListener(this, &RenderingSystem::receive);
	runtime::on_frame_render.removeListener(this, &RenderingSystem::frame_render);
}