#include "style_dock.h"

style_dock::style_dock(const std::string& dtitle, bool close_button, const ImVec2& min_size)
{
	initialize(dtitle, close_button, min_size, std::bind(&style_dock::render, this, std::placeholders::_1));
	auto& style = gui::get_gui_style();
	style.load_style();
	auto& setup = style.setup;
	style.set_style_colors(setup);
}

void style_dock::render(const ImVec2&)
{
	auto& style = gui::get_gui_style();
	auto& setup = style.setup;
	ImVec4 rgb;

	auto& col_main_hue = setup.col_main_hue;
	auto& col_main_sat = setup.col_main_sat;
	auto& col_main_val = setup.col_main_val;

	auto& col_area_hue = setup.col_area_hue;
	auto& col_area_sat = setup.col_area_sat;
	auto& col_area_val = setup.col_area_val;

	auto& col_back_hue = setup.col_back_hue;
	auto& col_back_sat = setup.col_back_sat;
	auto& col_back_val = setup.col_back_val;

	auto& col_text_hue = setup.col_text_hue;
	auto& col_text_sat = setup.col_text_sat;
	auto& col_text_val = setup.col_text_val;
	auto& frameRounding = setup.frame_rounding;

	gui::ColorConvertHSVtoRGB(col_main_hue, col_main_sat, col_main_val, rgb.x, rgb.y, rgb.z);
	if(gui::ColorEdit3("MAIN", &rgb.x, ImGuiColorEditFlags_HSV))
		gui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_main_hue, col_main_sat, col_main_val);

	gui::ColorConvertHSVtoRGB(col_area_hue, col_area_sat, col_area_val, rgb.x, rgb.y, rgb.z);
	if(gui::ColorEdit3("AREA", &rgb.x, ImGuiColorEditFlags_HSV))
		gui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_area_hue, col_area_sat, col_area_val);

	gui::ColorConvertHSVtoRGB(col_back_hue, col_back_sat, col_back_val, rgb.x, rgb.y, rgb.z);
	if(gui::ColorEdit3("BACK", &rgb.x, ImGuiColorEditFlags_HSV))
		gui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_back_hue, col_back_sat, col_back_val);

	gui::ColorConvertHSVtoRGB(col_text_hue, col_text_sat, col_text_val, rgb.x, rgb.y, rgb.z);
	if(gui::ColorEdit3("TEXT", &rgb.x, ImGuiColorEditFlags_HSV))
		gui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_text_hue, col_text_sat, col_text_val);

	gui::SliderFloat("ROUNDING", &frameRounding, 0.0f, 10.0f);

	if(gui::Button("SAVE"))
	{
		style.save_style();
	}
	gui::SameLine();
	if(gui::Button("RELOAD"))
	{
		style.load_style();
	}
	gui::SameLine();
	if(gui::Button("DEFAULT"))
	{
		style.reset_style();
	}
	else
	{
		style.set_style_colors(setup);
	}
}
