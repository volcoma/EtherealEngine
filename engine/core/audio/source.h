#pragma once

#include "sound.h"

namespace audio
{
namespace priv
{
class source_impl;
}

class source
{
public:
	source();
	~source();
	source(source&& rhs);
	source& operator=(source&& rhs);

	source(const source& rhs) = delete;
	source& operator=(const source& rhs) = delete;

	void bind(const sound& snd);

	void set_loop(bool on);
	void set_volume(float volume);
	void set_pitch(float pitch);
	void set_position(const float* position3);
	void set_velocity(const float* velocity3);
	void set_orientation(const float* direction3, const float* up3);

	void set_volume_rolloff(float rolloff);
	void set_distance(float mind, float maxd);
	void set_playing_offset(sound_data::duration_t offset);

	void play();
	void stop();
	void pause();
	bool is_playing() const;
	bool is_paused() const;
	bool is_looping() const;
	bool is_valid() const;

private:
	std::unique_ptr<priv::source_impl> _impl;
};
}
