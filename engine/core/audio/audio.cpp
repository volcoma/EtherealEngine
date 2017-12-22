#include "audio.h"
#include "check.h"
#include <AL/al.h>
#include <AL/alc.h>
#include <sstream>
namespace audio
{
namespace priv
{
logger_t& get_logger()
{
	static logger_t logger;
	return logger;
}

static ALCdevice* s_lib_device = nullptr;

}

bool init()
{
	priv::s_lib_device = alcOpenDevice(nullptr);
	return priv::s_lib_device != nullptr;
}

void deinit()
{
	alCheck(alcMakeContextCurrent(nullptr));
	
	if(priv::s_lib_device)
	{
        alCheck(alcCloseDevice(priv::s_lib_device));
        priv::s_lib_device = nullptr;
	}
}

std::string info()
{
	std::stringstream ss;
	if(alGetString(AL_VERSION))
		ss << "OpenAL version: " << alGetString(AL_VERSION) << std::endl;
	if(alGetString(AL_RENDERER))
		ss << "OpenAL renderer: " << alGetString(AL_RENDERER) << std::endl;
	if(alGetString(AL_VENDOR))
		ss << "OpenAL vendor: " << alGetString(AL_VENDOR) << std::endl;
	if(alGetString(AL_EXTENSIONS))
		ss << "OpenAL extensions: " << alGetString(AL_EXTENSIONS) << std::endl;

	return ss.str();
}

std::vector<std::string> enumerate()
{
	std::vector<std::string> vs;

	if(alcIsExtensionPresent(NULL, (const ALCchar*)"ALC_ENUMERATION_EXT") == AL_TRUE)
	{
		const char* devices = (const char*)alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);

		while(std::string(devices).size())
		{
			vs.push_back(devices);
			devices += vs.back().size() + 1;
		}
	}

	return vs;
}

void log(const std::string& msg)
{
	auto logger = priv::get_logger();
	if(logger)
	{
		logger(msg);
	}
}

void set_logger(std::function<void(const std::string&)> logger)
{
	priv::get_logger() = std::move(logger);
}
}
