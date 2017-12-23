#pragma once

namespace audio
{
namespace priv
{
class listener_impl
{
public:
	void set_gain(const float gain);
	void set_position(const float* position3);
	void set_velocity(const float* velocity3);
	void set_direction(const float* direction3);
};
}
}
