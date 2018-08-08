#include "../sound_data.h"

namespace audio
{

bool load_ogg_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
                          std::string& err);
bool load_wav_from_memory(const std::uint8_t* data, std::size_t data_size, sound_data& result,
                          std::string& err);
}
