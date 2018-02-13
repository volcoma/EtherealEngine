#include "sound.h"
#include "impl/sound_impl.h"

namespace audio
{

sound::~sound() = default;

sound::sound(sound_data&& data)
	: impl_(std::make_unique<priv::sound_impl>(std::move(data.data), data.info))
	, info_(std::move(data.info))
{
}

sound::sound(sound&& rhs)
	: impl_(std::move(rhs.impl_))
	, info_(std::move(rhs.info_))
{
	rhs.info_ = {};
	rhs.impl_ = nullptr;
}

sound& sound::operator=(sound&& rhs)
{
	impl_ = std::move(rhs.impl_);
	info_ = std::move(rhs.info_);

	rhs.impl_ = nullptr;
	rhs.info_ = {};

	return *this;
}

bool sound::is_valid() const
{
	return impl_ && impl_->is_valid();
}

const sound_info& sound::get_info() const
{
	return info_;
}
}
