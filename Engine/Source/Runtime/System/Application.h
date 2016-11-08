#pragma once

#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "Core/internal/assert/assert.h"
#include "Singleton.h"
//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------
class Timer;
class AssetManager;
class ThreadPool;
class InputContext;
class RenderWindow;
struct World;
struct ActionMapper;
//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : Application (Class)
/// <summary>
/// Central application handling class. Initializes the app and handles
/// all core processes.
/// </summary>
//-----------------------------------------------------------------------------
class Application
{
public:

	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : Application () (Constructor)
	/// <summary>
	/// Application Class Constructor
	/// </summary>
	//-----------------------------------------------------------------------------
	Application();

	//-----------------------------------------------------------------------------
	//  Name : ~Application () (Destructor)
	/// <summary>
	/// Application Class Destructor
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~Application();

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : frameAdvance ()
	/// <summary>
	/// Process the next application frame.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool frameAdvance(bool runSimulation = true);

	//-----------------------------------------------------------------------------
	//  Name : setRootDataPath ()
	/// <summary>
	/// Set the path that represents the root from which all data will be loaded
	/// or leave blank to use the process current directory. The information
	/// provided via this method only takes effect during a subsequent call to 
	/// 'initInstance()' and cannot be used to make changes after the fact.
	/// </summary>
	//-----------------------------------------------------------------------------
	void setRootDataPath(const std::string & path);

	//-----------------------------------------------------------------------------
	//  Name : setCopyrightData ()
	/// <summary>
	/// Set the application copyright information as it should be displayed to the
	/// user where applicable.
	/// </summary>
	//-----------------------------------------------------------------------------
	void setCopyrightData(const std::string& copyright);

	//-----------------------------------------------------------------------------
	//  Name : setVersionData ()
	/// <summary>
	/// Set the application version information as it should be displayed to the
	/// user where applicable.
	/// </summary>
	//-----------------------------------------------------------------------------
	void setVersionData(const std::string& version);

	//-----------------------------------------------------------------------------
	//  Name : quit ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void quit();

	//-----------------------------------------------------------------------------
	//  Name : getWorld ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline World& getWorld() { return *mWorld; }

	//-----------------------------------------------------------------------------
	//  Name : getThreadPool ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline ThreadPool& getThreadPool() { return *mThreadPool; }

	//-----------------------------------------------------------------------------
	//  Name : getAssetManager ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline AssetManager& getAssetManager() { return *mAssetManager; }

	//-----------------------------------------------------------------------------
	//  Name : getTimer ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline Timer& getTimer() { return *mTimer; }

	//-----------------------------------------------------------------------------
	//  Name : getInput ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	InputContext& getInput();

	//-----------------------------------------------------------------------------
	//  Name : getWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderWindow& getWindow();

	//-----------------------------------------------------------------------------
	//  Name : getMainWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	RenderWindow& getMainWindow();

	//-----------------------------------------------------------------------------
	//  Name : getWindows ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const std::vector<std::shared_ptr<RenderWindow>>& getWindows() const { return mWindows; }

	//-------------------------------------------------------------------------
	// Public Virtual Methods
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : initInstance ()
	/// <summary>
	/// Initializes the entire engine here.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool initInstance(const std::string& rootDataDir, const std::string& commandLine);

	//-----------------------------------------------------------------------------
	//  Name : begin ()
	/// <summary>
	/// Signals the beginning of the physical post-initialization stage.
	/// From here on, the game engine has control over processing.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual int begin();

	//-----------------------------------------------------------------------------
	//  Name : shutDown ()
	/// <summary>
	/// Shuts down the game engine, and frees up all resources.
	/// Note : You must explicitly call this method PRIOR to main function return.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool shutDown();

	//-----------------------------------------------------------------------------
	//  Name : registerWindow (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void registerWindow(std::shared_ptr<RenderWindow> window);

	//-----------------------------------------------------------------------------
	//  Name : createMainWindow ()
	/// <summary>
	/// Set the application main window.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual std::shared_ptr<RenderWindow> createMainWindow();

