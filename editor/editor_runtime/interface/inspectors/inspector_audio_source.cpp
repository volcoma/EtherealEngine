#include "inspector_audio_source.h"
#include "core/string_utils/string_utils.h"
#include "inspectors.h"

bool inspector_audio_source_component::inspect(rttr::variant& var, bool read_only,
											   const meta_getter& get_metadata)
{
	auto data = var.get_value<audio_source_component*>();

	if(data)
	{
		gui::PushFont(gui::GetFont("icons"));
		if(gui::Button(ICON_FA_PLAY))
		{
			data->play();
		}
		gui::SameLine();
		if(gui::Button(ICON_FA_PAUSE))
		{
			data->pause();
		}
		gui::SameLine();
		if(gui::Button(ICON_FA_STOP))
		{
			data->stop();
		}
		gui::PopFont();

		gui::SameLine();

		property_layout layout("");

		if(data->has_binded_sound())
		{
			float total_time = floorf(float(data->get_playing_duration().count()) * 100.0f) / 100.0f;
			auto current_time = data->get_playing_offset();
			float cur = float(current_time.count());

			if(total_time > 0.0f)
			{
				if(gui::SliderFloat("##playing_offset", &cur, 0.0f, total_time))
				{
					data->set_playing_offset(audio::sound_info::seconds_t(cur));
				}
			}
		}
	}

	return inspect_var(var, true, read_only, get_metadata);
}
