#pragma once

#include <AL/al.h>
#include <cstdint>
#include <string>
#include <vector>

namespace audio
{
class sound
{
public:
    using native_handle_type = ALuint;

	bool load(const std::string& type, const void* data, size_t size);
	void unload();

	bool ok() const;
    std::size_t get_samples_mem_size() const;
    
    native_handle_type native_handle() const;
    
private:
    native_handle_type _handle = 0;

	std::string _type;
	std::vector<std::int16_t> _samples;
	std::uint32_t _sample_rate = 0;
	std::uint32_t _channels = 0;
	double _seconds = 0;
};
}
