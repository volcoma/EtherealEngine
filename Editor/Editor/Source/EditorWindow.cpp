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
#include "Runtime/Rendering/RenderView.h"
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
// 	{
// 		//Create an overwrite material
// 		auto object = world.entities.create();
// 		object.assign<TransformComponent>();
// 
// 		//Add component and configure it.
// // 		object.addComponent<ecs::SphereReflectionProbeComponent>()
// // 			.setRange(256)
// // 			.setAffectMethod(AffectMethod::SkyOnly);
// 	}
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
		manager.load<Mesh>("sys://meshes/platform", false)
			.then([&model](auto asset)
		{
			model.setLod(asset, 0);
		});

		manager.load<Material>("sys://materials/checker", false)
			.then([&model](auto asset)
		{
			model.setMaterial(asset, 0);
		});

		//Add component and configure it.
		object.assign<ModelComponent>().lock()
			->setCastShadow(true)
			.setCastReflelction(false)
			.setModel(model);
	}
	{
		auto tree = world.entities.create();
		tree.setName("tree");
		tree.assign<TransformComponent>().lock()
			->setLocalScale({ 0.25f, 0.25f, 0.25f });

		{
			auto object = world.entities.create();
			object.setName("leafs");
			object.assign<TransformComponent>().lock()
				->setParent(tree.component<TransformComponent>());

			Model model;
			manager.load<Mesh>("sys://meshes/tree_leafs_0", false)
				.then([&model](auto asset)
			{
				model.setLod(asset, 0);
			});
			manager.load<Mesh>("sys://meshes/tree_leafs_1", false)
				.then([&model](auto asset)
			{
				model.setLod(asset, 1);
			});
			manager.load<Mesh>("sys://meshes/tree_leafs_2", false)
				.then([&model](auto asset)
			{
				model.setLod(asset, 2);
			});
			manager.load<Material>("sys://materials/leafs", false)
				.then([&model](auto asset)
			{
				model.setMaterial(asset, 0);
			});

			//Add component and configure it.
			object.assign<ModelComponent>().lock()
				->setCastShadow(true)
				.setCastReflelction(false)
				.setModel(model);
		}
		{
			auto object = world.entities.create();
			object.setName("trunk");
			object.assign<TransformComponent>().lock()
				->setParent(tree.component<TransformComponent>());

			Model model;
			manager.load<Mesh>("sys://meshes/tree_trunk_0", false)
				.then([&model](auto asset)
			{
				model.setLod(asset, 0);
			});
			manager.load<Mesh>("sys://meshes/tree_trunk_1", false)
				.then([&model](auto asset)
			{
				model.setLod(asset, 1);
			});
			manager.load<Mesh>("sys://meshes/tree_trunk_2", false)
				.then([&model](auto asset)
			{
				model.setLod(asset, 2);
			});

			manager.load<Material>("sys://materials/trunk", false)
				.then([&model](auto asset)
			{
				model.setMaterial(asset, 0);
			});


			//Add component and configure it.
			object.assign<ModelComponent>().lock()
				->setCastShadow(true)
				.setCastReflelction(false)
				.setModel(model);
		}
	}
	
// 	{
// 		auto object = world.createEntity();
// 		object.addComponent<ecs::TransformComponent>()
// 			.rotateLocal(45.0f, -45.0f, 0.0f);
// 
// 		//Add component and configure it.
// 		object.addComponent<ecs::DirectionalLightComponent>()
// 			.setColor(ColorValue::White)
// 			.setIntensity(10.0f)
// 			.enableShadows(true);
// 	}
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
	if (openFileDialog("scene", fs::resolveFileLocation("app://data/scenes/"), path))
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
	if (saveFileDialog("scene", fs::resolveFileLocation("app://data/scenes/"), path))
	{
		editState.scene = path;		
		saveScene();	
	}
}


MainEditorWindow::MainEditorWindow()
{
	maximize();
	openProjectManager();

}

MainEditorWindow::MainEditorWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style /*= sf::Style::Default*/)
	:GuiWindow(mode, title, style)
{
	maximize();
	openProjectManager();
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
	onProjectManager();
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
			openProjectManager();
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

bool MainEditorWindow::onProjectManager()
{
	auto& app = Singleton<EditorApp>::getInstance();
	auto& editState = app.getEditState();

	if (mOpenProjectManager)
	{
		gui::OpenPopup("ProjectManager");
		gui::SetNextWindowSize(ImVec2(gui::GetContentRegionAvail().x / 3, gui::GetContentRegionAvail().y / 3));
	}
	
	if (gui::BeginPopupModal("ProjectManager", &mOpenProjectManager, ImGuiWindowFlags_ShowBorders))
	{
		if (gui::Button("NEW PROJECT"))
		{
			std::string path;
			if (saveFileDialog("", fs::resolveFileLocation("engine://"), path))
			{
				app.createProject(path);
				gui::CloseCurrentPopup();
				mOpenProjectManager = false;	
			}
		}

		gui::SameLine();
		if (gui::Button("OPEN OTHER"))
		{
			std::string path;
			if (openFileDialog("projinfo", fs::resolveFileLocation("engine://"), path))
			{
				app.openProject(path);
				gui::CloseCurrentPopup();
				mOpenProjectManager = false;
			}
		}
		gui::Separator();
		auto frame_padding = gui::GetStyle().FramePadding;
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoTitleBar |
			ImGuiWindowFlags_NoMove |
			ImGuiWindowFlags_NoResize |
			ImGuiWindowFlags_NoSavedSettings;
		gui::BeginChild("###projects_content", gui::GetContentRegionAvail(), false, flags);
		{
// 			for (auto& project : editState.projects)
// 			{
// 				if (gui::Selectable(project.c_str()))
// 				{
// 					gui::CloseCurrentPopup();
// 					mOpenProjectManager = false;
// 				}
// 			}


		}
		gui::EndChild();
		gui::EndPopup();
		return true;
	}

	return false;
}
