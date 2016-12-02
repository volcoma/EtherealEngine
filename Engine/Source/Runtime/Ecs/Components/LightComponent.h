#pragma once
//-----------------------------------------------------------------------------
// LightComponent Header Includes
//-----------------------------------------------------------------------------
#include "../entityx/Component.h"
#include "../../Rendering/Light.h"
//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

using namespace entityx;
//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : LightComponent (Class)
/// <summary>
/// Class that contains our core Light data, used for rendering and other things.
/// </summary>
//-----------------------------------------------------------------------------
class LightComponent : public Component
{
	COMPONENT(LightComponent)
	SERIALIZABLE(LightComponent)
	REFLECTABLE(LightComponent, Component)
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	LightComponent();
	LightComponent(const LightComponent& camera);
	virtual ~LightComponent();

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	inline Light& getLight() { return mLight; }
	inline const Light& getLight() const { return mLight; }
private:
	//-------------------------------------------------------------------------
	// Private Member Variables.
	//-------------------------------------------------------------------------
	/// The light object this component represents
	Light mLight;
};