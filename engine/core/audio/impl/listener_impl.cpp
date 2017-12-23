#include "listener_impl.h"
#include "check.h"
#include <AL/al.h>

namespace audio
{
namespace priv
{
void listener_impl::set_gain(const float gain)
{
	alCheck(alListenerf(AL_GAIN, gain));
}

void listener_impl::set_position(const float* position3)
{
	alCheck(alListenerfv(AL_POSITION, position3));
}

void listener_impl::set_velocity(const float* velocity3)
{
	alCheck(alListenerfv(AL_VELOCITY, velocity3));
}

void listener_impl::set_direction(const float* direction3)
{
	// orientation { norm(at), norm(up) };
	float orientation6[] = {direction3[0], direction3[1], direction3[2], 0, 1, 0};
	alCheck(alListenerfv(AL_ORIENTATION, orientation6));
}
}
}
