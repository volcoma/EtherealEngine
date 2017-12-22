#include "listener.h"
#include "check.h"
#include <AL/al.h>

namespace audio
{
void listener::gain(const float gain)
{
	alCheck(alListenerf(AL_GAIN, gain));
}

void listener::position(const float* position3)
{
	alCheck(alListenerfv(AL_POSITION, position3));
}

void listener::velocity(const float* velocity3)
{
	alCheck(alListenerfv(AL_VELOCITY, velocity3));
}

void listener::direction(const float* direction3)
{
	// orientation { norm(at), norm(up) };
	float orientation6[] = {direction3[0], direction3[1], direction3[2], 0, 1, 0};
	alCheck(alListenerfv(AL_ORIENTATION, orientation6));
}
}
