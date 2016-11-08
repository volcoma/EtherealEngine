#include "PickingSystem.h"
#include "Runtime/ecs/Components/TransformComponent.h"
#include "Runtime/ecs/Components/CameraComponent.h"
#include "Runtime/ecs/Components/ModelComponent.h"
#include "Runtime/Assets/AssetManager.h"
#include "Runtime/Rendering/RenderView.h"
#include "Runtime/Rendering/Camera.h"
#include "Runtime/Rendering/Mesh.h"
#include "Runtime/Rendering/Model.h"
#include "Runtime/Rendering/VertexBuffer.h"
#include "Runtime/Rendering/IndexBuffer.h"
#include "Runtime/Rendering/Program.h"
#include "Runtime/Rendering/Texture.h"
#include "Runtime/Rendering/Material.h"
#include "Runtime/Rendering/RenderWindow.h"
#include "../EditorApp.h"
#include "../Interface/GuiWindow.h"
PickingSystem::PickingSystem()
{
	mRenderView = std::make_shared<RenderView>();
	// ID buffer clears to black, which represents clicking on nothing (background)
	mRenderView->clear(BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH
		, 0x000000ff
		, 1.0f
		, 0);


	// Set up ID buffer, which has a color target and depth buffer
	auto pickingRT = std::make_shared<Texture>(_id_dimensions, _id_dimensions, false, 1, gfx::TextureFormat::RGBA8, 0
		| BGFX_TEXTURE_RT
		| BGFX_TEXTURE_MIN_POINT
		| BGFX_TEXTURE_MAG_POINT
		| BGFX_TEXTURE_MIP_POINT
		| BGFX_TEXTURE_U_CLAMP
		| BGFX_TEXTURE_V_CLAMP
		);

	auto pickingRTDepth = std::make_shared<Texture>(_id_dimensions, _id_dimensions, false, 1, gfx::TextureFormat::D24S8, 0
		| BGFX_TEXTURE_RT
		| BGFX_TEXTURE_MIN_POINT
		| BGFX_TEXTURE_MAG_POINT
		| BGFX_TEXTURE_MIP_POINT
		| BGFX_TEXTURE_U_CLAMP
		| BGFX_TEXTURE_V_CLAMP
	);

	auto& surface = mRenderView->getRenderSurface();
	surface.populate
	(
		std::vector<std::shared_ptr<Texture>>
		{
			pickingRT,
			pickingRTDepth
		}
	);

	// CPU texture for blitting to and reading ID buffer so we can see what was clicked on.
	// Impossible to read directly from a render target, you *must* blit to a CPU texture
	// first. Algorithm Overview: Render on GPU -> Blit to CPU texture -> Read from CPU
	// texture.
	mBlitTex = std::make_shared<Texture>(_id_dimensions, _id_dimensions, false, 1, gfx::TextureFormat::RGBA8, 0
		| BGFX_TEXTURE_BLIT_DST
		| BGFX_TEXTURE_READ_BACK
		| BGFX_TEXTURE_MIN_POINT
		| BGFX_TEXTURE_MAG_POINT
		| BGFX_TEXTURE_MIP_POINT
		| BGFX_TEXTURE_U_CLAMP
		| BGFX_TEXTURE_V_CLAMP
	);

	

	auto& app = Singleton<EditorApp>::getInstance();
	auto& manager = app.getAssetManager();
	manager.load<Shader>("editor://shaders/vs_picking_id", false)
		.then([this, &manager](auto vs)
	{
		manager.load<Shader>("editor://shaders/fs_picking_id", false)
			.then([this, vs](auto fs)
		{
			mProgram = std::make_unique<Program>(vs, fs);
		});
	});
}

