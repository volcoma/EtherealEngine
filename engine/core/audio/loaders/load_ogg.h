#include "../sound_data.h"

namespace audio
{

bool load_ogg_from_memory(const uint8_t* data, std::size_t data_size, sound_data& result);
}
