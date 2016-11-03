#pragma once
#include <string>
#include <cstdint>
#include <vector>
#include <memory>
#include "Core/internal/assert/assert.h"
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
	Application															();
	virtual ~Application												();

    //-------------------------------------------------------------------------
    // Public Methods
    //-------------------------------------------------------------------------
	bool									frameAdvance				(bool runSimulation = true);
	void									setRootDataPath				(const std::string & path);
	void									setCopyrightData			(const std::string& copyright);
	void									setVersionData				(const std::string& version);
	void									quit						();

	inline World&							getWorld					( ) { return *mWorld; }
	inline ThreadPool&						getThreadPool				( ) { return *mThreadPool; }
	inline AssetManager&					getAssetManager				( ) { return *mAssetManagerEx; }
	inline Timer&							getTimer					( ) { return *mTimer; }
	InputContext&							getInput					( );
    RenderWindow&							getWindow					( );
	RenderWindow&							getMainWindow				( );
	inline const std::vector<std::shared_ptr<RenderWindow>>& getWindows() const { return mWindows; }
	//-------------------------------------------------------------------------
    // Public Virtual Methods
    //-------------------------------------------------------------------------
	virtual bool							initInstance				(const std::string& rootDataDir, const std::string& commandLine);
	virtual int								begin						();
	virtual bool							shutDown					();
	virtual void							registerWindow				(std::shared_ptr<RenderWindow> window);
	virtual std::shared_ptr<RenderWindow>	createMainWindow			();
	inline std::uint32_t					getRenderFrame				() const { return mRenderFrame; }

private:
	bool									registerMainWindow			(RenderWindow& window);
protected:
	//-------------------------------------------------------------------------
    // Protected Virtual Methods
    //-------------------------------------------------------------------------
	virtual bool							initLogging					();
	virtual bool							initAssetManager			();
	virtual bool							initDisplay					();
	virtual bool							initInputMappings			();
	virtual bool							initSystems					();
	virtual bool							initApplication				();

	virtual bool							frameBegin					(bool runSimulation = true);
	virtual void							processWindow				(RenderWindow& window);
	virtual void							frameWindowBegin			(RenderWindow& window);
	virtual void							frameWindowUpdate			(RenderWindow& window);
	virtual void							frameWindowRender			(RenderWindow& window);
	virtual void							frameWindowEnd				(RenderWindow& window);
	virtual void							frameEnd					();

    //-------------------------------------------------------------------------
    // Protected Member Variables
    //-------------------------------------------------------------------------
	std::string								mVersion;
	std::string								mCopyright;					// Cached copyright string retrieved from resource string table.
	std::string								mRootDataDir;				// The root data directory used by the file system.
	std::vector<std::shared_ptr<RenderWindow>>	mWindows;					// Collection of application windows.
	std::shared_ptr<RenderWindow>				mWindow;					// Currently processed window.

	// Configuration
	bool									mVsync = false;			// 
	float									mMaximumFPS = 0;			// Maximum frame rate cap (0 = disabled)
	float									mMaximumSmoothedFPS = 59.0f;// Maximum frame rate to attempt to smooth VSync input lag.
	bool									mTimerSmoothing = false;
	bool									mRunning = true;
	std::uint32_t							mRenderFrame = 0;
	std::unique_ptr<World>					mWorld;
	std::unique_ptr<AssetManager>			mAssetManagerEx;
	std::unique_ptr<Timer>					mTimer;
	std::unique_ptr<ThreadPool>				mThreadPool;
	std::unique_ptr<ActionMapper>			mActionMapper;
};
