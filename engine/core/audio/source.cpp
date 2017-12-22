#include "source.h"
#include "check.h"
#include <AL/al.h>
namespace audio
{
bool source::create()
{
	if(_handle)
		return true;
	alGenSources(1, &_handle);
	return _handle != 0;
}

bool source::bind(sound::native_handle_type buffer)
{
	if(is_binded())
		unbind();

	loop(false);
	alCheck(alSourcei(_handle, AL_SOURCE_RELATIVE, AL_FALSE));

	alCheck(alSourcei(_handle, AL_BUFFER, ALint(buffer)));
	// alSourceQueue

	alCheck(alSourcef(_handle, AL_MIN_GAIN, 0.0f));
	alCheck(alSourcef(_handle, AL_MAX_GAIN, 1.0f));

	gain(1.f);
	pitch(1.f);

	return true;
}

void source::unbind()
{
	alCheck(alSourcei(_handle, AL_BUFFER, 0));
}

void source::purge()
{
	unbind();
	if(!_handle)
		return;
	alCheck(alDeleteSources(1, &_handle));
	_handle = 0;
}

void source::play()
{
	alCheck(alSourcePlay(_handle));
}

void source::stop()
{
	alCheck(alSourceStop(_handle));
}

void source::pause()
{
	alCheck(alSourcePause(_handle));
}

bool source::is_playing() const
{
	ALint state;
	alCheck(alGetSourcei(_handle, AL_SOURCE_STATE, &state));
    return (state == AL_PLAYING);
}

bool source::is_binded() const
{
    ALint buffer = 0;
    alCheck(alGetSourcei(_handle, AL_BUFFER, &buffer));
    return (buffer != 0);   
}

void source::loop(const bool on)
{
	alCheck(alSourcei(_handle, AL_LOOPING, on ? AL_TRUE : AL_FALSE));
}

void source::gain(const float gain)
{
	alCheck(alSourcef(_handle, AL_GAIN, gain));
}

/* pitch, speed stretching */
void source::pitch(const float pitch)
{
	// if pitch == 0.f pitch = 0.0001f;
	alCheck(alSourcef(_handle, AL_PITCH, pitch));
}

/* tempo, time stretching */
/* @todo: check soundtouch library { pitch, tempo, bpm } */
/*

*/

void source::position(const float* position3, bool relative)
{
	alCheck(alSourcefv(_handle, AL_POSITION, position3));
	alCheck(alSourcei(_handle, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE));
}

void source::velocity(const float* velocity3)
{
	alCheck(alSourcefv(_handle, AL_VELOCITY, velocity3));
}

void source::direction(const float* direction3)
{
	alCheck(alSourcefv(_handle, AL_DIRECTION, direction3));
}

void source::attenuation(const float rollOff, const float refDistance)
{

	alCheck(alSourcef(_handle, AL_REFERENCE_DISTANCE, refDistance));
	alCheck(alSourcef(_handle, AL_ROLLOFF_FACTOR, rollOff));
}

void source::distance(const float mind, const float maxd)
{
	alCheck(alSourcef(_handle, AL_REFERENCE_DISTANCE, mind));
	alCheck(alSourcef(_handle, AL_REFERENCE_DISTANCE, maxd));
}

bool source::ok() const
{
	return _handle;
}

void source::play(const sound& snd)
{
	create();
	bind(snd.native_handle());
    play();
}

source::native_handle_type source::native_handle() const
{
    return _handle;    
}

sound::native_handle_type source::binded_handle() const
{
    ALint buffer = 0;
    alCheck(alGetSourcei(_handle, AL_BUFFER, &buffer));
    return sound::native_handle_type(buffer);
}
}
