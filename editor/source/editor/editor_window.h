#pragma once
#include "interface/gui_window.h"

class MainEditorWindow : public GuiWindow
{
public:
	//-----------------------------------------------------------------------------
	//  Name : ProjectManagerWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	MainEditorWindow();
	
	//-----------------------------------------------------------------------------
	//  Name : ProjectManagerWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	MainEditorWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style = sf::Style::Default);
	
	//-----------------------------------------------------------------------------
	//  Name : ~ProjectManagerWindow (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~MainEditorWindow();

	//-----------------------------------------------------------------------------
	//  Name : on_gui (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_gui(std::chrono::duration<float> dt);

	//-----------------------------------------------------------------------------
	//  Name : on_menubar (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_menubar();

	//-----------------------------------------------------------------------------
	//  Name : on_toolbar (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_toolbar();


private:
	bool _open_project_manager = false;
};

class ProjectManagerWindow : public GuiWindow
{
public:
	//-----------------------------------------------------------------------------
	//  Name : ProjectManagerWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	ProjectManagerWindow() = default;
	//-----------------------------------------------------------------------------
	//  Name : ProjectManagerWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	ProjectManagerWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style = sf::Style::Default);
	//-----------------------------------------------------------------------------
	//  Name : ~ProjectManagerWindow (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~ProjectManagerWindow() = default;
	//-----------------------------------------------------------------------------
	//  Name : on_gui (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_gui(std::chrono::duration<float> dt);
};