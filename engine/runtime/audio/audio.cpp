#include "audio.h"
#include "core/logging/logging.h"

namespace runtime
{

bool audio_device::initialize()
{
	// enumerate audio devices
	auto devices = audio::device::enumerate();
	APPLOG_SEPARATOR();
	APPLOG_INFO("Available audio devices...");
	APPLOG_SEPARATOR();

	for(const auto& dev : devices)
	{
		APPLOG_INFO("Audio device: {0}", dev);
	}
	APPLOG_SEPARATOR();

	_device = std::make_unique<audio::device>();

	if(!_device->is_valid())
	{
		APPLOG_ERROR("Cant open audio device: {0}", _device->get_device_id());
		APPLOG_SEPARATOR();
		return false;
	}
	APPLOG_INFO("Using audio device: {0}", _device->get_device_id());
	APPLOG_SEPARATOR();

	return true;
}

void audio_device::dispose()
{
    _device.reset();
}
}
