#include "Docks.h"
#include "../../EditorApp.h"
#include "Runtime/Ecs/World.h"
#include "Runtime/Ecs/Components/CameraComponent.h"
#include "Runtime/Rendering/RenderSurface.h"

namespace Docks
{
	
	void renderGame(ImVec2 area)
	{
		auto& app = Singleton<EditorApp>::getInstance();
		auto& world = app.getWorld();
		auto& editState = app.getEditState();
		auto& editorCamera = editState.camera;

		world.entities.each<CameraComponent>([&editorCamera](
			Entity e,
			CameraComponent& cameraComponent
			)
		{
			if (e == editorCamera)
				return;

			const auto surface = cameraComponent.getRenderSurface();
			const auto frameBuffer = surface->getBuffer();
			auto size = gui::GetContentRegionAvail();

			cameraComponent.setViewportSize({ static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y) });
			gui::Image(frameBuffer, size);
	
		});
	}

};