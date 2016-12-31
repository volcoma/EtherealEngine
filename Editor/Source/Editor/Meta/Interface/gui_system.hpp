#pragma once

#include "../../interface/gui_system.h"
#include "core/serialization/serialization.h"
SAVE(GUIStyle::HSVSetup)
{
	ar(
		cereal::make_nvp("col_main_hue", obj.col_main_hue),
		cereal::make_nvp("col_main_sat", obj.col_main_sat),
		cereal::make_nvp("col_main_val", obj.col_main_val),

		cereal::make_nvp("col_area_hue", obj.col_area_hue),
		cereal::make_nvp("col_area_sat", obj.col_area_sat),
		cereal::make_nvp("col_area_val", obj.col_area_val),

		cereal::make_nvp("col_back_hue", obj.col_back_hue),
		cereal::make_nvp("col_back_sat", obj.col_back_sat),
		cereal::make_nvp("col_back_val", obj.col_back_val),

		cereal::make_nvp("col_text_hue", obj.col_text_hue),
		cereal::make_nvp("col_text_sat", obj.col_text_sat),
		cereal::make_nvp("col_text_val", obj.col_text_val),

		cereal::make_nvp("frameRounding", obj.frameRounding)
	);
}

LOAD(GUIStyle::HSVSetup)
{
	ar(
		cereal::make_nvp("col_main_hue", obj.col_main_hue),
		cereal::make_nvp("col_main_sat", obj.col_main_sat),
		cereal::make_nvp("col_main_val", obj.col_main_val),

		cereal::make_nvp("col_area_hue", obj.col_area_hue),
		cereal::make_nvp("col_area_sat", obj.col_area_sat),
		cereal::make_nvp("col_area_val", obj.col_area_val),

		cereal::make_nvp("col_back_hue", obj.col_back_hue),
		cereal::make_nvp("col_back_sat", obj.col_back_sat),
		cereal::make_nvp("col_back_val", obj.col_back_val),

		cereal::make_nvp("col_text_hue", obj.col_text_hue),
		cereal::make_nvp("col_text_sat", obj.col_text_sat),
		cereal::make_nvp("col_text_val", obj.col_text_val),

		cereal::make_nvp("frameRounding", obj.frameRounding)
		);
}
