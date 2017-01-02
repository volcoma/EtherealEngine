#pragma once
#include "runtime/rendering/render_window.h"
#include "docks/imguidock.h"

class GuiWindow : public RenderWindow
{
public:
	//-----------------------------------------------------------------------------
	//  Name : GuiWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	GuiWindow();
	//-----------------------------------------------------------------------------
	//  Name : GuiWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	GuiWindow(sf::VideoMode mode, const std::string& title, std::uint32_t style = sf::Style::Default);
	//-----------------------------------------------------------------------------
	//  Name : ~GuiWindow (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual ~GuiWindow();

	//-----------------------------------------------------------------------------
	//  Name : frame_begin (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_begin();
	//-----------------------------------------------------------------------------
	//  Name : frame_update (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_update(std::chrono::duration<float> dt);
	//-----------------------------------------------------------------------------
	//  Name : frame_render (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_render(std::chrono::duration<float> dt);

	//-----------------------------------------------------------------------------
	//  Name : on_gui (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_gui(std::chrono::duration<float> dt) {}

	//-----------------------------------------------------------------------------
	//  Name : frame_end (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void frame_end();
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
	//  Name : get_dockspace ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline ImGuiDock::Dockspace& get_dockspace() { return _dockspace; }

private:
	///
	ImGuiDock::Dockspace _dockspace;
	///
	ImGuiContext* _gui_context;
};