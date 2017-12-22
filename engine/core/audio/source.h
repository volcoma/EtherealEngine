#pragma once

#include "sound.h"

namespace audio
{
class source
{
public:
	using native_handle_type = ALuint;

	bool create();
	bool bind(sound::native_handle_type buffer);
	void play(const sound& snd);
	void unbind();
	void purge();

	void loop(const bool on);
	void gain(const float gain);
	void pitch(const float pitch);
	void position(const float* position3, bool relative = false);
	void velocity(const float* velocity3);
	void direction(const float* direction3);

	void attenuation(const float rollOff, const float refDistance);
	void distance(const float mind, const float maxd);

	void play();
	void stop();
	void pause();
	bool is_playing() const;
	bool is_binded() const;
	bool ok() const;

	native_handle_type native_handle() const;
	sound::native_handle_type binded_handle() const;

private:
	native_handle_type _handle = 0;
};
}
