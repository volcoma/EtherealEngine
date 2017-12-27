#include "inspector_audio_source.h"
#include "inspectors.h"
#include "core/string_utils/string_utils.h"

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
		gui::SameLine();
        auto total_time = data->get_playing_duration();
		auto current_time = data->get_playing_offset();
        float percent = static_cast<float>(current_time.count() / total_time.count());
		auto label = string_utils::format("%.2f/%.2f", current_time.count(), total_time.count());
		gui::ProgressBar(percent, ImVec2(0.0f, 0.0f), label.c_str());
		gui::PopFont();
		gui::Separator();
	}
    
	return inspect_var(var, true, read_only, get_metadata);
}
