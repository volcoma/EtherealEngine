#pragma once
#include "Runtime/Application/Application.h"
#include "Runtime/System/Singleton.h"
#include "EditState.h"
#include "Runtime/Assets/AssetManager.h"
#include "Core/logging/logging.h"
struct ConsoleLog : public logging::sinks::base_sink<std::mutex>//, public IConsole
{
	using ItemContainer = std::vector<std::pair<std::string, logging::level::level_enum>>;

	ConsoleLog();

	void _sink_it(const logging::details::log_msg& msg) override;
	void flush() override;
	ItemContainer getItems();
	void clearLog();
	void clearInput();

	std::string					InputBuf;
	ItemContainer				Items;
	bool						ScrollToBottom = true;
	const std::size_t			MaxSize = 20;
};



class EditorApp : public Application
{
public:
	//-----------------------------------------------------------------------------
	//  Name : frameBegin (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool frameBegin(bool runSimulation = true);

	//-----------------------------------------------------------------------------
	//  Name : frameWindowEnd (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameWindowEnd(RenderWindow& window);

	//-----------------------------------------------------------------------------
	//  Name : createMainWindow (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual std::shared_ptr<RenderWindow> createMainWindow();

	//-----------------------------------------------------------------------------
	//  Name : initInstance (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool initInstance(const std::string& rootDataDir, const std::string& commandLine);
	
	//-----------------------------------------------------------------------------
	//  Name : initApplication (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	
	virtual bool initApplication();
	//-----------------------------------------------------------------------------
	//  Name : initUI (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	
	virtual bool initUI();
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
	//  Name : initDocks (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool initDocks();

	//-----------------------------------------------------------------------------
	//  Name : initEditState (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool initEditState();

	//-----------------------------------------------------------------------------
	//  Name : shutDown (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool shutDown();

	//-----------------------------------------------------------------------------
	//  Name : getEditState ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline EditState& getEditState() { return mEditState; }

	//-----------------------------------------------------------------------------
	//  Name : createProject ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void createProject(const std::string& projectDir);

	//-----------------------------------------------------------------------------
	//  Name : openProject ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void openProject(const std::string& projectDir);
private:
	EditState mEditState;
	std::vector<std::unique_ptr<ImGuiDock::Dock>> mDocks;
};


template<>
inline Application& Singleton<Application>::create()
{
	static EditorApp t;
	return t;
};

template<>
inline EditorApp& Singleton<EditorApp>::create()
{
	return static_cast<EditorApp&>(Singleton<Application>::getInstance());
};