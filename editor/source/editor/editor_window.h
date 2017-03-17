#pragma once
#include "interface/gui_window.h"

class MainEditorWindow : public GuiWindow
{
public:
	//-----------------------------------------------------------------------------
	//  Name : MainEditorWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	MainEditorWindow();
	
	//-----------------------------------------------------------------------------
	//  Name : MainEditorWindow ()
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

	virtual void render_dockspace();

	virtual void on_start_page();
	void show_start_page(bool show) { _show_start_page = show; }
private:
	bool _show_start_page = true;
};