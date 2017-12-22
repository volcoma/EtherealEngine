#pragma once

#include <AL/al.h>
#include "sound_impl.h"

namespace audio
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

	void loop(const bool on);
	void gain(const float gain);
	void pitch(const float pitch);
	void position(const float* position3, bool relative = false);
	void velocity(const float* velocity3);
	void direction(const float* direction3);

	void attenuation(const float rollOff, const float refDistance);
	void distance(const float mind, const float maxd);

	void play() const;
	void stop() const;
	void pause() const;
	bool is_playing() const;
	bool is_binded() const;
	bool is_valid() const;

	native_handle_type native_handle() const;
    sound_impl::native_handle_type binded_handle() const;
private:
	native_handle_type _handle = 0;
	std::size_t _id = 0;
};
}
