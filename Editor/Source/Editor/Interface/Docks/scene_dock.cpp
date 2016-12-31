#include "docks.h"
#include "../../edit_state.h"
#include "runtime/system/engine.h"
#include "runtime/input/input.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/rendering/camera.h"
#include "runtime/rendering/render_window.h"

namespace Docks
{
	static bool showGBuffer = false;

	void showStatistics(const unsigned int frameRate)
	{
		ImVec2 pos = gui::GetCursorScreenPos();
		gui::SetNextWindowPos(pos);
		gui::SetNextWindowCollapsed(true, ImGuiSetCond_FirstUseEver);
		gui::Begin("Statistics", nullptr,
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_AlwaysAutoResize);
		
		gui::Text("FPS  : %u", frameRate);
		gui::Separator();
		gui::Text("MSPF : %.3f ms ", 1000.0f / float(frameRate));
		gui::Separator();


		static bool renderStats = false;
		if (gui::Checkbox("Render Stats", &renderStats))
		{
			if (renderStats)
				gfx::setDebug(BGFX_DEBUG_STATS);
			else
				gfx::setDebug(BGFX_DEBUG_NONE);

		}
		gui::Separator();
		gui::Checkbox("Show G-Buffer", &showGBuffer);
		
//		if (renderStats)
//		{
// 			gui::Text("Graphics");
// 			auto stats = gfx::getStats();
// 
// 			double freq = double(stats->cpuTimerFreq);
// 			double toMs = 1000.0 / freq;
// 			gui::Text("CPU Time Begin : %u", stats->cpuTimeBegin);
// 			gui::Text("CPU Time End : %u", stats->cpuTimeEnd);
// 			gui::Text("CPU Time Freq : %u", stats->cpuTimerFreq);
// 			gui::Text("GPU Time Begin : %u", stats->gpuTimeBegin);
// 			gui::Text("GPU Time End : %u", stats->gpuTimeEnd);
// 			gui::Text("GPU Time Freq : %u", stats->gpuTimerFreq);
// 			gui::Text("Wait Render : %f", stats->waitRender*toMs);
// 			gui::Text("Wait Submit : %f", stats->waitSubmit*toMs);
//			gui::Separator();
//		}
		gui::End();

	}

	void drawSelectedCamera(const ImVec2& size)
	{
		auto is = core::get_subsystem<InputSystem>();
		auto es = core::get_subsystem<EditState>();
		auto& input = is->get_context();
		auto& selected = es->selectionData.object;
		auto& editorCamera = es->camera;

		if (selected.is_type<core::Entity>())
		{
			auto sel = selected.get_value<core::Entity>();

			if (sel && (editorCamera != sel) && sel.has_component<CameraComponent>())
			{
				const auto selectedCamera = sel.component<CameraComponent>().lock();
				const auto& camera = selectedCamera->getCamera();
				const auto surface = selectedCamera->getOutputBuffer();
				const auto viewSize = camera.getViewportSize();

				float factor = std::min(size.x / float(viewSize.width), size.y / float(viewSize.height)) / 4.0f;
				ImVec2 bounds(viewSize.width * factor, viewSize.height * factor);
				auto p = gui::GetWindowPos();
				p.x += size.x - bounds.x - 20.0f;
				p.y += size.y - bounds.y - 40.0f;
				gui::SetNextWindowPos(p);
				if (gui::Begin(
					"Camera Preview",
					nullptr,
					ImGuiWindowFlags_NoFocusOnAppearing |
					ImGuiWindowFlags_ShowBorders |
					ImGuiWindowFlags_NoCollapse |
					ImGuiWindowFlags_NoMove |
					ImGuiWindowFlags_NoResize |
					ImGuiWindowFlags_AlwaysAutoResize))
				{
					gui::Image(surface, bounds);
				}
				gui::End();

				if (input.isKeyPressed(sf::Keyboard::F) && sel.has_component<TransformComponent>())
				{
					auto transform = editorCamera.component<TransformComponent>().lock();
					auto transformSelected = sel.component<TransformComponent>().lock();
					transformSelected->setTransform(transform->getTransform());
				}
			}
		}
		
	}


