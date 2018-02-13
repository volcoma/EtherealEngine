#pragma once
//-----------------------------------------------------------------------------
// reflection_probe_component Header Includes
//-----------------------------------------------------------------------------
#include "../../rendering/reflection_probe.h"
#include "../ecs.h"
#include "core/common/basetypes.hpp"
#include "core/graphics/render_pass.h"
#include "core/graphics/render_view.h"
#include <array>
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
		return probe_;
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
	int compute_projected_sphere_rect(irect32_t& rect, const math::vec3& position, const math::transform& view,
									  const math::transform& proj);

	//-----------------------------------------------------------------------------
	//  Name : get_render_view ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline gfx::render_view& get_render_view(size_t idx)
	{
		return render_view_[idx];
	}

	//-----------------------------------------------------------------------------
	//  Name : get_cubemap ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<gfx::texture> get_cubemap();

	//-----------------------------------------------------------------------------
	//  Name : get_cubemap_fbo ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<gfx::frame_buffer> get_cubemap_fbo();

	void update();

private:
	//-------------------------------------------------------------------------
	// Private Member Variables.
	//-------------------------------------------------------------------------
	/// The probe object this component represents
	reflection_probe probe_;
	/// The render view for this component
	std::array<gfx::render_view, 6> render_view_;
};
