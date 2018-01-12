#pragma once

#include "../sound_data.h"
#include <AL/al.h>

namespace audio
{
namespace priv
{
class source_impl;

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
	friend class source_impl;

	void bind_to_source(source_impl* source);
	void unbind_from_source(source_impl* source);
	void unbind_from_all_sources();
	void cleanup();

	native_handle_type _handle = 0;
	std::size_t _id = 0;

	std::vector<source_impl*> _bound_to_sources;
};
}
}
