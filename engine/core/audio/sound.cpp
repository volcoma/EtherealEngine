#include "sound.h"
#include "impl/sound_impl.h"

namespace audio
{

sound::~sound() = default;

sound::sound(sound_data&& data)
	: _impl(std::make_unique<priv::sound_impl>(std::move(data.data), data.info))
	, _info(std::move(data.info))

{
}

sound::sound(sound&& rhs)
	: _impl(std::move(rhs._impl))
	, _info(std::move(rhs._info))
{
	rhs._info = {};
	rhs._impl = nullptr;
}

sound& sound::operator=(sound&& rhs)
{
	_impl = std::move(rhs._impl);
	_info = std::move(rhs._info);

	rhs._impl = nullptr;
	rhs._info = {};

	return *this;
}

bool sound::is_valid() const
{
	return _impl && _impl->is_valid();
}

sound_info::duration_t sound::get_duration() const
{
	return _info.duration;
}

uint32_t sound::get_sample_rate() const
{
	return _info.sample_rate;
}

uint32_t sound::get_channels() const
{
	return _info.channels;
}

const sound_info& sound::get_info() const
{
	return _info;
}
}
