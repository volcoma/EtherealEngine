#include "sound_impl.h"
#include "../logger.h"
#include "check.h"
#include "source_impl.h"
#include "stb_vorbis.h"
#include <AL/al.h>
#include <AL/alext.h>
#include <algorithm>

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
	rhs.cleanup();
}

sound_impl& sound_impl::operator=(sound_impl&& rhs)
{
	_handle = std::move(rhs._handle);
	rhs.cleanup();

	return *this;
}

sound_impl::~sound_impl()
{
	unbind_from_all_sources();

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

void sound_impl::bind_to_source(source_impl* source)
{
	_bound_to_sources.push_back(source);
}

void sound_impl::unbind_from_source(source_impl* source)
{
	_bound_to_sources.erase(std::remove_if(std::begin(_bound_to_sources), std::end(_bound_to_sources),
										   [source](const auto& item) { return item == source; }),
							std::end(_bound_to_sources));
}

void sound_impl::unbind_from_all_sources()
{
	for(auto& source : _bound_to_sources)
	{
		if(source)
		{
			source->unbind();
		}
	}
	_bound_to_sources.clear();
}

void sound_impl::cleanup()
{
	_handle = 0;
	unbind_from_all_sources();
}
}
}
