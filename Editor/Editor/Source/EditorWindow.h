#pragma once
#include "Interface/GuiWindow.h"

class MainEditorWindow : public GuiWindow
{
public:
	//-----------------------------------------------------------------------------
	//  Name : EditorWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	MainEditorWindow();
	//-----------------------------------------------------------------------------
	//  Name : EditorWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	MainEditorWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style = sf::Style::Default);
	//-----------------------------------------------------------------------------
	//  Name : ~EditorWindow (virtual )
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
	virtual void onToolbar();

	virtual bool onProjectManager();
	virtual void openProjectManager() { mOpenProjectManager = true; }

private:
	bool mOpenProjectManager = false;
};