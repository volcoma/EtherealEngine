#include "docks.h"

namespace Docks
{

	void render_style(ImVec2 area)
	{
		auto& GUIStyle = gui::getGUIStyle();
		auto& setup = GUIStyle.setup;
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
		auto& frameRounding = setup.frameRounding;

		gui::ColorConvertHSVtoRGB(col_main_hue, col_main_sat, col_main_val, rgb.x, rgb.y, rgb.z);
		if(gui::ColorEdit3("Main", &rgb.x, ImGuiColorEditFlags_HSV))
			gui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_main_hue, col_main_sat, col_main_val);

		gui::ColorConvertHSVtoRGB(col_area_hue, col_area_sat, col_area_val, rgb.x, rgb.y, rgb.z);
		if(gui::ColorEdit3("Area", &rgb.x, ImGuiColorEditFlags_HSV))
			gui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_area_hue, col_area_sat, col_area_val);

		gui::ColorConvertHSVtoRGB(col_back_hue, col_back_sat, col_back_val, rgb.x, rgb.y, rgb.z);	
		if(gui::ColorEdit3("Back", &rgb.x, ImGuiColorEditFlags_HSV))
			gui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_back_hue, col_back_sat, col_back_val);

		gui::ColorConvertHSVtoRGB(col_text_hue, col_text_sat, col_text_val, rgb.x, rgb.y, rgb.z);
		if(gui::ColorEdit3("Text", &rgb.x, ImGuiColorEditFlags_HSV))
			gui::ColorConvertRGBtoHSV(rgb.x, rgb.y, rgb.z, col_text_hue, col_text_sat, col_text_val);

		gui::SliderFloat("Rounding", &frameRounding, 0.0f, 10.0f);

		if (gui::Button("Save"))
		{
			GUIStyle.save_style();
		}
		gui::SameLine();
		if (gui::Button("Reload"))
		{
			GUIStyle.load_style();
		}
		gui::SameLine();
		if (gui::Button("Default"))
		{
			GUIStyle.reset_style();
		}
		else
		{
			GUIStyle.set_style_colors(setup);
		}
		
		
	}

};