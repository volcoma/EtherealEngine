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

namespace detail
{
static ALenum get_format_for_channels(std::uint32_t channels, std::uint32_t bytes_per_sample)
{
	ALenum format = 0;
	switch(channels)
	{
		case 1:
		{
			switch(bytes_per_sample)
			{
				case 1:
					format = AL_FORMAT_MONO8;
					break;
				case 2:
					format = AL_FORMAT_MONO16;
					break;
				default:
					log_error("Unsupported bytes per sample count : " + std::to_string(bytes_per_sample));
					break;
			}
		}
		break;

		case 2:
		{
			switch(bytes_per_sample)
			{
				case 1:
					format = AL_FORMAT_STEREO8;
					break;
				case 2:
					format = AL_FORMAT_STEREO16;
					break;
				default:
					log_error("Unsupported bytes per sample count : " + std::to_string(bytes_per_sample));
					break;
			}
		}
		break;

		default:
			log_error("Unsupported channel count : " + std::to_string(channels));
			break;
	}

	return format;
}
}

sound_impl::sound_impl(std::vector<std::uint8_t>&& buffer, const sound_info& info)
{
	if(buffer.empty())
	{
		return;
	}

	ALenum format = detail::get_format_for_channels(info.channels, info.bytes_per_sample);

	al_check(alGenBuffers(1, &handle_));
	al_check(alBufferData(handle_, format, buffer.data(), ALsizei(buffer.size()), ALsizei(info.sample_rate)));

	buffer.clear();
}

sound_impl::sound_impl(sound_impl&& rhs)
	: handle_(std::move(rhs.handle_))
{
	rhs.cleanup();
}

sound_impl& sound_impl::operator=(sound_impl&& rhs)
{
	handle_ = std::move(rhs.handle_);
	rhs.cleanup();

	return *this;
}

sound_impl::sound_impl() = default;

sound_impl::~sound_impl()
{
	unbind_from_all_sources();

	if(handle_ != 0u)
	{
		al_check(alDeleteBuffers(1, &handle_));
	}
}

bool sound_impl::is_valid() const
{
	return handle_ != 0;
}

sound_impl::native_handle_type sound_impl::native_handle() const
{
	return handle_;
}

void sound_impl::bind_to_source(source_impl* source)
{
	std::lock_guard<std::mutex> lock(mutex_);
	bound_to_sources_.push_back(source);
}

void sound_impl::unbind_from_source(source_impl* source)
{
	std::lock_guard<std::mutex> lock(mutex_);
	bound_to_sources_.erase(std::remove_if(std::begin(bound_to_sources_), std::end(bound_to_sources_),
										   [source](const auto& item) { return item == source; }),
							std::end(bound_to_sources_));
}

void sound_impl::unbind_from_all_sources()
{
    // We do this here to avoid recursive mutex lock.
    mutex_.lock();
    auto sources = std::move(bound_to_sources_);
    bound_to_sources_.clear();
    mutex_.unlock();
    
	for(auto& source : sources)
	{
		if(source != nullptr)
		{
			source->unbind();
		}
	}
}

void sound_impl::cleanup()
{
	handle_ = 0;
	unbind_from_all_sources();
}
}
}
