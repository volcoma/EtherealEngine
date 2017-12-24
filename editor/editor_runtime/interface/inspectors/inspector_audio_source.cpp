#include "inspector_audio_source.h"
#include "inspectors.h"

bool inspector_audio_source_component::inspect(rttr::variant& var, bool read_only,
											   const meta_getter& get_metadata)
{
	auto data = var.get_value<audio_source_component*>();

	if(data)
	{
        gui::PushFont(gui::GetFont("icons"));
		if(gui::Button(ICON_FA_PLAY " PLAY"))
		{
			data->play();
		}
		gui::SameLine();
		if(gui::Button(ICON_FA_PAUSE " PAUSE"))
		{
			data->pause();
		}
		gui::SameLine();
		if(gui::Button(ICON_FA_STOP " STOP"))
		{
			data->stop();
		}
		gui::PopFont();
		gui::Separator();
	}
    
	return inspect_var(var, true, read_only, get_metadata);
}
