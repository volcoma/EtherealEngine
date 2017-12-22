#pragma once

namespace audio
{

class listener_impl
{
public:
	void gain(const float gain);
	void position(const float* position3);
	void velocity(const float* velocity3);
	void direction(const float* direction3);

};
}
