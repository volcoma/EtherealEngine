#pragma once

namespace audio
{
namespace priv
{
class listener_impl
{
public:
	void set_volume(float volume);
	void set_position(const float* position3);
	void set_velocity(const float* velocity3);
	void set_orientation(const float* direction3, const float* up3);
};
}
}
