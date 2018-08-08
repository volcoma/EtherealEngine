#include "sound.h"
#include "impl/sound_impl.h"

namespace audio
{
sound::sound() = default;

sound::~sound() = default;

sound::sound(sound_data&& data, bool stream)
    : impl_(std::make_unique<priv::sound_impl>(std::move(data.data), data.info, stream))
    , info_(std::move(data.info))
{
}

sound::sound(sound&& rhs) noexcept = default;
sound& sound::operator=(sound&& rhs) noexcept = default;

bool sound::is_valid() const
{
    return impl_ && impl_->is_valid();
}

const sound_info& sound::get_info() const
{
    return info_;
}

bool sound::load_buffer()
{
    return impl_ && impl_->load_buffer();
}

uintptr_t sound::uid() const
{
    return reinterpret_cast<uintptr_t>(impl_.get());
}

}
