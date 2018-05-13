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

	auto devices = reinterpret_cast<const char*>(alcGetString(dev, e));

	while(!std::string(devices).empty())
	{
		result.emplace_back(devices);
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

//static std::string alc_extensions()
//{
//	std::stringstream ss;
//	auto extensions = alcGetString(nullptr, ALC_EXTENSIONS);
//	if(extensions != nullptr)
//	{
//		ss << "OpenALC extensions: " << extensions;
//	}

//	return ss.str();
//}
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
		device_id_ = playback_devices[std::size_t(devnum)];
	}

	// select device
	device_ = alcOpenDevice(device_id_.empty() ? nullptr : device_id_.c_str());

	if(device_ == nullptr)
	{
		log_error("Cant open audio playback device: " + device_id_);
		return;
	}
	bool has_efx_ = openal::al_has_extension(device_, "ALC_EXT_EFX");

	ALint attribs[4] = {0};
	attribs[0] = ALC_MAX_AUXILIARY_SENDS;
	attribs[1] = 4;

	// create context
	context_ = alcCreateContext(device_, has_efx_ ? attribs : nullptr);

	if(context_ == nullptr)
	{
		log_error("Cant create audio context for playback device: " + device_id_);
		return;
	}
	enable();

	version_ = openal::al_version();
	vendor_ = openal::al_vendor();
	extensions_ = openal::al_extensions();

	log_info(version_);
	log_info(vendor_);
	// log_info(_extensions);
	// log_info(openal::alc_extensions());
	log_info("Using audio playback device: " + device_id_);

	al_check(alDistanceModel(AL_LINEAR_DISTANCE));
}

device_impl::~device_impl()
{
	if(context_ != nullptr)
	{
		alcMakeContextCurrent(nullptr);
		alcDestroyContext(context_);
		context_ = nullptr;
	}

	if(device_ != nullptr)
	{
		alcCloseDevice(device_);
		device_ = nullptr;
	}
}

void device_impl::enable()
{
	al_check(alcMakeContextCurrent(context_));
}

void device_impl::disable()
{
	al_check(alcMakeContextCurrent(nullptr));
}

bool device_impl::is_valid() const
{
	return (device_ != nullptr) && (context_ != nullptr);
}

const std::string& device_impl::get_device_id() const
{
	return device_id_;
}

const std::string& device_impl::get_version() const
{
	return version_;
}

const std::string& device_impl::get_vendor() const
{
	return vendor_;
}

const std::string& device_impl::get_extensions() const
{
	return extensions_;
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