void PickingSystem::frameRender(ecs::EntityManager &entities, ecs::EventManager &events, ecs::TimeDelta dt)
{

	auto& app = Singleton<EditorApp>::getInstance();
	auto& editState = app.getEditState();
	auto& input = app.getInput();
	auto& world = app.getWorld();
	auto& window = static_cast<GuiWindow&>(app.getWindow());
	auto& dockspace = window.getDockspace();
	if (!dockspace.hasDock("Scene"))
		return;
	
	const auto renderFrame = app.getRenderFrame();

	auto& editorCamera = editState.camera;
	if (!editorCamera || !editorCamera.has_component<CameraComponent>() || ImGuizmo::IsOver() || ImGuizmo::IsUsing())
		return;

	auto cameraComponentRef = editorCamera.component<CameraComponent>();
	auto cameraComponent = cameraComponentRef.lock();
	auto transformComponentRef = editorCamera.component<TransformComponent>();
	auto transformComponent = transformComponentRef.lock();
	const auto camera = cameraComponent->getCamera();
	const auto& view = camera->getView();
	const auto& proj = camera->getProj();
	auto nearClip = camera->getNearClip();
	auto farClip = camera->getFarClip();
	auto viewProj = proj * view;
	auto invViewProj = math::inverse(viewProj);
	const auto& size = camera->getViewportSize();
	const auto& pos = camera->getViewportPos();
	const auto& mousePos = input.getMouseCurrentPosition();

	float mouseXNDC = ((float(mousePos.x) - float(pos.x)) / (float(size.width))) * 2.0f - 1.0f;
	float mouseYNDC = ((float(size.height) - (float(mousePos.y) - float(pos.y))) / float(size.height)) * 2.0f - 1.0f;
	
	// Check if we are testing outside our view
	if (mouseXNDC < -1.0f ||
		mouseXNDC > 1.0f ||
		mouseYNDC < -1.0f ||
		mouseYNDC > 1.0f)
		return;

	// If the user previously clicked, and we're done reading data from GPU, look at ID buffer on CPU
	// Whatever mesh has the most pixels in the ID buffer is the one the user clicked on.
	if (!mReading)
	{
		auto& surface = mRenderView->getRenderSurface();
		// Blit and read
		gfx::blit(0, mBlitTex->handle, 0, 0, surface.getBufferRaw()->handle);
		mReading = gfx::readTexture(mBlitTex->handle, mBlitData);
	}

	if(input.isMouseButtonPressed(sf::Mouse::Left))
	{
		mReading = 0;
		math::vec4 mousePosNDC = { mouseXNDC, mouseYNDC, 0.0f, 1.0f };
		math::vec4 mousePosNDCEnd = { mouseXNDC, mouseYNDC, 1.0f, 1.0f };

		// Un-project and perspective divide
		math::vec4 rayBegin = invViewProj.matrix() * mousePosNDC;
		rayBegin *= 1.0f / rayBegin.w;
		math::vec4 rayEnd = invViewProj.matrix() * mousePosNDCEnd;
		rayEnd *= 1.0f / rayEnd.w;

		math::vec3 pickEye = rayBegin;
		math::vec3 pickAt = rayEnd;
		math::vec3 pickUp = { 0.0f, 1.0f, 0.0f };

		auto pickView = math::lookAt(pickEye, pickAt, pickUp);
		auto pickProj = math::perspective(math::radians(1.0f), 1.0f, nearClip, farClip);

		auto viewId = mRenderView->getId();
		// View rect and transforms for picking pass
		RenderViewRAII pushView(mRenderView);
		gfx::setViewTransform(viewId, &pickView, &pickProj);

		entities.each<TransformComponent, ModelComponent>([this, viewId, camera, dt](
			ecs::Entity e,
			TransformComponent& transformComponent,
			ModelComponent& modelComponent
			)
		{
			auto& model = modelComponent.getModel();
			if (!model.isValid())
				return;

			const auto& worldTransform = transformComponent.getTransform();
		
			auto material = model.getMaterialForGroup({});
			if (!material)
				return;

			auto hMesh = model.getLod(0);
			if (!hMesh)
				return;

			const auto& frustum = camera->getFrustum();
			const auto& bounds = hMesh->aabb;

			// Test the bounding box of the mesh
			if (!math::frustum::testOBB(frustum, bounds, worldTransform))
				return;

			auto entityIndex = e.id().index();
			std::uint32_t rr = (entityIndex		 ) & 0xff;
			std::uint32_t gg = (entityIndex >> 8 ) & 0xff;
			std::uint32_t bb = (entityIndex >> 16) & 0xff;
			math::vec4 colorId =
			{
				rr / 255.0f,
				gg / 255.0f,
				bb / 255.0f,
				1.0f
			};

			mProgram->setUniform("u_id", &colorId);

			// Set render states.
			auto states = material->getRenderStates();

			hMesh->submit(viewId, mProgram->handle, worldTransform, states);	
		});	
	}

	if (mReading && mReading <= renderFrame)
	{
		mReading = 0;
		std::map<std::uint32_t, std::uint32_t> ids;  // This contains all the IDs found in the buffer
		std::uint32_t maxAmount = 0;
		for (std::uint8_t* x = mBlitData; x < mBlitData + _id_dimensions * _id_dimensions * 4;)
		{
			std::uint8_t rr = *x++;
			std::uint8_t gg = *x++;
			std::uint8_t bb = *x++;
			std::uint8_t aa = *x++;

			const gfx::Caps* caps = gfx::getCaps();
			if (gfx::RendererType::Direct3D9 == caps->rendererType)
			{
				// Comes back as BGRA
				std::swap(rr, bb);
			}

			// Skip background
			if (0 == (rr | gg | bb))
			{
				continue;
			}

			std::uint32_t hashKey = rr + (gg << 8) + (bb << 16);
			std::uint32_t amount = 1;
			auto mapIter = ids.find(hashKey);
			if (mapIter != ids.end())
			{
				amount = mapIter->second + 1;
			}

			// Amount of times this ID (color) has been clicked on in buffer
			ids[hashKey] = amount;
			maxAmount = maxAmount > amount ? maxAmount : amount;
		}

		std::uint32_t idKey = 0;
		if (maxAmount)
		{
			for (auto& pair : ids)
			{
				if (pair.second == maxAmount)
				{
					idKey = pair.first;
					if (world.entities.valid_index(idKey))
					{
						auto eid = world.entities.create_id(idKey);
						auto pickedEntity = world.entities.get(eid);
						if (pickedEntity)
							editState.select(pickedEntity);
					}
					break;
				}
			}

		}
		else
		{
			editState.unselect();
		}
	}
}
