#pragma once
#include "runtime/rendering/render_window.h"
#include "docks/imguidock.h"

class GuiWindow : public RenderWindow
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
	GuiWindow();
	//-----------------------------------------------------------------------------
	//  Name : EditorWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	GuiWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style = sf::Style::Default);
	//-----------------------------------------------------------------------------
	//  Name : ~EditorWindow (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~GuiWindow();

	//-----------------------------------------------------------------------------
	//  Name : frameBegin (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameBegin();
	//-----------------------------------------------------------------------------
	//  Name : frameUpdate (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameUpdate(float dt);
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
	//  Name : frameEnd (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frameEnd();
	//-----------------------------------------------------------------------------
	//  Name : filterEvent (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool filterEvent(const sf::Event& event);

	//-----------------------------------------------------------------------------
	//  Name : getDockspace ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline ImGuiDock::Dockspace& getDockspace() { return mDockspace; }
	inline ImGuiContext* getContext() { return mGuiContext; }
private:
	ImGuiDock::Dockspace mDockspace;
	ImGuiContext* mGuiContext;
};