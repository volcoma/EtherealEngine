#include "listener.h"
#include "impl/listener_impl.h"

namespace audio
{
listener::listener()
	: impl_(std::make_unique<priv::listener_impl>())
{
}

listener::~listener() = default;

void listener::set_volume(float volume)
{
	if(impl_)
	{
		impl_->set_volume(volume);
	}
}

void listener::set_position(const float3& position)
{
	if(impl_)
	{
		impl_->set_position(position);
	}
}

void listener::set_velocity(const float3& velocity)
{
	if(impl_)
	{
		impl_->set_velocity(velocity);
	}
}

void listener::set_orientation(const float3& direction, const float3& up)
{
	if(impl_)
	{
		impl_->set_orientation(direction, up);
	}
}
}
