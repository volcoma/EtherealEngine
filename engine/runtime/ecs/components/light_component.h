#pragma once
//-----------------------------------------------------------------------------
// light_component Header Includes
//-----------------------------------------------------------------------------
#include "../../rendering/light.h"
#include "../ecs.h"
#include "core/common/basetypes.hpp"
//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : light_component (Class)
/// <summary>
/// Class that contains our core light data, used for rendering and other
/// things.
/// </summary>
//-----------------------------------------------------------------------------
class light_component : public runtime::component_impl<light_component>
{
	SERIALIZABLE(light_component)
	REFLECTABLEV(light_component, runtime::component)
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	light_component();
	light_component(const light_component& camera);
	virtual ~light_component();

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
	inline const light& get_light() const
	{
		return _light;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_light ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_light(const light& light)
	{
		_light = light;
	}

	//-----------------------------------------------------------------------------
	//  Name : compute_projected_sphere_rect ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	int compute_projected_sphere_rect(irect& rect, const math::vec3& light_position,
									  const math::vec3& light_direction, const math::transform& view,
									  const math::transform& proj);

private:
	//-------------------------------------------------------------------------
	// Private Member Variables.
	//-------------------------------------------------------------------------
	/// The light object this component represents
	light _light;
};
