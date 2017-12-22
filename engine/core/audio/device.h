#pragma once
#include <string>
#include <vector>

#include "listener.h"
#include "sound.h"
#include "source.h"

#include <AL/alc.h>

namespace audio
{

struct device
{
    device(int devnum = 0);
    ~device();

	ALCdevice* dev = nullptr;
	ALCcontext* ctx = nullptr;

	audio::listener listener;
	std::vector<sound> sounds;
	std::vector<source> sources;

	std::string devname;

	bool init(int devnum);
	void quit();

	void enable();
	void disable();

	void clear();
	void reset();

	bool has_efx() const;
	std::size_t insert_sound(const sound& sound);
	void erase_sound(std::size_t sound);

	std::size_t insert_source(const source& src);
	void erase_source(std::size_t source);

	bool ok() const;
};
}
