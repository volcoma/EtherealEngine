#pragma once

#include "../sound_data.h"
#include <AL/al.h>
#include <mutex>

namespace audio
{
namespace priv
{
class source_impl;

class sound_impl
{
public:
	using native_handle_type = ALuint;

	sound_impl();
	~sound_impl();
	sound_impl(std::vector<std::uint8_t>&& buffer, const sound_info& info);
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

	native_handle_type handle_ = 0;
	/// openal doesn't let us destroy sounds that are
	/// binded, so we have to keep this bookkeeping
	std::mutex mutex_;
	std::vector<source_impl*> bound_to_sources_;
};
}
}
