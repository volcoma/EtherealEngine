#include "load_ogg.h"

#include "stb_vorbis.h"

namespace audio
{

bool load_ogg_from_memory(const uint8_t *data, std::size_t data_size, sound_data &result)
{
    if(!data)
        return false;
    
    if(!data_size)
        return false;
    
    auto* oss = stb_vorbis_open_memory(data, static_cast<int>(data_size), nullptr, nullptr);
    
    if(!oss)
    {
        return false;
    }
    stb_vorbis_info info = stb_vorbis_get_info(oss);
    result.channels = static_cast<std::uint32_t>(info.channels);
    result.sample_rate = info.sample_rate;
    
    std::size_t sz = std::size_t(stb_vorbis_stream_length_in_samples(oss) * std::size_t(info.channels));
    float seconds = stb_vorbis_stream_length_in_seconds(oss);
    result.duration = sound_data::duration_t(seconds);
    result.samples.resize(sz, 0);
    
    stb_vorbis_get_samples_short_interleaved(oss, info.channels, result.samples.data(),
                                             int(result.samples.size()));
    
    stb_vorbis_close(oss);
    
    return true;
}


}
