#pragma once
//-----------------------------------------------------------------------------
// LightComponent Header Includes
//-----------------------------------------------------------------------------
#include "../ecs.h"
#include "../../rendering/light.h"
#include "core/common/basetypes.hpp"
//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : LightComponent (Class)
/// <summary>
/// Class that contains our core Light data, used for rendering and other things.
/// </summary>
//-----------------------------------------------------------------------------
class LightComponent : public runtime::Component
{
	COMPONENT(LightComponent)
	SERIALIZABLE(LightComponent)
	REFLECTABLE(LightComponent, runtime::Component)
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
	//-----------------------------------------------------------------------------
	//  Name : get_light ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const Light& get_light() const { return _light; }

	//-----------------------------------------------------------------------------
	//  Name : set_light ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_light(const Light& light) { _light = light; }


	//-----------------------------------------------------------------------------
	//  Name : compute_projected_sphere_rect ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	int compute_projected_sphere_rect(
		iRect& rect,
		const math::vec3& light_position,
		const math::vec3& light_direction,
		const math::transform_t& view,
		const math::transform_t& proj);

private:
	//-------------------------------------------------------------------------
	// Private Member Variables.
	//-------------------------------------------------------------------------
	/// The light object this component represents
	Light _light;
};