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
	: _impl(std::make_unique<priv::device_impl>(devnum))
{
}

device::~device() = default;

void device::enable()
{
	if(_impl)
	{
		_impl->enable();
	}
}

void device::disable()
{
	if(_impl)
	{
		_impl->disable();
	}
}

bool device::is_valid() const
{
	return _impl && _impl->is_valid();
}

const std::string& device::get_device_id() const
{
	if(_impl)
	{
		return _impl->get_device_id();
	}
	return get_empty();
}

const std::string& device::get_vendor() const
{
	if(_impl)
	{
		return _impl->get_vendor();
	}
	return get_empty();
}

const std::string& device::get_version() const
{
	if(_impl)
	{
		return _impl->get_version();
	}
	return get_empty();
}

const std::string& device::get_extensions() const
{
	if(_impl)
	{
		return _impl->get_extensions();
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
