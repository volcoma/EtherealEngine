#pragma once

#include "../ecs.h"
#include "core/math/math_includes.h"

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : transform_component (Class)
/// <summary>
/// Class containing transformation data and functionality. It represents
/// an object's state in a 3D setup. Provides functionality for manipulating that state.
/// </summary>
//-----------------------------------------------------------------------------
class transform_component : public runtime::component_impl<transform_component>
{
	SERIALIZABLE(transform_component)
	REFLECTABLE(transform_component, runtime::component)

public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : transform_component ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_component();

	//-----------------------------------------------------------------------------
	//  Name : transform_component ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_component(const transform_component& rhs);

	//-----------------------------------------------------------------------------
	//  Name : ~transform_component ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~transform_component();


	//-------------------------------------------------------------------------
	// Public Static Methods
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : resolve ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void resolve(bool force = false, float dt = 0.0f);

	//-----------------------------------------------------------------------------
	//  Name : is_dirty (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool is_dirty() const;

	//-----------------------------------------------------------------------------
	//  Name : on_entity_set (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void on_entity_set();

	//-----------------------------------------------------------------------------
	//  Name : get_local_transform ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform& get_local_transform() const;

	//-----------------------------------------------------------------------------
	//  Name : get_transform ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform& get_transform();

	//-----------------------------------------------------------------------------
	//  Name : get_position ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::vec3& get_position();

	//-----------------------------------------------------------------------------
	//  Name : get_rotation ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::quat get_rotation();

	//-----------------------------------------------------------------------------
	//  Name : get_x_axis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 get_x_axis();

	//-----------------------------------------------------------------------------
	//  Name : get_y_axis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 get_y_axis();

	//-----------------------------------------------------------------------------
	//  Name : get_z_axis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 get_z_axis();

	//-----------------------------------------------------------------------------
	//  Name : get_scale()
	/// <summary>
	/// Retrieve the current scale of the node along its world space axes.
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 get_scale();

	//-----------------------------------------------------------------------------
	//  Name : get_local_position ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::vec3& get_local_position();

	//-----------------------------------------------------------------------------
	//  Name : get_local_rotation ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::quat get_local_rotation();

	//-----------------------------------------------------------------------------
	//  Name : get_local_x_axis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 get_local_x_axis();

	//-----------------------------------------------------------------------------
	//  Name : get_local_y_axis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 get_local_y_axis();

	//-----------------------------------------------------------------------------
	//  Name : get_local_z_axis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 get_local_z_axis();

	//-----------------------------------------------------------------------------
	//  Name : get_local_scale ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 get_local_scale();

	//-----------------------------------------------------------------------------
	//  Name : look_at (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_component& look_at(float x, float y, float z);

	//-----------------------------------------------------------------------------
	//  Name : look_at (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_component& look_at(const math::vec3 & point);

	//-----------------------------------------------------------------------------
	//  Name : set_position()
	/// <summary>
	/// Set the current world space position of the node.
	/// Note : This bypasses the physics system, so should really only be used
	/// for initialization purposes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& set_position(const math::vec3 & position);

	//-----------------------------------------------------------------------------
	//  Name : set_local_position()
	/// <summary>
	/// Set the current local space position of the node.
	/// Note : This bypasses the physics system, so should really only be used
	/// for initialization purposes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& set_local_position(const math::vec3 & position);

	//-----------------------------------------------------------------------------
	//  Name : move()
	/// <summary>
	/// Move the current position of the node by the specified amount.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& move(const math::vec3 & amount);

	//-----------------------------------------------------------------------------
	//  Name : move_local()
	/// <summary>
	/// Move the current position of the node by the specified amount relative to
	/// its own local axes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& move_local(const math::vec3 & amount);

	//-----------------------------------------------------------------------------
	//  Name : rotate (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& rotate(float x, float y, float z);

	//-----------------------------------------------------------------------------
	//  Name : rotate (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& rotate(float x, float y, float z, const math::vec3 & center);

	//-----------------------------------------------------------------------------
	//  Name : rotate_local (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& rotate_local(float x, float y, float z);

	//-----------------------------------------------------------------------------
	//  Name : rotate_axis (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& rotate_axis(float degrees, const math::vec3 & axis);

