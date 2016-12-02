#include "DebugDrawSystem.h"
#include "Runtime/ecs/Components/TransformComponent.h"
#include "Runtime/ecs/Components/CameraComponent.h"
#include "Runtime/ecs/Components/ModelComponent.h"
#include "Runtime/ecs/Components/LightComponent.h"
#include "Runtime/Rendering/RenderSurface.h"
#include "Runtime/Rendering/Camera.h"
#include "Runtime/Rendering/Mesh.h"
#include "Runtime/Rendering/Model.h"
#include "Runtime/Rendering/VertexBuffer.h"
#include "Runtime/Rendering/IndexBuffer.h"
#include "Runtime/Rendering/Program.h"
#include "Runtime/Rendering/Texture.h"
#include "Runtime/Rendering/Material.h"
#include "Runtime/Rendering/Debug/DebugDraw.h"
#include "Runtime/Assets/AssetManager.h"
#include "../EditorApp.h"
DebugDrawSystem::DebugDrawSystem()
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& manager = app.getAssetManager();
	manager.load<Shader>("editor://shaders/vs_wf_wireframe", false)
		.then([this, &manager](auto vs)
	{
		manager.load<Shader>("editor://shaders/fs_wf_wireframe", false)
			.then([this, vs](auto fs)
		{
			mProgram = std::make_unique<Program>(vs, fs);
		});
	});
}

