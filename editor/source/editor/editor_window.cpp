#include "editor_window.h"
#include "edit_state.h"
#include "project.h"

#include "filedialog/filedialog.h"
#include "runtime/ecs/systems/transform_system.h"
#include "runtime/ecs/components/model_component.h"
#include "runtime/ecs/components/transform_component.h"
#include "runtime/ecs/components/camera_component.h"
#include "runtime/ecs/components/light_component.h"
#include "runtime/ecs/utils.h"
#include "runtime/system/filesystem.h"
#include "runtime/rendering/render_pass.h"
#include "runtime/assets/asset_manager.h"
#include "runtime/input/input.h"
#include "core/logging/logging.h"

std::vector<core::Entity> gatherSceneData()
{
	auto es = core::get_subsystem<EditState>();
	auto ts = core::get_subsystem<TransformSystem>();
	const auto& roots = ts->get_roots();
	auto editorCamera = es->camera;
	std::vector<core::Entity> entities;
	for (auto root : roots)
	{
		auto entity = root.lock()->getEntity();
		if (entity != editorCamera)
			entities.push_back(entity);
	}

	return entities;
}


void defaultScene()
{
	auto am = core::get_subsystem<AssetManager>();
	auto ecs = core::get_subsystem<core::EntityComponentSystem>();

	{
		auto object = ecs->create();
		object.set_name("Main Camera");
		object.assign<TransformComponent>().lock()
			->setLocalPosition({ 0.0f, 2.0f, -5.0f });
		object.assign<CameraComponent>();
	}
	{
		auto object = ecs->create();
		object.set_name("Light");
		object.assign<TransformComponent>().lock()
			->setLocalPosition({ 1.0f, 6.0f, -3.0f })
			.rotateLocal(50.0f, -30.0f, 0.0f);
		object.assign<LightComponent>().lock()
			->getLight().lightType = LightType::Directional;
	}
	{
		auto object = ecs->create();
		object.set_name("platform");
		object.assign<TransformComponent>().lock()
			->setLocalScale({ 10.0f, 0.1f, 10.0f });

		Model model;
		am->load<Mesh>("data://meshes/platform", false)
			.then([&model](auto asset)
		{
			model.setLod(asset, 0);
		});

		//Add component and configure it.
		object.assign<ModelComponent>().lock()
			->setCastShadow(true)
			.setCastReflelction(false)
			.setModel(model);
	}
	{
		auto object = ecs->create();
		object.set_name("object");
		object.assign<TransformComponent>();

		Model model;
		am->load<Mesh>("data://meshes/bunny", false)
			.then([&model](auto asset)
		{
			model.setLod(asset, 0);
		});

		//Add component and configure it.
		object.assign<ModelComponent>().lock()
			->setCastShadow(true)
			.setCastReflelction(false)
			.setModel(model);
	}
}

void loadEditorCamera()
{
	auto es = core::get_subsystem<EditState>();
	auto ecs = core::get_subsystem<core::EntityComponentSystem>();
	core::Entity outData = ecs->create();
	outData.set_name("Editor Camera");
	outData.assign<TransformComponent>().lock()
		->setLocalPosition({ 0.0f, 2.0f, -5.0f });
	outData.assign<CameraComponent>();
	
	es->camera = outData;
}


auto createNewScene()
{
	auto es = core::get_subsystem<EditState>();
	auto ecs = core::get_subsystem<core::EntityComponentSystem>();
	ecs->dispose();
	loadEditorCamera();
	defaultScene();
	es->scene.clear();
}

auto openScene()
{
	auto es = core::get_subsystem<EditState>();
	auto ecs = core::get_subsystem<core::EntityComponentSystem>();
	std::string path;
	if (open_file_dialog("scene", fs::resolve_protocol("data://scenes").string(), path))
	{
		ecs->dispose();
		loadEditorCamera();

		std::vector<core::Entity> outData;
		if (ecs::utils::loadData(path, outData))
		{
			es->scene = path;
		}
	}
}

auto saveScene()
{
	auto es = core::get_subsystem<EditState>();
	const auto& path = es->scene;
	if (path != "")
	{
		std::vector<core::Entity> entities = gatherSceneData();
		ecs::utils::saveData(path, entities);
	}
}

void saveSceneAs()
{
	auto es = core::get_subsystem<EditState>();

	std::string path;
	if (save_file_dialog("scene", fs::resolve_protocol("data://scenes").string(), path))
	{
		es->scene = path;		
		saveScene();	
	}
}


MainEditorWindow::MainEditorWindow()
{
	maximize();
}

MainEditorWindow::MainEditorWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/)
	:GuiWindow(mode, title, style)
{
	maximize();
}

MainEditorWindow::~MainEditorWindow()
{
}

void MainEditorWindow::frameRender()
{
	if (gui::BeginMainMenuBar())
	{
		onMenuBar();

		float offset = gui::GetWindowHeight();
		gui::EndMainMenuBar();
		gui::SetCursorPosY(gui::GetCursorPosY() + offset);
	}

	
	onToolbar();
	GuiWindow::frameRender();
}

