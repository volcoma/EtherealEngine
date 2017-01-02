#pragma once

#include "core/subsystem.h"
#include "imgui/imgui.h"
#include "imgui/imgui_internal.h"
#include <memory>

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : GuiSystem (Class)
/// <summary>
/// Class for the management of interface elements.
/// </summary>
//-----------------------------------------------------------------------------
struct GuiSystem : public core::Subsystem
{
	bool initialize();
	void dispose();
	void frame_begin(std::chrono::duration<float>);
};

namespace gui
{
	using namespace ImGui;
}

struct GUIStyle
{
	struct HSVSetup
	{
		float col_main_hue = 145.0f / 255.0f;
		float col_main_sat = 255.0f / 255.0f;
		float col_main_val = 205.0f / 255.0f;

		float col_area_hue = 145.0f / 255.0f;
		float col_area_sat = 0.0f / 255.0f;
		float col_area_val = 65.0f / 255.0f;

		float col_back_hue = 145.0f / 255.0f;
		float col_back_sat = 0.0f / 255.0f;
		float col_back_val = 45.0f / 255.0f;

		float col_text_hue = 0.0f / 255.0f;
		float col_text_sat = 0.0f / 255.0f;
		float col_text_val = 255.0f / 255.0f;
		float frameRounding = 0.0f;
	};

	void reset_style();
	void set_style_colors(const HSVSetup& _setup);
	void load_style();
	void save_style();

	HSVSetup setup;
};

struct ITexture;
namespace gui
{   
	static const int drag_button = 0;
	void begin();

	// Helper function for passing ITexture to ImGui::Image.
	void Image(std::shared_ptr<ITexture> texture
		, const ImVec2& _size
		, const ImVec2& _uv0 = ImVec2(0.0f, 0.0f)
		, const ImVec2& _uv1 = ImVec2(1.0f, 1.0f)
		, const ImVec4& _tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
		, const ImVec4& _borderCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f)
	);


	// Helper function for passing ITexture to ImGui::ImageButton.
	bool ImageButton(std::shared_ptr<ITexture> texture
		, const ImVec2& _size
		, const ImVec2& _uv0 = ImVec2(0.0f, 0.0f)
		, const ImVec2& _uv1 = ImVec2(1.0f, 1.0f)
		, int _framePadding = -1
		, const ImVec4& _bgCol = ImVec4(0.0f, 0.0f, 0.0f, 0.0f)
		, const ImVec4& _tintCol = ImVec4(1.0f, 1.0f, 1.0f, 1.0f)
	);
	bool ImageButtonEx(std::shared_ptr<ITexture> texture
		, ImVec2 size
		, const char* tooltip = nullptr
		, bool selected = false
		, bool enabled = true
	);
	GUIStyle& getGUIStyle();
};

