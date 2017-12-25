#include "sound_impl.h"
#include "../logger.h"
#include "check.h"
#include "repository.h"
#include "stb_vorbis.h"
#include <AL/al.h>
#include <AL/alext.h>

namespace audio
{
namespace priv
{
static ALenum get_format_for_channels(std::uint32_t channels)
{
	ALenum format = 0;
	switch(channels)
	{
		case 1:
			format = AL_FORMAT_MONO16;
			break;
		case 2:
			format = AL_FORMAT_STEREO16;
			break;
		default:
			log_error("Unsupported channel count : " + std::to_string(channels));
			format = 0;
			break;
	}

	return format;
}

template <typename T>
static std::size_t get_mem_size(const T& container)
{
	return container.size() * sizeof(typename T::value_type);
}

sound_impl::sound_impl(const sound_data& data)
{
	_id = get_repository().insert_sound(this);

	if(data.data.empty())
		return;

	ALenum format = get_format_for_channels(data.channels);

	alCheck(alGenBuffers(1, &_handle));
	alCheck(alBufferData(_handle, format, data.data.data(), ALsizei(get_mem_size(data.data)),
						 ALsizei(data.sample_rate)));
}

sound_impl::sound_impl(sound_impl&& rhs)
	: _handle(std::move(rhs._handle))
{
	rhs._handle = 0;
	_id = get_repository().insert_sound(this);
}

sound_impl& sound_impl::operator=(sound_impl&& rhs)
{
	_handle = std::move(rhs._handle);
	rhs._handle = 0;

	return *this;
}

sound_impl::~sound_impl()
{
	get_repository().erase_sound(_id);

	if(_handle)
	{
		alCheck(alDeleteBuffers(1, &_handle));
	}
}

bool sound_impl::is_valid() const
{
	return _handle != 0;
}

sound_impl::native_handle_type sound_impl::native_handle() const
{
	return _handle;
}
}
}
