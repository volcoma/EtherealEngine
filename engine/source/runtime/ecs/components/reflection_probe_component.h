#pragma once
//-----------------------------------------------------------------------------
// ReflectionProbeComponent Header Includes
//-----------------------------------------------------------------------------
#include "../ecs.h"
#include "../../rendering/reflection_probe.h"
#include "../../rendering/render_pass.h"
#include "core/common/basetypes.hpp"
//-----------------------------------------------------------------------------
// Forward Declarations
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : ReflectionProbeComponent (Class)
/// <summary>
/// Class that contains our core Light data, used for rendering and other things.
/// </summary>
//-----------------------------------------------------------------------------
class ReflectionProbeComponent : public runtime::Component
{
	COMPONENT(ReflectionProbeComponent)
	SERIALIZABLE(ReflectionProbeComponent)
	REFLECTABLE(ReflectionProbeComponent, runtime::Component)
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	ReflectionProbeComponent();
	ReflectionProbeComponent(const ReflectionProbeComponent& camera);
	virtual ~ReflectionProbeComponent();

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
	inline const ReflectionProbe& get_probe() const { return _probe; }

	//-----------------------------------------------------------------------------
	//  Name : set_probe ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_probe(const ReflectionProbe& probe);

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
		const math::vec3& position,
		const math::transform_t& view,
		const math::transform_t& proj);

	//-----------------------------------------------------------------------------
	//  Name : get_render_view ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline RenderView& get_render_view(size_t idx) { return _render_view[idx]; }

	//-----------------------------------------------------------------------------
	//  Name : get_cubemap ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<Texture> get_cubemap();

	//-----------------------------------------------------------------------------
	//  Name : get_cubemap_fbo ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	std::shared_ptr<FrameBuffer> get_cubemap_fbo();
private:
	//-------------------------------------------------------------------------
	// Private Member Variables.
	//-------------------------------------------------------------------------
	/// The probe object this component represents
	ReflectionProbe _probe;
	/// The render view for this component
	RenderView _render_view[6];
};