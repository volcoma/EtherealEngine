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

template<typename T>
void watchAssets(const std::string& toLowerKey, bool reloadAsync)
{
	auto& app = Singleton<Application>::getInstance();
	auto& manager = app.getAssetManager();
	auto storage = manager.getStorage<T>();

	std::string absoluteKey = fs::resolveFileLocation(toLowerKey);
	std::string dir = fs::getDirectoryName(absoluteKey);
	dir = string_utils::replace(dir, '\\', '/');
	dir = string_utils::toLower(dir);
	static const std::string ext = "*.asset";
	const std::string watchDir = dir + storage->subdir + ext;

	wd::watch(watchDir, [&app, &manager, toLowerKey, reloadAsync](const std::vector<fs::path>& paths)
	{
		for (auto& p : paths)
		{
			auto& pool = app.getThreadPool();
			auto relativeKey = toLowerKey + p.filename().replace_extension().string();

			if (!fs::exists(p))
			{
				//removed
				pool.enqueue_with_callback([]() {}, [reloadAsync, relativeKey, &manager]()
				{
					manager.clearAsset<T>(relativeKey);
				});
				
			}
			else
			{
				//created or modified
				if (fs::is_regular_file(p))
				{			
					pool.enqueue_with_callback([]() {}, [reloadAsync, relativeKey, &manager]()
					{
						manager.load<T>(relativeKey, reloadAsync, true);
					});

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
	auto logger = logging::get("Log");
	auto consoleLog = std::make_shared<ConsoleLog>();
	logger->add_sink(consoleLog);

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
	console->initialize("Console", true, ImVec2(200.0f, 200.0f), [consoleLog](ImVec2 area)
	{
		Docks::renderConsole(area, *consoleLog.get());
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

bool EditorApp::initInstance(const std::string& rootDataDir, const std::string& commandLine)
{
	fs::addPathProtocol("engine", rootDataDir + "../../");
	fs::addPathProtocol("editor", rootDataDir + "../../Editor_Data/");

	return Application::initInstance(rootDataDir, commandLine);
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
	if (!initUI()) { shutDown(); return false; }

	if (!initDocks()) { shutDown(); return false; }

	if (!initEditState()) { shutDown(); return false; }
	
	return Application::initApplication();
}

bool EditorApp::shutDown()
{
	mDocks.clear();
	
	gui::shutdown();

	mEditState.clear();

	return Application::shutDown();
}

void EditorApp::createProject(const std::string& projectDir)
{
	auto projName = fs::getFileName(projectDir);
	auto projFile = fs::resolveFileLocation(projectDir + "/" + projName + std::string(".projinfo"));

	fs::addPathProtocol("app", projectDir);
	fs::ensurePath(projectDir + "/", true);
	fs::ensurePath(projectDir + "/data/", true);
	fs::ensurePath(projectDir + "/data/shaders/", true);
	fs::ensurePath(projectDir + "/data/textures/", true);
	fs::ensurePath(projectDir + "/data/materials/", true);
	fs::ensurePath(projectDir + "/data/meshes/", true);
	fs::ensurePath(projectDir + "/data/scenes/", true);
	fs::ensurePath(projectDir + "/data/shaders/runtime/", true);
	fs::ensurePath(projectDir + "/data/textures/runtime/", true);
	fs::ensurePath(projectDir + "/data/materials/runtime/", true);
	fs::ensurePath(projectDir + "/data/meshes/runtime/", true);
	fs::ensurePath(projectDir + "/data/scenes/", true);
	fs::ensurePath(projectDir + "/settings/", true);
	fs::createFile(projFile);
	openProject(projFile);
}

void EditorApp::openProject(const std::string& projectDir)
{
	fs::addPathProtocol("app", fs::getDirectoryName(projectDir));
	fs::addPathProtocol("data", fs::resolveFileLocation("app://data/"));
	wd::unwatchAll();
	watchAssets<Texture>("data://textures/", true);
	watchAssets<Mesh>("data://meshes/", true);
	watchAssets<Material>("data://materials/", false);

	auto& world = getWorld();
	world.reset();
	auto& editState = getEditState();
	editState.unselect();
	editState.scene.clear();
	auto& manager = getAssetManager();
	manager.clear();
	auto projName = fs::getFileName(projectDir);
	editState.project = projName;

	
}

ConsoleLog::ConsoleLog()
{
	InputBuf.resize(64, 0);
	InputBuf.shrink_to_fit();
}

void ConsoleLog::_sink_it(const logging::details::log_msg& msg)
{
	Items.push_back({ msg.formatted.c_str(), msg.level });
	if (Items.size() > MaxSize)
		Items.pop_back();
	flush();
	ScrollToBottom = true;
}

void ConsoleLog::flush()
{

}

ConsoleLog::ItemContainer ConsoleLog::getItems()
{
	ItemContainer itemsCopy;
	{
		std::lock_guard<std::mutex> lock(_mutex);
		itemsCopy = Items;
	}
	return itemsCopy;
}

void ConsoleLog::clearLog()
{
	Items.clear();
	ScrollToBottom = true;
}

void ConsoleLog::clearInput()
{
	std::fill(std::begin(InputBuf), std::end(InputBuf), 0);
}
