#pragma once

#include "../ecs.h"
#include "core/audio/listener.h"
#include "core/math/math_includes.h"

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : audio_listener_component (Class)
/// <summary>
/// Class that contains core data for audio listeners.
/// There can only be one instance of it per scene.
/// </summary>
//-----------------------------------------------------------------------------
class audio_listener_component : public runtime::component_impl<audio_listener_component>
{
	SERIALIZABLE(audio_listener_component)
	REFLECTABLEV(audio_listener_component, component)

public:
	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : update ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void update(const math::transform& t);

private:
	//-------------------------------------------------------------------------
	// Private Member Variables.
	//-------------------------------------------------------------------------
	audio::listener _listener;
};
