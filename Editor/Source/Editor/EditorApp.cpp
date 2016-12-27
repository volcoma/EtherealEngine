#include "EditorApp.h"
#include "EditorWindow.h"
#include "Interface/Docks/Docks.h"
#include "Systems/PickingSystem.h"
#include "Systems/DebugDrawSystem.h"
#include "Runtime/System/MessageBox.h"
#include "Runtime/System/FileSystem.h"
#include "Runtime/Assets/AssetManager.h"
#include "Runtime/System/Watchdog.h"
#include "Runtime/Rendering/Material.h"
#include "Runtime/Rendering/Texture.h"
#include "Runtime/Rendering/Mesh.h"
#include "Runtime/Rendering/Shader.h"
#include "Runtime/Threading/ThreadPool.h"
#include "Runtime/Ecs/Prefab.h"
#include "Console/ConsoleLog.h"
#include "Assets/AssetCompiler.h"
template<typename T>
void watchAssets(const fs::path& protocol, bool reloadAsync)
{
	auto& app = Singleton<Application>::getInstance();
	auto& manager = app.getAssetManager();
	auto storage = manager.getStorage<T>();

	const fs::path dir = fs::resolve_protocol(protocol);
	static const std::string ext = "*.asset";
	fs::path watchDir = dir / storage->subdir;
	fs::create_directory(watchDir);
	watchDir /= storage->platform;
	fs::create_directory(watchDir);
	watchDir /= ext;

	wd::watch(watchDir, true, [&app, &manager, protocol, reloadAsync](const std::vector<wd::Entry>& entries)
	{
		for (auto& entry : entries)
		{
			auto& pool = app.getThreadPool();
			auto p = entry.path;
			auto key = (protocol / p.filename().replace_extension()).generic_string();

			if (entry.state == wd::Entry::Removed)
			{
				//removed
				pool.enqueue_with_callback([]() {}, [reloadAsync, key, &manager]()
				{
					manager.clearAsset<T>(key);
				});
				
			}
			else
			{
				//created or modified
				if (fs::is_regular_file(p, std::error_code{}))
				{			
					pool.enqueue_with_callback([]() {}, [reloadAsync, key, &manager]()
					{
						manager.load<T>(key, reloadAsync, true);
					});

				}
			}
		}
	});
}

void watchRawShaders(const fs::path& protocol, bool reloadAsync)
{
	auto& app = Singleton<Application>::getInstance();

	const fs::path dir = fs::resolve_protocol(protocol);
	static const std::string ext = "*.sc";
	const fs::path watchDir = dir / ext;

	wd::watch(watchDir, false, [&app, reloadAsync](const std::vector<wd::Entry>& entries)
	{
		for (auto& entry : entries)
		{
			const auto& p = entry.path;
			if(string_utils::endsWith(p.string(), "def.sc"))
				continue;

			auto& pool = app.getThreadPool();
	
			if (entry.state == wd::Entry::Removed)
			{
				//removed
			}
			else
			{
				//created or modified
				if (fs::is_regular_file(p, std::error_code{}))
				{
					auto callback = []() {};
					pool.enqueue_with_callback([p]()
					{
						ShaderCompiler compiler;
						compiler.compile(p);
					}, callback);

				}
			}
		}
	});
}



bool EditorApp::initUI()
{
	auto logger = logging::get("Log");

	if (!gui::init())
	{
		// Write debug info
		logger->critical() << "Failed to initialize ui manager.";

		// Failure to initialize!
		misc::messageBox(
			"Failed to initialize UI. The application must now exit.",
			"Fatal Error",
			misc::Style::Error,
			misc::Buttons::OK
		);

		return false;
	}

	return true;
}


bool EditorApp::initDocks()
{
	mDocks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	mDocks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	mDocks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	mDocks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	mDocks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	mDocks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	mDocks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	mDocks.emplace_back(std::make_unique<ImGuiDock::Dock>());
	mDocks.emplace_back(std::make_unique<ImGuiDock::Dock>());

	auto& scene = mDocks[0];
	scene->initialize("Scene", true, ImVec2(200.0f, 200.0f), &Docks::renderScene);

	auto& game = mDocks[1];
	game->initialize("Game", true, ImVec2(200.0f, 200.0f), &Docks::renderGame);

	auto& hierarchy = mDocks[2];
	hierarchy->initialize("Hierarchy", true, ImVec2(300.0f, 200.0f), &Docks::renderHierarcy);

	auto& inspector = mDocks[3];
	inspector->initialize("Inspector", true, ImVec2(300.0f, 200.0f), &Docks::renderInspector);

	auto& assets = mDocks[4];
	assets->initialize("Assets", true, ImVec2(200.0f, 200.0f), &Docks::renderAssets);

	auto& console = mDocks[5];
	console->initialize("Console", true, ImVec2(200.0f, 200.0f), [this](ImVec2 area)
	{
		Docks::renderConsole(area, *mConsoleLog.get());
	});

	auto& style = mDocks[6];
	style->initialize("Style", true, ImVec2(300.0f, 200.0f), &Docks::renderStyle);


	auto& projects = mDocks[7];
	projects->initialize("Projects", true, ImVec2(200.0f, 200.0f), &Docks::renderProject);

	auto& gettingStarted = mDocks[8];
	gettingStarted->initialize("Getting Started", true, ImVec2(200.0f, 200.0f), &Docks::renderGettingStarted);


	{
		auto& window = static_cast<GuiWindow&>(getMainWindow());
		auto& dockspace = window.getDockspace();
		dockspace.dock(scene.get(), ImGuiDock::DockSlot::None, 200, true);
		dockspace.dockWith(game.get(), scene.get(), ImGuiDock::DockSlot::Tab, 300, false);
		dockspace.dockWith(inspector.get(), scene.get(), ImGuiDock::DockSlot::Right, 300, true);
		dockspace.dockWith(hierarchy.get(), scene.get(), ImGuiDock::DockSlot::Left, 300, true);
		dockspace.dock(assets.get(), ImGuiDock::DockSlot::Bottom, 200, true);
		dockspace.dockWith(console.get(), assets.get(), ImGuiDock::DockSlot::Tab, 200, true);
		dockspace.dockWith(style.get(), assets.get(), ImGuiDock::DockSlot::Right, 300, true);

	}
	
	return true;
}

