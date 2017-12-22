#pragma once

#include "sound.h"

namespace audio
{
struct source_impl;

class source
{
public:
    source();
    ~source();
    source(source&& rhs);
    source& operator=(source&& rhs);
    
    source(const source& rhs) = delete;
    source& operator=(const source& rhs) = delete;

	void play(const sound& snd);

	void loop(bool on);
	void gain(float gain);
	void pitch(float pitch);
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
	bool is_valid() const;

private:
	std::unique_ptr<source_impl> _impl;
};
}
