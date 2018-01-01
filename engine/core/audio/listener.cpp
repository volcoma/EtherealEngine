#include "listener.h"
#include "impl/listener_impl.h"

namespace audio
{
listener::listener()
	: _impl(std::make_unique<priv::listener_impl>())
{
}

listener::~listener() = default;

void listener::set_volume(float volume)
{
	if(_impl)
	{
		_impl->set_volume(volume);
	}
}

void listener::set_position(const float* position3)
{
	if(_impl)
	{
		_impl->set_position(position3);
	}
}

void listener::set_velocity(const float* velocity3)
{
	if(_impl)
	{
		_impl->set_velocity(velocity3);
	}
}

void listener::set_orientation(const float* direction3, const float* up3)
{
	if(_impl)
	{
		_impl->set_orientation(direction3, up3);
	}
}
}
