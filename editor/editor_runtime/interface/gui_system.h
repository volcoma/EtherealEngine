#pragma once

#include "editor_core/gui/gui.h"
#include "runtime/system/events.h"

#include "core/graphics/texture.h"
#include "runtime/assets/asset_handle.h"
#include <map>
#include <memory>

class render_window;
//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : gui_system (Class)
/// <summary>
/// Class for the management of interface elements.
/// </summary>
//-----------------------------------------------------------------------------
struct gui_system
{
	// window id, is_focused
	using window_info = std::pair<std::uint32_t, bool>;

	gui_system();
	~gui_system();

	void frame_begin(delta_t);

	std::uint32_t get_draw_calls() const;
	ImGuiContext* get_context(std::uint32_t id);

	void push_context(std::uint32_t id);
	void draw_begin(render_window& window, delta_t dt);

	void draw_end();
	void pop_context();

private:
	void platform_events(const window_info& info, const std::vector<mml::platform_event>&);

	std::map<uint32_t, ImGuiContext*> contexts_;
	ImFontAtlas atlas_;
	ImGuiContext* initial_context_ = nullptr;
};

namespace gui
{
inline texture_info get_info(const std::shared_ptr<gfx::texture>& texture)
{
	gui::texture_info info;
	info.texture = texture;
	bool is_rt = texture ? texture->is_render_target() : false;
	info.is_rt = is_rt;
	info.is_origin_bl = gfx::is_origin_bottom_left();
	return info;
}

inline texture_info get_info(const asset_handle<gfx::texture>& asset)
{
	return get_info(asset.get_asset());
}
}

struct gui_style
{
	struct hsv_setup
	{
		float col_main_hue = 0.0f / 255.0f;
		float col_main_sat = 200.0f / 255.0f;
		float col_main_val = 170.0f / 255.0f;

		float col_area_hue = 0.0f / 255.0f;
		float col_area_sat = 0.0f / 255.0f;
		float col_area_val = 60.0f / 255.0f;

		float col_back_hue = 0.0f / 255.0f;
		float col_back_sat = 0.0f / 255.0f;
		float col_back_val = 35.0f / 255.0f;

		float col_text_hue = 0.0f / 255.0f;
		float col_text_sat = 0.0f / 255.0f;
		float col_text_val = 255.0f / 255.0f;
		float frame_rounding = 0.0f;
	};

	void reset_style();
	void set_style_colors(const hsv_setup& _setup);
	void load_style();
	void save_style();
	static hsv_setup get_dark_style();
	static hsv_setup get_lighter_red();
	hsv_setup setup;
};

gui_style& get_gui_style();
