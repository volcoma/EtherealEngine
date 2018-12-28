#pragma once
#include "render_window.h"

#include <core/cmd_line/parser.hpp>
#include <core/common/basetypes.hpp>

#include <memory>
#include <vector>

namespace runtime
{
struct renderer
{
	renderer(cmd_line::parser& parser);
	~renderer();

	//-----------------------------------------------------------------------------
	//  Name : init_backend ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	bool init_backend(cmd_line::parser& parser);

	//-----------------------------------------------------------------------------
	//  Name : frame_end ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void frame_end(delta_t);

	//-----------------------------------------------------------------------------
	//  Name : get_render_frame ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline std::uint32_t get_render_frame() const
	{
		return render_frame_;
	}

	//-----------------------------------------------------------------------------
	//  Name : register_window ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void register_window(std::unique_ptr<render_window> window);

	//-----------------------------------------------------------------------------
	//  Name : get_windows ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<std::unique_ptr<render_window>>& get_windows() const;
	const std::unique_ptr<render_window>& get_window(std::uint32_t id) const;
	const std::unique_ptr<render_window>& get_main_window() const;
	void hide_all_secondary_windows();
	void show_all_secondary_windows();
	//-----------------------------------------------------------------------------
	//  Name : get_focused_window ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	render_window* get_focused_window() const;
	void process_pending_windows();

	void platform_events(const std::pair<std::uint32_t, bool>& info,
						 const std::vector<mml::platform_event>& events);

protected:
	std::uint32_t render_frame_ = 0;

	/// engine windows
	std::unique_ptr<mml::window> init_window_;
	std::vector<std::unique_ptr<render_window>> windows_;
	std::vector<std::unique_ptr<render_window>> windows_pending_addition_;
};
} // namespace runtime
