#include "Docks.h"
#include "../../EditorApp.h"

namespace Docks
{

	void renderStyle(ImVec2 area)
	{
		auto& GUIStyle = gui::getGUIStyle();
		ImVec4 rgb;

		auto& col_main_hue = GUIStyle.col_main_hue;
		auto& col_main_sat = GUIStyle.col_main_sat;
		auto& col_main_val = GUIStyle.col_main_val;

		auto& col_area_hue = GUIStyle.col_area_hue;
		auto& col_area_sat = GUIStyle.col_area_sat;
		auto& col_area_val = GUIStyle.col_area_val;

		auto& col_back_hue = GUIStyle.col_back_hue;
		auto& col_back_sat = GUIStyle.col_back_sat;
		auto& col_back_val = GUIStyle.col_back_val;

		auto& col_text_hue = GUIStyle.col_text_hue;
		auto& col_text_sat = GUIStyle.col_text_sat;
		auto& col_text_val = GUIStyle.col_text_val;
		auto& frameRounding = GUIStyle.frameRounding;

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

		if (gui::Button("Reset to Defaults"))
		{
			GUIStyle.resetStyle();
		}
		else
		{
			ImVec4 col_text = ImColor::HSV(col_text_hue, col_text_sat, col_text_val);
			ImVec4 col_main = ImColor::HSV(col_main_hue, col_main_sat, col_main_val);
			ImVec4 col_back = ImColor::HSV(col_back_hue, col_back_sat, col_back_val);
			ImVec4 col_area = ImColor::HSV(col_area_hue, col_area_sat, col_area_val);
			GUIStyle.setStyleColors(frameRounding, col_text, col_main, col_back, col_area);
		}
	}

};