	void manipulationGizmos()
	{
		auto is = core::get_subsystem<InputSystem>();
		auto es = core::get_subsystem<EditState>();
		auto& input = is->get_context();
		auto& selected = es->selectionData.object;
		auto& editorCamera = es->camera;
		auto& operation = es->operation;
		auto& mode = es->mode;

		if (!input.isMouseButtonDown(sf::Mouse::Right) && !gui::IsAnyItemActive() && !ImGuizmo::IsUsing())
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

		if (selected && selected.is_type<core::Entity>())
		{
			auto sel = selected.get_value<core::Entity>();
			if (sel && sel != editorCamera && sel.has_component<TransformComponent>())
			{
				auto p = gui::GetItemRectMin();
				auto s = gui::GetItemRectSize();
				ImGuizmo::SetViewRect(p.x, p.y, s.x, s.y);
				auto cameraComponent = editorCamera.component<CameraComponent>().lock();
				auto transformComponent = sel.component<TransformComponent>().lock();
				transformComponent->resolveTransform(true);
				auto transform = transformComponent->getTransform();
				math::transform_t delta;
				math::transform_t inputTransform = transform;
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
					cameraComponent->getCamera().getView(),
					cameraComponent->getCamera().getProj(),
					operation,
					mode,
					transform,
					nullptr,
					snap);


				transformComponent->setTransform(transform);
			}
		}
	}

	void handleCameraMovement()
	{
		if (!gui::IsWindowFocused())
			return;
		auto is = core::get_subsystem<InputSystem>();
		auto es = core::get_subsystem<EditState>();
		auto engine = core::get_subsystem<runtime::Engine>();

		auto& input = is->get_context();
		auto& editorCamera = es->camera;
		auto dt = engine->get_delta_time().count();

		auto transform = editorCamera.component<TransformComponent>().lock();
		float movementSpeed = 5.0f;
		float rotationSpeed = 0.2f;
		float boostMultiplier = 5.0f;
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

	void renderScene(ImVec2 area)
	{
		auto es = core::get_subsystem<EditState>();
		auto engine = core::get_subsystem<runtime::Engine>();
		//auto& window = app.getWindow();
		auto ecs = core::get_subsystem<core::EntityComponentSystem>();
		auto is = core::get_subsystem<InputSystem>();
		auto& input = is->get_context();
		auto& editorCamera = es->camera;
		auto& selected = es->selectionData.object;
		bool hasEditCamera = editorCamera
			&& editorCamera.has_component<CameraComponent>()
			&& editorCamera.has_component<TransformComponent>();

		showStatistics(engine->get_fps());

		if (!hasEditCamera)
			return;
	
		
		auto size = gui::GetContentRegionAvail();
		auto pos = gui::GetCursorScreenPos();
		drawSelectedCamera(size);

		auto cameraComponent = editorCamera.component<CameraComponent>().lock();
		if (size.x > 0 && size.y > 0)
		{
			cameraComponent->getCamera().setViewportPos({ static_cast<std::uint32_t>(pos.x), static_cast<std::uint32_t>(pos.y) });
			cameraComponent->setViewportSize({ static_cast<std::uint32_t>(size.x), static_cast<std::uint32_t>(size.y) });
			
			const auto surface = cameraComponent->getOutputBuffer();
			gui::Image(surface, size);

			if (gui::IsItemClicked(1) || gui::IsItemClicked(2))
			{
				gui::SetWindowFocus();
				//window.setMouseCursorVisible(false);
			}

			manipulationGizmos();

			handleCameraMovement();
			if (gui::IsWindowFocused())
			{
				ImGui::PushStyleColor(ImGuiCol_Border, gui::GetStyle().Colors[ImGuiCol_Button]);
				ImGui::RenderFrameEx(gui::GetItemRectMin(), gui::GetItemRectMax(), true, 0.0f, 2.0f);
				ImGui::PopStyleColor();

				if (input.isKeyPressed(sf::Keyboard::Delete))
				{
					if (selected && selected.is_type<core::Entity>())
					{
						auto sel = selected.get_value<core::Entity>();
						if (sel != editorCamera)
						{
							sel.destroy();
							es->unselect();
						}
					}
				}
			}


			if (gui::IsMouseReleased(1) || gui::IsMouseReleased(2))
			{
				//window.setMouseCursorVisible(true);
			}

			if (showGBuffer)
			{
				const auto gBufferSurface = cameraComponent->getGBuffer();
				for (std::uint32_t i = 0; i < gBufferSurface->getAttachmentCount(); ++i)
				{
					const auto attachment = gBufferSurface->getAttachment(i).texture;
					gui::Image(attachment, size);

					if (gui::IsItemClicked(1) || gui::IsItemClicked(2))
					{
						gui::SetWindowFocus();
						//window.setMouseCursorVisible(false);
					}
				}
			}

		}
	}

};