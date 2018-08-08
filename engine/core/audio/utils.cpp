#include "utils.h"
#include "logger.h"
#include <cstring>
namespace audio
{
namespace utils
{
std::vector<std::uint8_t> convert_to_mono(const std::vector<std::uint8_t>& input,
                                          std::uint8_t bytes_per_sample)
{
    if(bytes_per_sample > 2)
    {
        log_error("Sound buffer is not 8/16 bits per sample. Unsupported");
        return input;
    }

    std::vector<std::uint8_t> output;
    output.reserve(input.size() / 2);

    if(bytes_per_sample == 1)
    {
        for(std::size_t i = 0, sz = input.size(); i < sz; i += 2 * bytes_per_sample)
        {
            const std::uint8_t left = input[i];
            const std::uint8_t right = input[i + bytes_per_sample];
            const auto mono_sample = std::uint8_t((int(left) + right) / 2);

            output.push_back(mono_sample);
        }
    }
    else if(bytes_per_sample == 2)
    {
        for(std::size_t i = 0, sz = input.size(); i < sz; i += 2 * bytes_per_sample)
        {
            const std::int16_t left = *reinterpret_cast<const std::int16_t*>(&input[i]);
            const std::int16_t right = *reinterpret_cast<const std::int16_t*>(&input[i + bytes_per_sample]);
            const auto mono_sample = std::int16_t((int(left) + right) / 2);

            const auto sample_part1 = std::uint8_t(mono_sample >> 0);
            const auto sample_part2 = std::uint8_t(mono_sample >> 8);
            output.push_back(sample_part1);
            output.push_back(sample_part2);
        }
    }
    return output;
}

std::vector<std::uint8_t> convert_to_stereo(const std::vector<std::uint8_t>& input,
                                            std::uint8_t bytes_per_sample)
{
    if(bytes_per_sample > 2)
    {
        log_error("Sound buffer is not 8/16 bits per sample");
        return input;
    }

    std::vector<std::uint8_t> output;
    output.resize(input.size() * 2, 0);

    for(std::size_t i = 0, sz = input.size(); i < sz; i += bytes_per_sample)
    {
        const std::size_t dst_idx = i * 2;
        std::memcpy(output.data() + dst_idx, input.data() + i, bytes_per_sample);
        std::memcpy(output.data() + dst_idx + bytes_per_sample, input.data() + i, bytes_per_sample);
    }

    return output;
}
}
}