void MainEditorWindow::onMenuBar()
{
	auto es = core::get_subsystem<EditState>();
	auto input = core::get_subsystem<Input>();
	auto pm = core::get_subsystem<ProjectManager>();
	const auto& current_project = pm->get_current_project();

	if (input->is_key_down(sf::Keyboard::LControl))
	{
		if (input->is_key_down(sf::Keyboard::LShift))
		{
			if (input->is_key_pressed(sf::Keyboard::S))
			{
				saveSceneAs();
			}
		}
		else if (input->is_key_pressed(sf::Keyboard::S))
		{
			saveScene();
		}

		if (input->is_key_pressed(sf::Keyboard::O))
		{
			openScene();
		}

		if (input->is_key_pressed(sf::Keyboard::N))
		{
			createNewScene();
		}
	}

	if (gui::BeginMenu("File"))
	{
		if (gui::MenuItem("New Scene", "Ctrl+N", false, current_project != ""))
		{
			createNewScene();
		}
		if (gui::MenuItem("Open Scene", "Ctrl+O", false, current_project != ""))
		{
			openScene();
		}
		if (gui::MenuItem("Open Project Manager", "Ctrl+P"))
		{
			pm->open();
		}

		if (gui::MenuItem("Save", "Ctrl+S", false, es->scene != "" && current_project != ""))
		{
			saveScene();
		}
		auto ecs = core::get_subsystem<core::EntityComponentSystem>();

		if (gui::MenuItem("Save As..", "Ctrl+Shift+S", false, ecs->size() > 0 && current_project != ""))
		{
			saveSceneAs();
		}
		gui::Separator();

		if (gui::MenuItem("Quit", "Alt+F4"))
		{
			//app.quit();
		}

		gui::EndMenu();
	}
	if (gui::BeginMenu("Edit"))
	{
		if (gui::MenuItem("Undo", "CTRL+Z"))
		{

		}
		if (gui::MenuItem("Redo", "CTRL+Y", false, false))
		{

		}
		gui::Separator();
		if (gui::MenuItem("Cut", "CTRL+X"))
		{

		}
		if (gui::MenuItem("Copy", "CTRL+C"))
		{

		}
		if (gui::MenuItem("Paste", "CTRL+V"))
		{

		}
		gui::EndMenu();
	}

}

void MainEditorWindow::onToolbar()
{
	auto es = core::get_subsystem<EditState>();
	auto& icons = es->icons;

	float width = gui::GetContentRegionAvailWidth();
	if (gui::ToolbarButton(icons["translate"].get(), "Translate", es->operation == ImGuizmo::OPERATION::TRANSLATE))
	{
		es->operation = ImGuizmo::OPERATION::TRANSLATE;
	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["rotate"].get(), "Rotate", es->operation == ImGuizmo::OPERATION::ROTATE))
	{
		es->operation = ImGuizmo::OPERATION::ROTATE;
	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["scale"].get(), "Scale", es->operation == ImGuizmo::OPERATION::SCALE))
	{
		es->operation = ImGuizmo::OPERATION::SCALE;
		es->mode = ImGuizmo::MODE::LOCAL;
	}
	gui::SameLine(0.0f, 50.0f);

	if (gui::ToolbarButton(icons["local"].get(), "Local Coordinate System", es->mode == ImGuizmo::MODE::LOCAL))
	{
		es->mode = ImGuizmo::MODE::LOCAL;
	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["global"].get(), "Global Coordinate System", es->mode == ImGuizmo::MODE::WORLD, es->operation != ImGuizmo::OPERATION::SCALE))
	{
		es->mode = ImGuizmo::MODE::WORLD;
	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["grid"].get(), "Show Grid", es->showGrid))
	{
		es->showGrid = !es->showGrid;
	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["wireframe"].get(), "Wireframe Selection", es->wireframeSelection))
	{
		es->wireframeSelection = !es->wireframeSelection;
	}

	gui::SameLine(width / 2.0f - 36.0f);
	if (gui::ToolbarButton(icons["play"].get(), "Play", false))
	{

	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["pause"].get(), "Pause", false))
	{

	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["next"].get(), "Step", false))
	{

	}
}

ProjectManagerWindow::ProjectManagerWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/)
	: GuiWindow(mode, title, style)
{

}

void ProjectManagerWindow::frameRender()
{
	auto es = core::get_subsystem<EditState>();
	auto pm = core::get_subsystem<ProjectManager>();

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings;

	gui::Text("Recent Projects");
	gui::Separator();
	gui::BeginGroup();
	{
		if (gui::BeginChild("###projects_content", ImVec2(gui::GetContentRegionAvail().x / 1.3f, gui::GetContentRegionAvail().y), false, flags))
		{
			
			const auto& rencent_projects = pm->get_recent_projects();
			for (auto& path : rencent_projects)
			{
				if (gui::Selectable(path.c_str()))
				{
					pm->open_project(path);
					setMain(false);
					close();
				}
			}
			gui::EndChild();
		}
	}
	gui::EndGroup();

	gui::SameLine();

	gui::BeginGroup();
	{
		if (gui::Button("NEW PROJECT"))
		{
			std::string path;
			if (open_folder_dialog("", fs::resolve_protocol("engine://").string(), path))
			{
				pm->create_project(path);
				setMain(false);
				close();
			}
		}

		if (gui::Button("OPEN OTHER"))
		{
			std::string path;
			if (open_folder_dialog("", fs::resolve_protocol("engine://").string(), path))
			{
				pm->open_project(path);
				setMain(false);
				close();
			}
		}
	}
	gui::EndGroup();
	
	//Call grandparent directly to skip dockspace update
	RenderWindow::frameRender();
}