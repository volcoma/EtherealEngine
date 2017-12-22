#include "source_impl.h"
#include "check.h"
#include "repository.h"

namespace audio
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

bool source_impl::is_binded() const
{
	ALint buffer = 0;
	alCheck(alGetSourcei(_handle, AL_BUFFER, &buffer));
	return (buffer != 0);
}

void source_impl::loop(const bool on)
{
	alCheck(alSourcei(_handle, AL_LOOPING, on ? AL_TRUE : AL_FALSE));
}

void source_impl::gain(const float gain)
{
	alCheck(alSourcef(_handle, AL_GAIN, gain));
}

/* pitch, speed stretching */
void source_impl::pitch(const float pitch)
{
	// if pitch == 0.f pitch = 0.0001f;
	alCheck(alSourcef(_handle, AL_PITCH, pitch));
}

void source_impl::position(const float* position3, bool relative)
{
	alCheck(alSourcefv(_handle, AL_POSITION, position3));
	alCheck(alSourcei(_handle, AL_SOURCE_RELATIVE, relative ? AL_TRUE : AL_FALSE));
}

void source_impl::velocity(const float* velocity3)
{
	alCheck(alSourcefv(_handle, AL_VELOCITY, velocity3));
}

void source_impl::direction(const float* direction3)
{
	alCheck(alSourcefv(_handle, AL_DIRECTION, direction3));
}

void source_impl::attenuation(const float rollOff, const float refDistance)
{
	alCheck(alSourcef(_handle, AL_REFERENCE_DISTANCE, refDistance));
	alCheck(alSourcef(_handle, AL_ROLLOFF_FACTOR, rollOff));
}

void source_impl::distance(const float mind, const float maxd)
{
	alCheck(alSourcef(_handle, AL_REFERENCE_DISTANCE, mind));
	alCheck(alSourcef(_handle, AL_REFERENCE_DISTANCE, maxd));
}

bool source_impl::is_valid() const
{
	return _handle != 0;
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
