#pragma once

#include "sound_data.h"
#include <memory>

namespace audio
{
namespace priv
{
class sound_impl;
}

class sound
{
public:
	sound() = default;
	~sound();
	sound(sound_data&& data);
	sound(sound&& rhs);
	sound& operator=(sound&& rhs);

	sound(const sound& rhs) = delete;
	sound& operator=(const sound& rhs) = delete;

	bool is_valid() const;

	sound_data::duration_t get_duration() const;
	std::uint32_t get_sample_rate() const;
	std::uint32_t get_channels() const;
    const sound_data& get_data() const;

private:
    friend class source;

	sound_data _data;
	std::unique_ptr<priv::sound_impl> _impl;
};
}
