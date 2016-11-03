#include "SceneView.h"
#include "../EditorApp.h"
#include "../Gizmos/ImGuizmo.h"

void showStatistics(const Timer& timer, const World& world)
{
	auto totalEntiteis = world.entities.size();
	ImVec2 pos = gui::GetCursorScreenPos();
	gui::SetNextWindowPos(pos);
	gui::Begin("Statistics", nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_AlwaysAutoResize);
	gui::Text("FPS  : %u", timer.getFrameRate());
	gui::Separator();
	gui::Text("MSPF : %.3f ms ", 1000.0f / (float)timer.getFrameRate());
	gui::Separator();
	gui::Text("Total Entities  : %u", totalEntiteis);
	gui::Separator();
	
	static bool debug = false;
	if (gui::Checkbox("Render Stats", &debug))
	{
		if(debug)
			gfx::setDebug(BGFX_DEBUG_STATS);
		else
			gfx::setDebug(BGFX_DEBUG_NONE);
	}
	gui::End();

}

void drawSelectedCamera(const ImVec2& size)
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& input = app.getInputManager();
	auto& editState = app.getEditState();
	auto& selection = editState.entitySelection;
	auto& selected = selection.selected;
	auto& editCamera = editState.editorCamera;

	if (selected && (editCamera != selected) && selected.has_component<CameraComponent>())
	{
		auto selectedCamera = selected.component<CameraComponent>();
		RenderView* view = selectedCamera.lock()->getRenderView();

		ddBegin(editCamera.component<CameraComponent>().lock()->getRenderView()->getId());
		ddSetState(false, false, false);
		auto fr = selectedCamera.lock()->getCamera()->getFrustum();
		ddDrawFrustum(fr);
		ddEnd();

		auto viewSize = view->getSize();

		float factor = std::min(size.x / float(viewSize.width), size.y / float(viewSize.height)) / 4.0f;
		ImVec2 bounds(viewSize.width * factor, viewSize.height * factor);
		auto p = gui::GetWindowPos();
		p.x += size.x - bounds.x - 20.0f;
		p.y += size.y - bounds.y - 40.0f;
		gui::SetNextWindowPos(p);
		if (gui::Begin("Camera Preview", nullptr, ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_AlwaysAutoResize))
		{
			auto frameBuffer = view->getViewBuffer().get();
			if (frameBuffer)
			{
				ImVec2 uv0 = { 0.0f, 0.0f };
				ImVec2 uv1 = { 1.0f, 1.0f };

				auto originBottomLeft = gfx::getCaps()->originBottomLeft;
				if (originBottomLeft)
				{
					uv0 = { 0.0f, 1.0f };
					uv1 = { 1.0f, 0.0f };
				}
				gui::Image(frameBuffer, bounds, uv0, uv1);
			}
		}
		gui::End();

		if (input.isKeyPressed(sf::Keyboard::F))
		{
			auto transform = editCamera.component<TransformComponent>().lock();
			auto transformSelected = selected.component<TransformComponent>().lock();
			transformSelected->setTransform(transform->getTransform());
		}
	}
}


void manipulationGizmos()
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& input = app.getInputManager();
	auto& editState = app.getEditState();
	auto& selection = editState.entitySelection;
	auto& selected = selection.selected;
	auto& editCamera = editState.editorCamera;
	auto& operation = editState.operation;
	auto& mode = editState.mode;

	if (!input.isMouseButtonDown(sf::Mouse::Right) && gui::IsWindowFocused())
	{
		if (input.isKeyPressed(sf::Keyboard::W))
		{
			operation = ImGuizmo::OPERATION::TRANSLATE;
		}
		if (input.isKeyPressed(sf::Keyboard::E))
		{
			operation = ImGuizmo::OPERATION::ROTATE;
		}
		if (input.isKeyPressed(sf::Keyboard::R))
		{
			operation = ImGuizmo::OPERATION::SCALE;
			mode = ImGuizmo::MODE::LOCAL;
		}
		if (input.isKeyPressed(sf::Keyboard::T))
		{
			mode = ImGuizmo::MODE::LOCAL;
		}
		if (input.isKeyPressed(sf::Keyboard::Y) && operation != ImGuizmo::OPERATION::SCALE)
		{
			mode = ImGuizmo::MODE::WORLD;
		}
	}

	if (selected && selected != editCamera)
	{
		auto p = gui::GetItemRectMin();
		auto s = gui::GetItemRectSize();
		ImGuizmo::SetViewRect(p.x, p.y, s.x, s.y);
		auto cameraComponent = editCamera.component<CameraComponent>().lock();
		auto transformComponent = selected.component<TransformComponent>().lock();
		transformComponent->resolveTransform(true);
		auto transform = transformComponent->getTransform();
		math::transform delta;

		float* snap = nullptr;
		static math::vec3 translationSnap = { 1.0f, 1.0f, 1.0f };
		static float rotationDegreeSnap = 15.0f;
		static float scaleSnap = 0.1f;
		if (input.isKeyDown(sf::Keyboard::LControl))
		{
			if (operation == ImGuizmo::OPERATION::TRANSLATE)
				snap = &translationSnap[0];
			else if (operation == ImGuizmo::OPERATION::ROTATE)
				snap = &rotationDegreeSnap;
			else if (operation == ImGuizmo::OPERATION::SCALE)
				snap = &scaleSnap;
		}
		
		ImGuizmo::Manipulate(
			cameraComponent->getCamera()->getView(),
			cameraComponent->getCamera()->getProj(),
			operation,
			mode,
			transform,
			nullptr,
			snap);

		transformComponent->setTransform(transform);
		
	}
}

