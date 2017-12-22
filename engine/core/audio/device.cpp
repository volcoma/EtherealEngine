#include "device.h"
#include "audio.h"
#include "check.h"
#include "blank_ogg.bin.h"
#include <AL/alc.h>
#include <AL/alext.h>
#include <cstdint>

namespace audio
{

device::device(int devnum)
{
    if(!init(devnum))
    {
        log("Cant open audio device.");
    }
}

device::~device()
{
    quit();
}

bool device::init(int devnum)
{
	// init
	dev = 0;
	ctx = 0;
	devname = std::string();

	// device name
	auto list = enumerate();
	if(devnum >= 0 && devnum < int(list.size()))
		devname = list[std::size_t(devnum)];

	// select device
	dev = alcOpenDevice(devname.empty() ? 0 : devname.c_str());

	if(!dev)
		return false;

	bool has_efx_ = has_efx();

	ALint attribs[4] = {0};
	attribs[0] = ALC_MAX_AUXILIARY_SENDS;
	attribs[1] = 4;

	// select device
	ctx = alcCreateContext(dev, has_efx_ ? attribs : nullptr);

	if(!ctx)
		return false;

	enable();

	//$alDistanceModel( AL_INVERSE_DISTANCE );
	alCheck(alDistanceModel(AL_INVERSE_DISTANCE_CLAMPED));
	listener.gain(1.f);

	reset();

	return true;
}

void device::enable()
{
	alCheck(alcMakeContextCurrent(ctx));
}

void device::disable()
{
	alCheck(alcMakeContextCurrent(nullptr));
}

void device::quit()
{
    clear();
	if(ctx)
	{
        alcMakeContextCurrent(nullptr);
		alcDestroyContext(ctx);
		ctx = nullptr;
	}

	if(dev)
	{
        (alcCloseDevice(dev));
		dev = nullptr;
	}
}

void device::clear()
{
	for(std::size_t index = sounds.size(); index-- != 0;)
	{
		erase_sound(index);
	}

	for(std::size_t index = sources.size(); index-- != 0;)
	{
		erase_source(index);
	}
	sounds.clear();
	sources.clear();
}

void device::reset()
{
	clear();

	// reserve id (0) ; blank
	sound snd;
	snd.load("ogg", blank_ogg::data(), blank_ogg::size());

	insert_sound(snd);
}

bool device::has_efx() const
{
	return (alcIsExtensionPresent(dev, "ALC_EXT_EFX") == AL_TRUE); // ALC_TRUE
}

std::size_t device::insert_sound(const sound& source)
{
	for(std::size_t i = 0; i < sounds.size(); i++)
	{
		if(sounds[i].ok() == false)
		{
			sounds[i] = source;
			return i;
		}
	}

	sounds.push_back(source);
	return sounds.size() - 1;
}

void device::erase_sound(std::size_t _sound)
{
	auto& sound = sounds[_sound];

	// unbind sound from all sources
	for(auto& source : sources)
	{
		if(source.binded_handle() == sound.native_handle())
			source.unbind();
	}

	sound.unload();
}

std::size_t device::insert_source(const source& src)
{
	for(std::size_t i = 0; i < sources.size(); i++)
	{
		if(!sources[i].native_handle())
		{
			sources[i] = src;
			return i;
		}
	}

	sources.push_back(src);
	return sources.size() - 1;
}

void device::erase_source(std::size_t source)
{
	sources[source].purge();
}

bool device::ok() const
{
	return dev && ctx;
}
}