	//-----------------------------------------------------------------------------
	//  Name : set_scale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& set_scale(const math::vec3 & s);

	//-----------------------------------------------------------------------------
	//  Name : set_local_scale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& set_local_scale(const math::vec3 & scale);

	//-----------------------------------------------------------------------------
	//  Name : set_rotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& set_rotation(const math::quat & rotation);

	//-----------------------------------------------------------------------------
	//  Name : set_local_rotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& set_local_rotation(const math::quat & rotation);

	//-----------------------------------------------------------------------------
	//  Name : reset_rotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& reset_rotation();

	//-----------------------------------------------------------------------------
	//  Name : reset_scale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& reset_scale();

	//-----------------------------------------------------------------------------
	//  Name : reset_local_rotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& reset_local_rotation();

	//-----------------------------------------------------------------------------
	//  Name : reset_local_scale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& reset_local_scale();

	//-----------------------------------------------------------------------------
	//  Name : reset_pivot (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& reset_pivot();

	//-----------------------------------------------------------------------------
	//  Name : set_local_transform (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& set_local_transform(const math::transform & trans);

	//-----------------------------------------------------------------------------
	//  Name : set_transform (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& set_transform(const math::transform & trans);

	//-----------------------------------------------------------------------------
	//  Name : look_at (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& look_at(const math::vec3 & eye, const math::vec3 & at);

	//-----------------------------------------------------------------------------
	//  Name : look_at (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& look_at(const math::vec3 & eye, const math::vec3 & at, const math::vec3 & up);

	//-----------------------------------------------------------------------------
	//  Name : can_adjust_pivot (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool can_adjust_pivot() const;

	//-----------------------------------------------------------------------------
	//  Name : can_scale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool can_scale() const;

	//-----------------------------------------------------------------------------
	//  Name : can_rotate (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool can_rotate() const;

	//-----------------------------------------------------------------------------
	//  Name : set_parent (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& set_parent(runtime::chandle<transform_component> parent, bool world_position_stays, bool local_position_stays);

	//-----------------------------------------------------------------------------
	//  Name : set_parent (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual transform_component& set_parent(runtime::chandle<transform_component> parent);

	//-----------------------------------------------------------------------------
	//  Name : get_parent ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const runtime::chandle<transform_component>& get_parent() const;

	//-----------------------------------------------------------------------------
	//  Name : get_children ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<runtime::chandle<transform_component>>& get_children() const;

	//-----------------------------------------------------------------------------
	//  Name : attach_child ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void attach_child(runtime::chandle<transform_component> child);

	//-----------------------------------------------------------------------------
	//  Name : remove_child ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void remove_child(runtime::chandle<transform_component> child);

    //-----------------------------------------------------------------------------
    //  Name : cleanup_dead_children ()
    /// <summary>
    ///
    ///
    ///
    /// </summary>
    //-----------------------------------------------------------------------------
    void cleanup_dead_children();

	//-----------------------------------------------------------------------------
	//  Name : get_slow_parenting ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool get_slow_parenting() const { return _slow_parenting; }

	//-----------------------------------------------------------------------------
	//  Name : set_slow_parenting ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_slow_parenting(bool val) { _slow_parenting = val; }

	//-----------------------------------------------------------------------------
	//  Name : get_slow_parenting_speed ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float get_slow_parenting_speed() const { return _slow_parenting_speed; }

	//-----------------------------------------------------------------------------
	//  Name : setSlowParentingSpeed ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_slow_parenting_speed(float val) { _slow_parenting_speed = val; }
protected:
	//-------------------------------------------------------------------------
	// Protected Member Variables
	//-------------------------------------------------------------------------
	/// Parent object.
	runtime::chandle<transform_component> _parent;
	/// Children object.
	std::vector<runtime::chandle<transform_component>> _children;
	/// Local transformation relative to the parent
	math::transform _local_transform;
	/// Cached world transformation at pivot point.
	math::transform _world_transform;
	/// Is slow parenting enabled?
	bool _slow_parenting = false;
	/// Slow parenting speed.
	float _slow_parenting_speed = 5.0f;
};