void handleCameraMovement()
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& input = app.getInputManager();
	auto& timer = app.getTimer();
	auto& editState = app.getEditState();
	auto& selection = editState.entitySelection;
	auto& selected = selection.selected;
	auto& editCamera = editState.editorCamera;

	auto transform = editCamera.component<TransformComponent>().lock();
	float movementSpeed = 5.0f;
	float rotationSpeed = 0.2f;
	float boostMultiplier = 5.0f;
	float dt = (float)timer.getDeltaTime();
	iPoint deltaMove = { input.getMouseCurrentPosition().x - input.getMousePreviousPosition().x, input.getMouseCurrentPosition().y - input.getMousePreviousPosition().y };
	
	if (input.isMouseButtonDown(sf::Mouse::Middle))
	{
		if (input.isKeyDown(sf::Keyboard::LShift))
		{
			movementSpeed *= boostMultiplier;
		}

		if (deltaMove.x != 0)
		{
			transform->moveLocal({ -1 * deltaMove.x * movementSpeed * dt, 0.0f, 0.0f });
		}
		if (deltaMove.y != 0)
		{
			transform->moveLocal({ 0.0f, deltaMove.y * movementSpeed * dt, 0.0f });
		}
	}

	if (input.isMouseButtonDown(sf::Mouse::Right))
	{

		if (input.isKeyDown(sf::Keyboard::LShift))
		{
			movementSpeed *= boostMultiplier;
		}

		if (input.isKeyDown(sf::Keyboard::W))
		{
			transform->moveLocal({ 0.0f, 0.0f, movementSpeed * dt });
		}

		if (input.isKeyDown(sf::Keyboard::S))
		{
			transform->moveLocal({ 0.0f, 0.0f, -movementSpeed * dt });
		}

		if (input.isKeyDown(sf::Keyboard::A))
		{
			transform->moveLocal({ -movementSpeed * dt, 0.0f, 0.0f });
		}

		if (input.isKeyDown(sf::Keyboard::D))
		{
			transform->moveLocal({ movementSpeed * dt, 0.0f, 0.0f });
		}
		if (input.isKeyDown(sf::Keyboard::Up))
		{
			transform->moveLocal({ 0.0f, 0.0f, movementSpeed * dt });
		}

		if (input.isKeyDown(sf::Keyboard::Down))
		{
			transform->moveLocal({ 0.0f, 0.0f, -movementSpeed * dt });
		}

		if (input.isKeyDown(sf::Keyboard::Left))
		{
			transform->moveLocal({ -movementSpeed * dt, 0.0f, 0.0f });
		}

		if (input.isKeyDown(sf::Keyboard::Right))
		{
			transform->moveLocal({ movementSpeed * dt, 0.0f, 0.0f });
		}

		if (input.isKeyDown(sf::Keyboard::Space))
		{
			transform->moveLocal({ 0.0f, movementSpeed * dt, 0.0f });
		}

		if (input.isKeyDown(sf::Keyboard::LControl))
		{
			transform->moveLocal({ 0.0f, -movementSpeed * dt, 0.0f });
		}

		float x = static_cast<float>(deltaMove.x);
		float y = static_cast<float>(deltaMove.y);

		// Make each pixel correspond to a quarter of a degree.
		float dx = x * rotationSpeed;
		float dy = y * rotationSpeed;

		transform->resolveTransform(true);
		transform->rotate(0.0f, dx, 0.0f);
		transform->rotateLocal(dy, 0.0f, 0.0f);


		float DeltaWheel = input.getMouseWheelScrollDelta();
		transform->moveLocal({ 0.0f, 0.0f, 14.0f * movementSpeed * DeltaWheel * dt });
	}
}

SceneView::SceneView()
{
	mName = "Scene";

	
}

SceneView::~SceneView()
{
}

