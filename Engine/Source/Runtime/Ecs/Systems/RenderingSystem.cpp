#include "RenderingSystem.h"
#include "../Components/TransformComponent.h"
#include "../Components/CameraComponent.h"
#include "../Components/ModelComponent.h"
#include "../../Rendering/RenderSurface.h"
#include "../../Rendering/Camera.h"
#include "../../Rendering/Mesh.h"
#include "../../Rendering/Model.h"
#include "../../Rendering/VertexBuffer.h"
#include "../../Rendering/IndexBuffer.h"
#include "../../Rendering/Program.h"
#include "../../Rendering/Texture.h"
#include "../../Rendering/Material.h"

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

void RenderingSystem::frameRender(EntityManager &entities, EventManager &events, TimeDelta dt)
{
	entities.each<CameraComponent>([this, &entities, dt](
		Entity ce,
		CameraComponent& cameraComponent
		)
	{
		const auto gBufferSurface = cameraComponent.getGBufferSurface();
		const auto camera = cameraComponent.getCamera();
		const auto viewId = gBufferSurface->getId();
		auto& cameraLods = mLodDataMap[ce];

		{
			RenderSurfaceScope surfaceScope(gBufferSurface);
			gBufferSurface->clear();

			gfx::setViewTransform(viewId, &camera->getView(), &camera->getProj());

			entities.each<TransformComponent, ModelComponent>([this, &cameraLods, camera, dt, viewId](
				Entity e,
				TransformComponent& transformComponent,
				ModelComponent& modelComponent
				)
			{
				const auto& model = modelComponent.getModel();
				if (!model.isValid())
					return;

				const auto& worldTransform = transformComponent.getTransform();
				const auto clip_planes = math::vec2(camera->getNearClip(), camera->getFarClip());

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

				const auto& frustum = camera->getFrustum();
				const auto& bounds = hMeshCurr->aabb;

				float t = 0;
				const auto rayOrigin = camera->getPosition();
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
				updateLodData(
					lodData,
					lodCount,
					minDistance,
					maxDistance,
					transitionTime,
					distance,
					dt);
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

				material->setUniform("u_camera_wpos", &camera->getPosition());
				material->setUniform("u_camera_clip_planes", &clip_planes);
				material->setUniform("u_lod_params", &params);
				material->submit();

				// Set render states.
				const auto states = material->getRenderStates();

				auto program = material->getProgram();

				hMeshCurr->submit(viewId, program->handle, worldTransform, states);

				if (currentTime != 0.0f)
				{
					material->setUniform("u_lod_params", &paramsInv);
					material->submit();

					const auto hMeshTarget = model.getLod(targetLodIndex);
					if (!hMeshTarget)
						return;
					hMeshTarget->submit(viewId, program->handle, worldTransform, states);
				}

			});

		}


		{
			auto surface = cameraComponent.getRenderSurface();
			RenderSurfaceScope scope(surface);			

			//this will change soon
		}
		
	});

}

void RenderingSystem::receive(const EntityDestroyedEvent &event)
{
	mLodDataMap.erase(event.entity);
	for (auto& pair : mLodDataMap)
	{
		pair.second.erase(event.entity);
	}
}

void RenderingSystem::configure(EventManager &events)
{
	events.subscribe<EntityDestroyedEvent>(*this);
}
