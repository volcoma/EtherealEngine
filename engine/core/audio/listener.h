#pragma once
#include <memory>

namespace audio
{
namespace priv
{
class listener_impl;
}
class listener
{
public:
	listener();
	~listener();
	void set_gain(const float gain);
	void set_position(const float* position3);
	void set_velocity(const float* velocity3);
	void set_direction(const float* direction3);

private:
	std::unique_ptr<priv::listener_impl> _impl;
};
}
