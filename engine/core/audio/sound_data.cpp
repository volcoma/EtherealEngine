#include "sound_data.h"
#include "logger.h"
#include "utils.h"

namespace audio
{

sound_data::duration_t::rep sound_data::get_duration() const
{
    return duration.count();
}

void sound_data::convert_to_mono()
{
    if(channels == 2)
    {
        data = utils::convert_to_mono(data, bytes_per_sample);
        channels = 1;
    }
    else if(channels > 2)
    {
		log_error("Does not support mono conversion of buffers with more than 2 channels");
    }
}

void sound_data::convert_to_stereo()
{
    if(channels == 1)
    {
        data = utils::convert_to_stereo(data, bytes_per_sample);
        channels = 2;
    }
    else if(channels >= 2)
    {
		log_error("Does not support mono conversion of buffers with more than 2 channels");
    }
}
}