	//-----------------------------------------------------------------------------
	//  Name : getRenderFrame ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::uint32_t getRenderFrame() const { return mRenderFrame; }

private:
	//-----------------------------------------------------------------------------
	//  Name : registerMainWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool registerMainWindow(RenderWindow& window);
protected:
	//-------------------------------------------------------------------------
	// Protected Virtual Methods
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : initLogging (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool initLogging();

	//-----------------------------------------------------------------------------
	//  Name : initAssetManager (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool initAssetManager();

	//-----------------------------------------------------------------------------
	//  Name : initDisplay ()
	/// <summary>
	/// Initialize the display driver, create device window, load
	/// render configuration etc.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool initDisplay();

	//-----------------------------------------------------------------------------
	//  Name : initInputMappings (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool initInputMappings();

	//-----------------------------------------------------------------------------
	//  Name : initSystems (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool initSystems();

	//-----------------------------------------------------------------------------
	//  Name : initApplication ()
	/// <summary>
	/// Initialize all required aspects of the application ready for us to begin.
	/// This includes setting up all required states that the application may enter
	/// etc.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool initApplication();

	//-----------------------------------------------------------------------------
	//  Name : frameBegin () (Protected, Virtual)
	/// <summary>
	/// Called to signal that we are just about to commence processing of the
	/// current frame.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool frameBegin(bool runSimulation = true);

	//-----------------------------------------------------------------------------
	//  Name : processWindow (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void processWindow(RenderWindow& window);

	//-----------------------------------------------------------------------------
	//  Name : frameWindowBegin (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameWindowBegin(RenderWindow& window);

	//-----------------------------------------------------------------------------
	//  Name : frameWindowUpdate (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameWindowUpdate(RenderWindow& window);

	//-----------------------------------------------------------------------------
	//  Name : frameWindowRender () (Protected, Virtual)
	/// <summary>
	/// Actually performs the default rendering of the current frame for that window.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameWindowRender(RenderWindow& window);

	//-----------------------------------------------------------------------------
	//  Name : frameWindowEnd () (Protected, Virtual)
	/// <summary>
	/// Called to signal that we have finished rendering the current frame for that window.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameWindowEnd(RenderWindow& window);

	//-----------------------------------------------------------------------------
	//  Name : frameEnd () (Protected, Virtual)
	/// <summary>
	/// Called to signal that we have finished rendering the current frame.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameEnd();

	//-------------------------------------------------------------------------
	// Protected Member Variables
	//-------------------------------------------------------------------------
	/// Application Version Information
	std::string mVersion;
	/// Cached copyright string retrieved from resource string table.
	std::string mCopyright;
	/// The root data directory used by the file system.
	std::string mRootDataDir;
	/// Collection of application windows.
	std::vector<std::shared_ptr<RenderWindow>> mWindows;
	/// Collection of application windows that are pending closure.
	std::vector<std::shared_ptr<RenderWindow>> mPendingClosureWindows;
	/// Currently processed window.
	std::shared_ptr<RenderWindow> mWindow;
	/// Is V-sync enabled?
	bool mVsync = false;
	/// Maximum frame rate cap (0 = disabled)
	float mMaximumFPS = 0;
	/// Maximum frame rate to attempt to smooth VSync input lag.
	float mMaximumSmoothedFPS = 59.0f;
	/// Is timer smoothing enabled?
	bool mTimerSmoothing = false;
	/// Is Application running?
	bool mRunning = true;
	/// Current render frame.
	std::uint32_t mRenderFrame = 0;
	/// The Application's World containing the ECS
	std::unique_ptr<World> mWorld;
	/// The Application's Asset Manager
	std::unique_ptr<AssetManager> mAssetManager;
	/// The Application's Timer
	std::unique_ptr<Timer> mTimer;
	/// The Application's ThreadPool
	std::unique_ptr<ThreadPool> mThreadPool;
	/// The Application's ActionMapper
	std::unique_ptr<ActionMapper> mActionMapper;
};
