#include "device.h"
#include "impl/device_impl.h"

namespace audio
{

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
	static std::string empty;
	return empty;
	;
}

const std::string& device::get_info() const
{
	if(_impl)
	{
		return _impl->get_info();
	}
	static std::string empty;
	return empty;
}

std::vector<std::string> device::enumerate()
{
	return priv::device_impl::enumerate();
}
}