void SceneView::render(AppWindow& window)
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& manager = app.getAssetManager();
	auto& timer = app.getTimer();
	auto& input = app.getInputManager();
	auto& world = app.getWorld();
	auto& editState = app.getEditState();
	auto& icons = editState.icons;
	auto& selection = editState.entitySelection;
	auto& selected = selection.selected;
	auto& editCamera = editState.editorCamera;
	bool hasEditCamera = editCamera
		&& editCamera.has_component<CameraComponent>()
		&& editCamera.has_component<TransformComponent>();


	ImGuiIO& io = gui::GetIO();
	ImVec2 sz = ImVec2(io.DisplaySize.x / 2, io.DisplaySize.y / 2);

	gui::SetNextWindowPos(ImVec2(sz.x - sz.x / 2, sz.y - sz.y / 2));
	gui::SetNextWindowSize(sz, ImGuiSetCond_FirstUseEver);
	if (!gui::BeginDock(mName.c_str(), &mOpen))
	{
		gui::EndDock();
		return;
	}

	auto pos = gui::GetCursorScreenPos();
	if (gui::BeginToolbar("scene_view_toolbar", pos, ImVec2(gui::GetContentRegionAvailWidth(), 24)))
	{
		if (gui::ToolbarButton(icons["translate"].get(), "Translate", editState.operation == ImGuizmo::OPERATION::TRANSLATE))
		{
			editState.operation = ImGuizmo::OPERATION::TRANSLATE;
		}
		gui::SameLine(0.0f);
		if (gui::ToolbarButton(icons["rotate"].get(), "Rotate", editState.operation == ImGuizmo::OPERATION::ROTATE))
		{
			editState.operation = ImGuizmo::OPERATION::ROTATE;
		}
		gui::SameLine(0.0f);
		if (gui::ToolbarButton(icons["scale"].get(), "Scale", editState.operation == ImGuizmo::OPERATION::SCALE))
		{
			editState.operation = ImGuizmo::OPERATION::SCALE;
			editState.mode = ImGuizmo::MODE::LOCAL;
		}
		gui::SameLine(0.0f, 50.0f);

		if (gui::ToolbarButton(icons["local"].get(), "Local Coordinate System", editState.mode == ImGuizmo::MODE::LOCAL))
		{
			editState.mode = ImGuizmo::MODE::LOCAL;
		}
		gui::SameLine(0.0f);
		if (gui::ToolbarButton(icons["global"].get(), "Global Coordinate System", editState.mode == ImGuizmo::MODE::WORLD, editState.operation != ImGuizmo::OPERATION::SCALE))
		{
			editState.mode = ImGuizmo::MODE::WORLD;
		}
		gui::SameLine(0.0f, 50.0f);
		if (gui::ToolbarButton(icons["play"].get(), "Play", false))
		{
			app.registerWindow(std::make_shared<AppWindow>(sf::VideoMode{ 800, 600 }, "Game Window"));

		}
		gui::SameLine(0.0f);
		if (gui::ToolbarButton(icons["pause"].get(), "Pause", false))
		{

		}
		gui::SameLine(0.0f);
		if (gui::ToolbarButton(icons["stop"].get(), "Stop", false))
		{
		
		}
		gui::SameLine(0.0f);
		if (gui::ToolbarButton(icons["next"].get(), "Step", false))
		{
			
		}


		gui::EndToolbar();
	}

	showStatistics(timer, world);
	
	if (!hasEditCamera)
	{
		gui::EndDock();
		return;
	}

	auto size = gui::GetContentRegionAvail();
	
	drawSelectedCamera(size);

 	auto cameraComponent = editCamera.component<CameraComponent>().lock();
	RenderView* view = cameraComponent->getRenderView();
	auto frameBuffer = view->getViewBuffer().get();
	if (size.x > 0 && size.y > 0 && view && frameBuffer)
	{
		cameraComponent->setViewportSize({static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y) });
		
		ImVec2 uv0 = { 0.0f, 0.0f };
		ImVec2 uv1 = { 1.0f, 1.0f };

		auto originBottomLeft = gfx::getCaps()->originBottomLeft;
		if (originBottomLeft)
		{
			uv0 = { 0.0f, 1.0f };
			uv1 = { 1.0f, 0.0f };
		}
		gui::Image(frameBuffer, size, uv0, uv1);	
		manipulationGizmos();
 		if (gui::IsItemHovered())
 		{
			if(input.isMouseButtonPressed(sf::Mouse::Right) || input.isMouseButtonPressed(sf::Mouse::Middle))
			{
				gui::FocusWindow(gui::GetCurrentWindow());
				window.setMouseCursorVisible(false);

			}

			handleCameraMovement();	
 		}
		if (input.isMouseButtonReleased(sf::Mouse::Right) || input.isMouseButtonReleased(sf::Mouse::Middle))
		{
			window.setMouseCursorVisible(true);
		}
		
 	}
	
	gui::EndDock();
}
