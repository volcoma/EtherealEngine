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

void listener_impl::set_position(const float3& position)
{
	al_check(alListenerfv(AL_POSITION, position.data()));
}

void listener_impl::set_velocity(const float3& velocity)
{
	al_check(alListenerfv(AL_VELOCITY, velocity.data()));
}

void listener_impl::set_orientation(const float3& direction, const float3& up)
{
	// orientation { norm(at), norm(up) };
	float orientation6[] = {-direction[0], -direction[1], -direction[2], up[0], up[1], up[2]};
	al_check(alListenerfv(AL_ORIENTATION, orientation6));
}
}
}
