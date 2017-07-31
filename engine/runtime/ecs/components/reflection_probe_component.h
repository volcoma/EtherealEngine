#pragma once
//-----------------------------------------------------------------------------
// reflection_probe_component Header Includes
//-----------------------------------------------------------------------------
#include "../../rendering/reflection_probe.h"
#include "../../rendering/render_pass.h"
#include "../ecs.h"
#include "core/common/basetypes.hpp"
//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : reflection_probe_component (Class)
/// <summary>
/// Class that contains our core light data, used for rendering and other
/// things.
/// </summary>
//-----------------------------------------------------------------------------
class reflection_probe_component : public runtime::component_impl<reflection_probe_component>
{
	SERIALIZABLE(reflection_probe_component)
	REFLECTABLEV(reflection_probe_component, runtime::component)
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	reflection_probe_component();
	reflection_probe_component(const reflection_probe_component& camera);
	virtual ~reflection_probe_component();

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : get_probe ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const reflection_probe& get_probe() const
	{
		return _probe;
	}

	//-----------------------------------------------------------------------------
	//  Name : set_probe ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_probe(const reflection_probe& probe);

	//-----------------------------------------------------------------------------
	//  Name : compute_projected_sphere_rect ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	int compute_projected_sphere_rect(irect& rect, const math::vec3& position, const math::transform& view,
									  const math::transform& proj);

	//-----------------------------------------------------------------------------
	//  Name : get_render_view ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline render_view& get_render_view(size_t idx)
	{
		return _render_view[idx];
	}

	//-----------------------------------------------------------------------------
	//  Name : get_cubemap ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<texture> get_cubemap();

	//-----------------------------------------------------------------------------
	//  Name : get_cubemap_fbo ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<frame_buffer> get_cubemap_fbo();

private:
	//-------------------------------------------------------------------------
	// Private Member Variables.
	//-------------------------------------------------------------------------
	/// The probe object this component represents
	reflection_probe _probe;
	/// The render view for this component
	render_view _render_view[6];
};
