#include "EditorWindow.h"
#include "EditorApp.h"
#include "FileDialog/FileDialog.h"

#include "Runtime/Ecs/Systems/TransformSystem.h"
#include "Runtime/Ecs/Components/ModelComponent.h"
#include "Runtime/Ecs/Components/TransformComponent.h"
#include "Runtime/Ecs/Components/CameraComponent.h"
#include "Runtime/Ecs/World.h"
#include "Runtime/System/FileSystem.h"
#include "Runtime/System/Timer.h"
#include "Runtime/Rendering/RenderSurface.h"
#include "Runtime/Assets/AssetManager.h"

#include "Core/logging/logging.h"

std::vector<ecs::Entity> gatherSceneData()
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& world = app.getWorld();
	auto& editState = app.getEditState();
	auto transformSystem = world.systems.system<TransformSystem>();

	auto roots = transformSystem->getRoots();
	auto editorCamera = editState.camera;
	std::vector<ecs::Entity> entities;
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
	auto& app = Singleton<EditorApp>::getInstance();
	auto& manager = app.getAssetManager();
	auto& world = app.getWorld();

	{
		auto object = world.entities.create();
		object.setName("MainCamera");
		object.assign<TransformComponent>().lock()
			->setLocalPosition({ 0.0f, 2.0f, -5.0f });
		object.assign<CameraComponent>();
	}
	{
		auto object = world.entities.create();
		object.setName("platform");
		object.assign<TransformComponent>().lock()
			->setLocalScale({ 10.0f, 0.1f, 10.0f });

		Model model;
		manager.load<Mesh>("data://meshes/platform", false)
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
		auto object = world.entities.create();
		object.setName("object");
		object.assign<TransformComponent>();

		Model model;
		manager.load<Mesh>("data://meshes/bunny", false)
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
	auto& app = Singleton<EditorApp>::getInstance();
	auto& world = app.getWorld();
	auto& editState = app.getEditState();
	ecs::Entity outData = world.entities.create();
	outData.setName("EditorCamera");
	outData.assign<TransformComponent>().lock()
		->setLocalPosition({ 0.0f, 2.0f, -5.0f });
	outData.assign<CameraComponent>();
	
	editState.camera = outData;
}


auto createNewScene()
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& world = app.getWorld();
	auto& editState = app.getEditState();
	world.reset();
	loadEditorCamera();
	defaultScene();
	editState.scene.clear();
}

auto openScene()
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& world = app.getWorld();
	auto& editState = app.getEditState();
	std::string path;
	if (openFileDialog("scene", fs::resolveFileLocation("data://scenes/"), path))
	{
		world.reset();
		loadEditorCamera();

		Timer timer;
		std::vector<Entity> outData;
		if (world.loadData(path, outData))
		{
			auto time = timer.getTime(true);
			std::string log_msg = "Scene loading time : " + std::to_string(time);
			logging::get("Log")->info(log_msg.c_str());
			editState.scene = path;
		}
	}
}

auto saveScene()
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& world = app.getWorld();
	auto& editState = app.getEditState();
	const auto& path = editState.scene;
	if (path != "")
	{
		Timer timer;
		std::vector<ecs::Entity> entities = gatherSceneData();
		world.saveData(path, entities);
		auto time = timer.getTime(true);
		std::string log_msg = "Scene saving time : " + std::to_string(time);
		logging::get("Log")->info(log_msg.c_str());
	}
}

void saveSceneAs()
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& world = app.getWorld();
	auto& editState = app.getEditState();

	if (world.entities.size() == 0)
		return;

	std::string path;
	if (saveFileDialog("scene", fs::resolveFileLocation("data://scenes/"), path))
	{
		editState.scene = path;		
		saveScene();	
	}
}


MainEditorWindow::MainEditorWindow()
{
	maximize();

	// Force id init order
	TransformComponent::getId();
	CameraComponent::getId();
	ModelComponent::getId();
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
	auto& app = Singleton<EditorApp>::getInstance();
	auto& input = app.getInput();
	auto& world = app.getWorld();
	auto& editState = app.getEditState();

	if (input.isKeyDown(sf::Keyboard::LControl))
	{
		if (input.isKeyDown(sf::Keyboard::LShift))
		{
			if (input.isKeyPressed(sf::Keyboard::S))
			{
				saveSceneAs();
			}
		}
		else if (input.isKeyPressed(sf::Keyboard::S))
		{
			saveScene();
		}

		if (input.isKeyPressed(sf::Keyboard::O))
		{
			openScene();
		}

		if (input.isKeyPressed(sf::Keyboard::N))
		{
			createNewScene();
		}
	}

	if (gui::BeginMenu("File"))
	{
		if (gui::MenuItem("New Scene", "Ctrl+N", false, editState.project != ""))
		{
			createNewScene();
		}
		if (gui::MenuItem("Open Scene", "Ctrl+O", false, editState.project != ""))
		{
			openScene();
		}
		if (gui::MenuItem("Open Project Manager", "Ctrl+P"))
		{
			app.openProjectManager();
		}

		if (gui::MenuItem("Save", "Ctrl+S", false, editState.scene != "" && editState.project != ""))
		{
			saveScene();
		}
		if (gui::MenuItem("Save As..", "Ctrl+Shift+S", false, world.entities.size() > 0 && editState.project != ""))
		{
			saveSceneAs();
		}
		gui::Separator();

		if (gui::MenuItem("Quit", "Alt+F4"))
		{
			app.quit();
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
	auto& app = Singleton<EditorApp>::getInstance();
	auto& editState = app.getEditState();
	auto& icons = editState.icons;

	float width = gui::GetContentRegionAvailWidth();
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
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["grid"].get(), "Show Grid", editState.showGrid))
	{
		editState.showGrid = !editState.showGrid;
	}
	gui::SameLine(0.0f);
	if (gui::ToolbarButton(icons["wireframe"].get(), "Wireframe Selection", editState.wireframeSelection))
	{
		editState.wireframeSelection = !editState.wireframeSelection;
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
	auto& app = Singleton<EditorApp>::getInstance();

	ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoSavedSettings;
	
	gui::BeginGroup();
	{
		
		if (gui::BeginChild("###projects_content", ImVec2(gui::GetContentRegionAvail().x / 1.3f, gui::GetContentRegionAvail().y), false, flags))
		{
			gui::Text("Recent Projects");
			gui::Separator();
			//for testing purposes
			for (auto& project : std::array<std::string, 3>{ "my/recent/project/name1", "my/recent/project/name2", "my/recent/project/name3" })
			{
				if (gui::Selectable(project.c_str()))
				{
					//app.openProject(project);
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
			if (openFolderDialog("", fs::resolveFileLocation("engine://"), path))
			{
				app.createProject(path);
				setMain(false);
				close();
			}
		}

		if (gui::Button("OPEN OTHER"))
		{
			std::string path;
			if (openFolderDialog("", fs::resolveFileLocation("engine://"), path))
			{
				app.openProject(path);
				close();
			}
		}
	}
	gui::EndGroup();
	
	//Call grandparent directly to skip dockspace update
	RenderWindow::frameRender();
}