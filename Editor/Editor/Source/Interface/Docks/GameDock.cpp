#include "Docks.h"
#include "../../EditorApp.h"
#include "Runtime/Ecs/World.h"
#include "Runtime/Ecs/Components/CameraComponent.h"
#include "Runtime/Rendering/RenderView.h"

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

			auto renderView = cameraComponent.getRenderView();
			auto& surface = renderView->getRenderSurface();
			auto frameBuffer = surface.getBuffer();
			auto size = gui::GetContentRegionAvail();
			if (size.x > 0 && size.y > 0 && frameBuffer)
			{
				cameraComponent.setViewportSize({ static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y) });

				ImVec2 uv0 = { 0.0f, 0.0f };
				ImVec2 uv1 = { 1.0f, 1.0f };

				auto originBottomLeft = gfx::getCaps()->originBottomLeft;
				if (originBottomLeft)
				{
					uv0 = { 0.0f, 1.0f };
					uv1 = { 1.0f, 0.0f };
				}
				gui::Image(frameBuffer, size, uv0, uv1);

			}
		});
	}

};