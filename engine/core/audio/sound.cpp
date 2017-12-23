#include "sound.h"
#include "impl/sound_impl.h"

namespace audio
{

sound::~sound() = default;

sound::sound(sound_data&& data)
	: _data(std::move(data))
	, _impl(std::make_unique<priv::sound_impl>(_data))

{
}

sound::sound(sound&& rhs)
	: _data(std::move(rhs._data))
	, _impl(std::move(rhs._impl))
{
	rhs._data = {};
	rhs._impl = nullptr;
}

sound& sound::operator=(sound&& rhs)
{
	_data = std::move(rhs._data);
	_impl = std::move(rhs._impl);

	rhs._data = {};
	rhs._impl = nullptr;

	return *this;
}

bool sound::is_valid() const
{
	return _impl && _impl->is_valid();
}

sound_data::duration_t sound::get_duration() const
{
	return _data.duration;
}

uint32_t sound::get_sample_rate() const
{
	return _data.sample_rate;
}

uint32_t sound::get_channels() const
{
	return _data.channels;
}

}
