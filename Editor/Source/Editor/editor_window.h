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
	//  Name : frameRender (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameRender();

	//-----------------------------------------------------------------------------
	//  Name : onMenuBar (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void onMenuBar();

	//-----------------------------------------------------------------------------
	//  Name : onToolbar (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void onToolbar();


private:
	bool mOpenProjectManager = false;
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
	//  Name : frameRender (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameRender();
};