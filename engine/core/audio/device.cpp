#include "device.h"
#include "impl/device_impl.h"

namespace audio
{

namespace
{
static const std::string& get_empty()
{
	static std::string empty;
	return empty;
}
}

device::device(int devnum)
	: impl_(std::make_unique<priv::device_impl>(devnum))
{
}

device::~device() = default;

void device::enable()
{
	if(impl_)
	{
		impl_->enable();
	}
}

void device::disable()
{
	if(impl_)
	{
		impl_->disable();
	}
}

bool device::is_valid() const
{
	return impl_ && impl_->is_valid();
}

const std::string& device::get_device_id() const
{
	if(impl_)
	{
		return impl_->get_device_id();
	}
	return get_empty();
}

const std::string& device::get_vendor() const
{
	if(impl_)
	{
		return impl_->get_vendor();
	}
	return get_empty();
}

const std::string& device::get_version() const
{
	if(impl_)
	{
		return impl_->get_version();
	}
	return get_empty();
}

const std::string& device::get_extensions() const
{
	if(impl_)
	{
		return impl_->get_extensions();
	}
	return get_empty();
}

std::vector<std::string> device::enumerate_playback_devices()
{
	return priv::device_impl::enumerate_playback_devices();
}

std::vector<std::string> device::enumerate_capture_devices()
{
	return priv::device_impl::enumerate_capture_devices();
}
}
