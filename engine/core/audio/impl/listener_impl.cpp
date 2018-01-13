#include "listener_impl.h"
#include "check.h"
#include <AL/al.h>

namespace audio
{
namespace priv
{
void listener_impl::set_volume(float volume)
{
	al_check(alListenerf(AL_GAIN, volume));
}

void listener_impl::set_position(const float* position3)
{
	al_check(alListenerfv(AL_POSITION, position3));
}

void listener_impl::set_velocity(const float* velocity3)
{
	al_check(alListenerfv(AL_VELOCITY, velocity3));
}

void listener_impl::set_orientation(const float* direction3, const float* up3)
{
	// orientation { norm(at), norm(up) };
	float orientation6[] = {-direction3[0], -direction3[1], -direction3[2], up3[0], up3[1], up3[2]};
	al_check(alListenerfv(AL_ORIENTATION, orientation6));
}
}
}
