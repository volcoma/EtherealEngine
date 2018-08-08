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
    sound_impl(std::vector<std::uint8_t>&& buffer, const sound_info& info, bool stream = false);

    sound_impl(sound_impl&& rhs) = delete;
    sound_impl& operator=(sound_impl&& rhs) = delete;
    sound_impl(const sound_impl& rhs) = delete;
    sound_impl& operator=(const sound_impl& rhs) = delete;

    bool is_valid() const;

    const std::vector<native_handle_type>& native_handles() const {
        return handles_;
    }

    bool load_buffer();

private:
    friend class source_impl;

    bool load_buffer(const size_t chunk_size);

    void bind_to_source(source_impl* source);
    void unbind_from_source(source_impl* source);
    void unbind_from_all_sources();
    void cleanup();

    std::vector<native_handle_type> handles_;

    // transient data valid until the audio is being streamed from memory
    std::vector<std::uint8_t> buf_;
    size_t buf_ptr_ = 0;
    sound_info buf_info_;

    /// openal doesn't let us destroy sounds that are
    /// binded, so we have to keep this bookkeeping
    std::mutex mutex_;
    std::vector<source_impl*> bound_to_sources_;
};
}
}
