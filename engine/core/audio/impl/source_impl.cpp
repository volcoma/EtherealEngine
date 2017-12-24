#include "source_impl.h"
#include "check.h"
#include "repository.h"

namespace audio
{
namespace priv
{
source_impl::source_impl()
{
	_id = get_repository().insert_source(this);
	create();
}

source_impl::~source_impl()
{
	get_repository().erase_source(_id);
	purge();
}

source_impl::source_impl(source_impl&& rhs)
	: _handle(std::move(rhs._handle))
{
	rhs._handle = 0;
	_id = get_repository().insert_source(this);
}

source_impl& source_impl::operator=(source_impl&& rhs)
{
	_handle = std::move(rhs._handle);
	rhs._handle = 0;

	return *this;
}

bool source_impl::create()
{
	if(_handle)
		return true;

	alCheck(alGenSources(1, &_handle));

	return _handle != 0;
}

bool source_impl::bind(sound_impl::native_handle_type buffer)
{
	if(is_binded())
	{   
        if(binded_handle() == buffer)
        {
            return true;
        };
		unbind();
    }
	alCheck(alSourcei(_handle, AL_SOURCE_RELATIVE, AL_FALSE));

	alCheck(alSourcei(_handle, AL_BUFFER, ALint(buffer)));

	return true;
}

void source_impl::unbind() const
{
	stop();
	alCheck(alSourcei(_handle, AL_BUFFER, 0));
}

void source_impl::purge()
{
	if(!_handle)
		return;

	unbind();

	alCheck(alDeleteSources(1, &_handle));

	_handle = 0;
}

void source_impl::set_playing_offset(float seconds)
{
	alCheck(alSourcef(_handle, AL_SEC_OFFSET, seconds));
}

void source_impl::play() const
{
	alCheck(alSourcePlay(_handle));
}

void source_impl::stop() const
{
	alCheck(alSourceStop(_handle));
}

void source_impl::pause() const
{
	alCheck(alSourcePause(_handle));
}

bool source_impl::is_playing() const
{
	ALint state;
	alCheck(alGetSourcei(_handle, AL_SOURCE_STATE, &state));
    return (state == AL_PLAYING);
}

bool source_impl::is_paused() const
{
    ALint state;
	alCheck(alGetSourcei(_handle, AL_SOURCE_STATE, &state));
    return (state == AL_PAUSED);
}

bool source_impl::is_binded() const
{
	ALint buffer = 0;
	alCheck(alGetSourcei(_handle, AL_BUFFER, &buffer));
	return (buffer != 0);
}

void source_impl::set_loop(bool on)
{
	alCheck(alSourcei(_handle, AL_LOOPING, on ? AL_TRUE : AL_FALSE));
}

void source_impl::set_volume(float volume)
{
	alCheck(alSourcef(_handle, AL_GAIN, volume));
}

/* pitch, speed stretching */
void source_impl::set_pitch(float pitch)
{
	// if pitch == 0.f pitch = 0.0001f;
	alCheck(alSourcef(_handle, AL_PITCH, pitch));
}

void source_impl::set_position(const float* position3)
{
	alCheck(alSourcefv(_handle, AL_POSITION, position3));
}

void source_impl::set_velocity(const float* velocity3)
{
	alCheck(alSourcefv(_handle, AL_VELOCITY, velocity3));
}

void source_impl::set_orientation(const float* direction3, const float* up3)
{
	//alCheck(alSourcefv(_handle, AL_DIRECTION, direction3));
	float orientation6[] = {-direction3[0], -direction3[1], -direction3[2], up3[0], up3[1], up3[2]};
	alCheck(alSourcefv(_handle, AL_ORIENTATION, orientation6));
}

void source_impl::set_volume_rolloff(float rolloff)
{
	alCheck(alSourcef(_handle, AL_ROLLOFF_FACTOR, rolloff));
}

void source_impl::set_distance(float mind, float maxd)
{

    // The distance that the source will be the loudest (if the listener is
    // closer, it won't be any louder than if they were at this distance)
	alCheck(alSourcef(_handle, AL_REFERENCE_DISTANCE, mind));
	
	// The distance that the source will be the quietest (if the listener is
    // farther, it won't be any quieter than if they were at this distance)
	alCheck(alSourcef(_handle, AL_MAX_DISTANCE, maxd));
}

bool source_impl::is_valid() const
{
	return _handle != 0;
}

bool source_impl::is_looping() const
{
	ALint loop;
	alCheck(alGetSourcei(_handle, AL_LOOPING, &loop));
	return loop != 0;
}

source_impl::native_handle_type source_impl::native_handle() const
{
	return _handle;
}

sound_impl::native_handle_type source_impl::binded_handle() const
{
	ALint buffer = 0;
	alCheck(alGetSourcei(_handle, AL_BUFFER, &buffer));
	return sound_impl::native_handle_type(buffer);
}
}
}
