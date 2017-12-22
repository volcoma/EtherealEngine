#include "sound.h"
#include "audio.h"
#include "check.h"

#include <AL/al.h>

#include <cstring>
#include <fstream>
#include <sstream>

#include "stb_vorbis.h"

namespace audio
{
bool sound::load(const std::string& tp, const void* data, std::size_t data_size)
{
	if(tp.empty())
		return false;

	if(!data)
		return false;

	if(!data_size)
		return false;

	unload();

	_type = tp;
	// Clear error flag
	alGetError();

	if(_type == "ogg")
	{
		auto* oss = stb_vorbis_open_memory(reinterpret_cast<const unsigned char*>(data),
										   static_cast<int>(data_size), nullptr, nullptr);

		if(!oss)
		{
			log("cant decode .ogg file");
			return false;
		}
		stb_vorbis_info info = stb_vorbis_get_info(oss);
        _channels = static_cast<std::uint32_t>(info.channels);
		_sample_rate = info.sample_rate;

		std::size_t sz = std::size_t(stb_vorbis_stream_length_in_samples(oss) * std::size_t(info.channels));

		_samples.resize(sz, 0);

		stb_vorbis_get_samples_short_interleaved(oss, info.channels, _samples.data(), int(_samples.size()));

		stb_vorbis_close(oss);
	}
	else
	{
		log("unsupported file format");
		return false;
	}

	alCheck(alGenBuffers(1, &_handle));

	ALenum format = _channels == 1 ? AL_FORMAT_MONO16 : AL_FORMAT_STEREO16;

	alCheck(alBufferData(_handle, format, _samples.data(), ALsizei(get_samples_mem_size()),
						 ALsizei(_sample_rate)));

	_seconds = 0;
	if(_handle && (_samples.empty() == false))
	{
		ALint sz, bits, channels, freq;

		alCheck(alGetBufferi(_handle, AL_SIZE, &sz));
		alCheck(alGetBufferi(_handle, AL_BITS, &bits));
		alCheck(alGetBufferi(_handle, AL_CHANNELS, &channels));
		alCheck(alGetBufferi(_handle, AL_FREQUENCY, &freq));

		if(alGetError() == AL_NO_ERROR)
			_seconds = ALdouble(sz / channels / (bits / 8)) / ALdouble(freq);
	}
	return true;
}

void sound::unload()
{
	if(_handle)
	{
		alCheck(alDeleteBuffers(1, &_handle));
	}

	_samples.clear();
}

bool sound::ok() const
{
	return (_samples.empty() == false) && _handle;
}

std::size_t sound::get_samples_mem_size() const
{
    return _samples.size() * sizeof(decltype(_samples)::value_type);
}

sound::native_handle_type sound::native_handle() const
{
    return _handle;   
}
}
