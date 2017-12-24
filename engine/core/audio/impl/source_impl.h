#pragma once

#include "sound_impl.h"
#include <AL/al.h>

namespace audio
{
namespace priv
{
class source_impl
{
public:
	using native_handle_type = ALuint;
	source_impl();
	~source_impl();
	source_impl(source_impl&& rhs);
	source_impl& operator=(source_impl&& rhs);

	source_impl(const source_impl& rhs) = delete;
	source_impl& operator=(const source_impl& rhs) = delete;

	bool create();
	bool bind(sound_impl::native_handle_type buffer);
	void unbind() const;
	void purge();

	void set_loop(bool on);
	void set_volume(float volume);
	void set_pitch(float pitch);
	void set_position(const float* position3);
	void set_velocity(const float* velocity3);
	void set_orientation(const float* direction3, const float* up3);

	void set_volume_rolloff(float rolloff);
	void set_distance(float mind, float maxd);
	void set_playing_offset(float seconds);

	void play() const;
	void stop() const;
	void pause() const;
	bool is_playing() const;
	bool is_paused() const;
	bool is_binded() const;
	bool is_valid() const;
	bool is_looping() const;

	native_handle_type native_handle() const;
	sound_impl::native_handle_type binded_handle() const;

private:
	native_handle_type _handle = 0;
	std::size_t _id = 0;
};
}
}
