#pragma once

#include "../sound_data.h"
#include <AL/al.h>

namespace audio
{

class sound_impl
{
public:
	using native_handle_type = ALuint;

	sound_impl() = default;
	~sound_impl();
	sound_impl(const sound_data& data);
	sound_impl(sound_impl&& rhs);
	sound_impl& operator=(sound_impl&& rhs);

	sound_impl(const sound_impl& rhs) = delete;
	sound_impl& operator=(const sound_impl& rhs) = delete;

	bool is_valid() const;

	native_handle_type native_handle() const;

private:
	native_handle_type _handle = 0;
	std::size_t _id = 0;
};
}
