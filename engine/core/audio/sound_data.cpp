#include "sound_data.h"
#include "logger.h"
#include "utils.h"

namespace audio
{

void sound_data::convert_to_mono()
{
	if(info.channels == 2)
	{
		data = utils::convert_to_mono(data, info.bytes_per_sample);
		info.channels = 1;
	}
	else if(info.channels > 2)
	{
		log_error("Does not support mono conversion of buffers with more than 2 channels");
	}
}

void sound_data::convert_to_stereo()
{
	if(info.channels == 1)
	{
		data = utils::convert_to_stereo(data, info.bytes_per_sample);
		info.channels = 2;
	}
	else if(info.channels > 2)
	{
		log_error("Does not support mono conversion of buffers with more than 2 channels");
	}
}
}