bool EditorApp::initEditState()
{
	auto& manager = getAssetManager();
	mEditState.loadIcons(manager);
	mEditState.loadOptions();
	return true;
}

bool EditorApp::frameBegin(bool runSimulation /*= true*/)
{
	gui::begin();

	return Application::frameBegin(runSimulation);
}

void EditorApp::frameWindowEnd(RenderWindow& window)
{
	auto& editState = getEditState();
	editState.frameEnd();

	Application::frameWindowEnd(window);
}

std::shared_ptr<RenderWindow> EditorApp::createMainWindow()
{
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	desktop.width = 1280;
	desktop.height = 720;
	return std::make_shared<MainEditorWindow>(desktop, "Editor v1.0", sf::Style::Default);
}

bool EditorApp::initInstance(const std::string& commandLine)
{
	fs::create_directory(fs::resolve_protocol("editor_data://Config"));
	return Application::initInstance(commandLine);
}

bool EditorApp::initSystems()
{
	if (!Application::initSystems())
		return false;

	auto& world = getWorld();
 	world.systems.add<PickingSystem>();
 	world.systems.add<DebugDrawSystem>();

	return true;
}

bool EditorApp::initApplication()
{
	auto logger = logging::get("Log");
	logger->add_sink(mConsoleLog);

	std::function<void()> logVersion = [logger]()
	{
		logger->info() << "Version 1.0";
	};
	mConsoleLog->registerCommand(
		"version",
		"Returns the current version of the Editor.",
		{ },
		{ },
		logVersion
	);

	if (!initUI()) { shutDown(); return false; }

	if (!initDocks()) { shutDown(); return false; }

	if (!initEditState()) { shutDown(); return false; }

	openProjectManager();

	return Application::initApplication();
}

bool EditorApp::shutDown()
{
	mDocks.clear();
	
	gui::shutdown();

	mEditState.clear();

	return Application::shutDown();
}

void EditorApp::createProject(const fs::path& projectPath)
{
	fs::add_path_protocol("app:", projectPath);
	fs::create_directory(fs::resolve_protocol("app://data"));
	fs::create_directory(fs::resolve_protocol("app://data/shaders"));
	fs::create_directory(fs::resolve_protocol("app://data/textures"));
	fs::create_directory(fs::resolve_protocol("app://data/materials"));
	fs::create_directory(fs::resolve_protocol("app://data/meshes"));
	fs::create_directory(fs::resolve_protocol("app://data/scenes"));
	fs::create_directory(fs::resolve_protocol("app://data/shaders/runtime"));
	fs::create_directory(fs::resolve_protocol("app://data/textures/runtime"));
	fs::create_directory(fs::resolve_protocol("app://data/meshes/runtime"));
	fs::create_directory(fs::resolve_protocol("app://data/prefabs"));
	fs::create_directory(fs::resolve_protocol("app://data/scenes"));
	fs::create_directory(fs::resolve_protocol("app://settings"));

	fs::copy(fs::resolve_protocol("engine_data://meshes/runtime"), fs::resolve_protocol("app://data/meshes/runtime"), std::error_code{});
	
	openProject(projectPath);
}

void EditorApp::openProject(const fs::path& projectPath)
{
	if (!fs::exists(projectPath, std::error_code{}))
	{
		auto logger = logging::get("Log");
		logger->error().write("Project directory doesn't exist {0}", projectPath.string());
		return;
	}
	fs::add_path_protocol("app:", projectPath);
	fs::add_path_protocol("data:", fs::resolve_protocol("app://data"));
	wd::unwatchAll();
	watchAssets<Texture>("data://textures", true);
	watchAssets<Texture>("editor_data://icons", true);
	watchAssets<Mesh>("data://meshes", true);
	watchAssets<Prefab>("data://prefabs", true);
	watchAssets<Material>("data://materials", false);
	watchAssets<Shader>("engine_data://shaders", true);
	watchRawShaders("engine_data://shaders", true);

	watchAssets<Shader>("editor_data://shaders", true);
	watchRawShaders("editor_data://shaders", true);
	
	auto& world = getWorld();
	world.reset();
	auto& editState = getEditState();
	editState.unselect();
	editState.scene.clear();
	auto& manager = getAssetManager();
	manager.clear("data://");
	auto projName = projectPath.filename();
	editState.project = projName.string();
	auto& rp = editState.options.recentProjects;
	if (std::find(std::begin(rp), std::end(rp), projectPath.generic_string()) == std::end(rp))
	{
		rp.push_back(projectPath.generic_string());
		editState.saveOptions();
	}
}

void EditorApp::openProjectManager()
{
	getMainWindow().setVisible(false);
	sf::VideoMode desktop = sf::VideoMode::getDesktopMode();
	desktop.width = 500;
	desktop.height = 300;;
	auto wnd = std::make_shared<ProjectManagerWindow>(desktop, "Project Manager", sf::Style::Titlebar);
	registerWindow(wnd);
 	wnd->onClosed.addListener([this](RenderWindow& window)
 	{
		getMainWindow().setVisible(true);
 	});
}