void DebugDrawSystem::frameRender(ecs::EntityManager &entities, ecs::EventManager &events, ecs::TimeDelta dt)
{	
	auto& app = Singleton<EditorApp>::getInstance();
	auto& editState = app.getEditState();

	auto& editorCamera = editState.camera;
	auto& selected = editState.selected;
	if (!editorCamera || 
		!editorCamera.has_component<CameraComponent>())
		return;

	const auto cameraComponentRef = editorCamera.component<CameraComponent>();
	const auto cameraComponent = cameraComponentRef.lock();
	const auto surface = cameraComponent->getOutputBuffer();
	auto& camera = cameraComponent->getCamera();
	const auto& view = camera.getView();
	const auto proj = camera.getProj();
	const auto cameraPos = camera.getPosition();

	const auto viewId = surface->getId();
	RenderSurfaceScope surfaceScope(surface);
	gfx::setViewTransform(viewId, &view, &proj);

	ddRAII dd(viewId);
	const std::uint32_t colorGrid = 0xff606060;
	auto drawGrid = [](std::uint32_t gridColor, float height, float heightIntervals, std::uint32_t gridSize, std::uint32_t sizeIntervals, std::uint32_t iteration, std::uint32_t maxIterations)
	{
		std::uint32_t detailGridColor = gridColor;

		bool shouldRender = true;
		if (iteration + 1 != maxIterations)
		{
			const auto iterationHeight = heightIntervals * float(iteration + 1);
			const float factor = math::clamp(height, 0.0f, iterationHeight) / iterationHeight;
			std::uint32_t r = (gridColor) & 0xff;
			std::uint32_t g = (gridColor >> 8) & 0xff;
			std::uint32_t b = (gridColor >> 16) & 0xff;
			std::uint32_t a = (gridColor >> 24) & 0xff;
			a = static_cast<std::uint32_t>(math::lerp(255.0f, 0.0f, factor));
			if (a < 0.01f)
				shouldRender = false;

			detailGridColor = r + (g << 8) + (b << 16) + (a << 24);
		}


		if (shouldRender)
		{
			auto step = (sizeIntervals * iteration);
			step = step ? step : 1;
			ddPush();
			ddSetColor(detailGridColor);
			ddDrawGrid(Axis::Y, math::vec3{ 0.0f, 0.0f, 0.0f }, gridSize / step, float(step));
			ddPop();
		}

	};

	if (editState.showGrid)
	{
		static const auto farClip = 200;
		static const auto height = 40.0f;
		static const auto divison = 10;
		const auto iterations = math::power_of_n_round_down(farClip, divison);
		for (std::uint32_t i = 0; i < iterations; ++i)
		{
			drawGrid(colorGrid, math::abs(cameraPos.y), height, farClip, divison, i, iterations);
		}
	}
	

	if (!selected || !selected.is_type<ecs::Entity>())
		return;

	auto selectedEntity = selected.get_value<ecs::Entity>();

	if (!selectedEntity ||
		!selectedEntity.has_component<TransformComponent>())
		return;


	auto& transformComponent = *selectedEntity.component<TransformComponent>().lock();
	const auto& worldTransform = transformComponent.getTransform();

	if (selectedEntity.has_component<CameraComponent>() && selectedEntity != editorCamera)
	{
		auto& cameraComponent = *selectedEntity.component<CameraComponent>().lock();
		auto& selectedCamera = cameraComponent.getCamera();

		const auto& frust = selectedCamera.getFrustum();
		ddPush();
		ddSetColor(0xffffffff);
		ddSetTransform(nullptr);
		ddDrawFrustum(frust);
		ddPop();
	}

	if (selectedEntity.has_component<LightComponent>())
	{
		auto& lightComponent = *selectedEntity.component<LightComponent>().lock();
		auto& light = lightComponent.getLight();
		if (light.lightType == LightType::Spot)
		{
			auto sinAngle = math::max(math::sin(math::radians(light.spotData.spotOuterAngle * 2.0f)), 0.1f);
			auto height = 10.0f;//light.getRange();
			auto radius = sinAngle * height;
			ddPush();
			ddSetColor(0xff00ff00);
			ddSetTransform(&worldTransform);
			ddDrawCone(math::vec3(0.0f, 0.0f, height), math::vec3(0.0f, 0.0f, 0.0f), radius);
			ddPop();
		}
		else if (light.lightType == LightType::Point)
		{
			auto radius = 10.0f;//light.getRange();
			ddPush();
			ddSetColor(0xff00ff00);
			ddSetTransform(&worldTransform);
			ddDrawCircle(Axis::X, 0.0f, 0.0f, 0.0f, radius);
			ddDrawCircle(Axis::Y, 0.0f, 0.0f, 0.0f, radius);
			ddDrawCircle(Axis::Z, 0.0f, 0.0f, 0.0f, radius);
			ddPop();
		}
		else if (light.lightType == LightType::Directional)
		{
			ddPush();
			ddSetLod(UINT8_MAX);
			ddSetColor(0xff00ff00);
			ddSetTransform(&worldTransform);
			ddSetWireframe(false);
			ddDrawCylinder(math::vec3(0.0f, 0.0f, 0.0f), math::vec3(0.0f, 0.0f, 1.0f), 0.1f);
			ddDrawCone(math::vec3(0.0f, 0.0f, 1.0f), math::vec3(0.0f, 0.0f, 2.5f), 0.5f);
			ddPop();
		}
	}
	
	if (selectedEntity.has_component<ModelComponent>())
	{
		const auto& modelComponent = *selectedEntity.component<ModelComponent>().lock();
		const auto& model = modelComponent.getModel();
		if (!model.isValid())
			return;

		const auto hMesh = model.getLod(0);
		if (!hMesh)
			return;
		const auto& frustum = camera.getFrustum();
		const auto& bounds = hMesh->aabb;

		// Test the bounding box of the mesh
		if (math::frustum::testOBB(frustum, bounds, worldTransform))
		{
			ddPush();
			ddSetColor(0xff00ff00);
			ddSetTransform(&worldTransform);
			ddDraw(Aabb{ bounds.min, bounds.max });
			ddPop();

			if (editState.wireframeSelection)
			{
				const float u_params[8] =
				{
					1.0f, 1.0f, 0.0f, 1.0f,
					1.0f, 2.0f, 0.0f, 0.0f
				};
				mProgram->setUniform("u_params", u_params, 2);
				const auto material = model.getMaterialForGroup({});
				if (!material)
					return;
				const std::uint64_t state = material->getRenderStates(false, false, false)
					| BGFX_STATE_BLEND_FUNC(BGFX_STATE_BLEND_SRC_ALPHA, BGFX_STATE_BLEND_INV_SRC_ALPHA)
					;
				hMesh->submit(viewId, mProgram->handle, worldTransform, state);
			}	
		}
	}
}
