#include "device_impl.h"
#include "../logger.h"
#include "check.h"
#include <AL/alc.h>
#include <AL/alext.h>
#include <cstdint>
#include <sstream>

namespace audio
{
namespace priv
{

namespace openal
{
static bool al_has_extension(ALCdevice* dev, const char* ext)
{
	return (alcIsExtensionPresent(dev, ext) == AL_TRUE); // ALC_TRUE
}

static std::vector<std::string> al_get_strings(ALCdevice* dev, ALenum e)
{
	std::vector<std::string> result;

	const char* devices = reinterpret_cast<const char*>(alcGetString(dev, e));

	while(!std::string(devices).empty())
	{
		result.push_back(devices);
		devices += result.back().size() + 1;
	}

	return result;
}

static std::string al_version()
{
	std::stringstream ss;
	auto version = alGetString(AL_VERSION);
	if(version != nullptr)
	{
		ss << "OpenAL version: " << version;
	}
	return ss.str();
}

static std::string al_vendor()
{
	std::stringstream ss;
	auto vendor = alGetString(AL_VERSION);
	if(vendor != nullptr)
	{
		ss << "OpenAL vendor: " << vendor;
	}
	return ss.str();
}

static std::string al_extensions()
{
	std::stringstream ss;
	auto extensions = alGetString(AL_EXTENSIONS);
	if(extensions != nullptr)
	{
		ss << "OpenAL extensions: " << extensions;
	}
	return ss.str();
}

static std::string alc_extensions()
{
	std::stringstream ss;
	auto extensions = alcGetString(nullptr, ALC_EXTENSIONS);
	if(extensions != nullptr)
	{
		ss << "OpenALC extensions: " << extensions;
	}

	return ss.str();
}
}

device_impl::device_impl(int devnum)
{
	// device name
	auto playback_devices = enumerate_playback_devices();
	log_info("Supported audio playback devices:");
	for(const auto& dev : playback_devices)
	{
		log_info("-- " + dev);
	}
	auto capture_devices = enumerate_capture_devices();
	log_info("Supported audio capture devices:");
	for(const auto& dev : capture_devices)
	{
		log_info("-- " + dev);
	}

	if(devnum >= 0 && devnum < int(playback_devices.size()))
	{
		_device_id = playback_devices[std::size_t(devnum)];
	}

	// select device
	_device = alcOpenDevice(_device_id.empty() ? nullptr : _device_id.c_str());

	if(_device == nullptr)
	{
		log_error("Cant open audio playback device: " + _device_id);
		return;
	}
	bool has_efx_ = openal::al_has_extension(_device, "ALC_EXT_EFX");

	ALint attribs[4] = {0};
	attribs[0] = ALC_MAX_AUXILIARY_SENDS;
	attribs[1] = 4;

	// create context
	_context = alcCreateContext(_device, has_efx_ ? attribs : nullptr);

	if(_context == nullptr)
	{
		log_error("Cant create audio context for playback device: " + _device_id);
		return;
	}
	enable();

	_version = openal::al_version();
	_vendor = openal::al_vendor();
	_extensions = openal::al_extensions();

	log_info(_version);
	log_info(_vendor);
	// log_info(_extensions);
	// log_info(openal::alc_extensions());
	log_info("Using audio playback device: " + _device_id);

	al_check(alDistanceModel(AL_LINEAR_DISTANCE));
}

device_impl::~device_impl()
{
	if(_context != nullptr)
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(_context);
		_context = nullptr;
	}

	if(_device != nullptr)
	{
		alcCloseDevice(_device);
		_device = nullptr;
	}
}

void device_impl::enable()
{
	al_check(alcMakeContextCurrent(_context));
}

void device_impl::disable()
{
	al_check(alcMakeContextCurrent(nullptr));
}

bool device_impl::is_valid() const
{
	return (_device != nullptr) && (_context != nullptr);
}

const std::string& device_impl::get_device_id() const
{
	return _device_id;
}

const std::string& device_impl::get_version() const
{
	return _version;
}

const std::string& device_impl::get_vendor() const
{
	return _vendor;
}

const std::string& device_impl::get_extensions() const
{
	return _extensions;
}

std::vector<std::string> device_impl::enumerate_capture_devices()
{
	bool can_enumerate = openal::al_has_extension(nullptr, "ALC_ENUMERATION_EXT");

	if(can_enumerate)
	{
		return openal::al_get_strings(nullptr, ALC_CAPTURE_DEVICE_SPECIFIER);
	}

	return {};
}

std::vector<std::string> device_impl::enumerate_playback_devices()
{
	bool can_enumerate = openal::al_has_extension(nullptr, "ALC_ENUMERATION_EXT");

	if(can_enumerate)
	{
		return openal::al_get_strings(nullptr, ALC_ALL_DEVICES_SPECIFIER);
	}

	return {};
}
}
}
