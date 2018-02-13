#pragma once

#include "../types.h"
#include <AL/al.h>
#include <cstdint>
#include <mutex>
#include <vector>

namespace audio
{
namespace priv
{
class sound_impl;

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
	bool bind(sound_impl* sound);
	bool has_binded_sound() const;
	void unbind();
	void purge();

	void set_loop(bool on);
	void set_volume(float volume);
	void set_pitch(float pitch);
	void set_position(const float3& position);
	void set_velocity(const float3& velocity);
	void set_orientation(const float3& direction, const float3& up);

	void set_volume_rolloff(float rolloff);
	void set_distance(float mind, float maxd);
	void set_playing_offset(float seconds);
	float get_playing_offset() const;
	float get_playing_duration() const;

	void play() const;
	void stop() const;
	void pause() const;
	bool is_playing() const;
	bool is_paused() const;
	bool is_stopped() const;
	bool is_binded() const;
	bool is_valid() const;
	bool is_looping() const;

	native_handle_type native_handle() const;

private:
	void bind_sound(sound_impl* sound);
	void unbind_sound();

	native_handle_type handle_ = 0;

    /// openal doesn't let us destroy sounds that are
    /// binded, so we have to keep this bookkeeping
	std::mutex mutex_;  
    /// non owning
	sound_impl* bound_sound_ = nullptr;
};
}
}
