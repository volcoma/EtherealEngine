#pragma once

#include "../ecs.h"
#include "core/math/math_includes.h"

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : TransformComponent (Class)
/// <summary>
/// Class containing transformation data and functionality. It represents
/// an object's state in a 3D setup. Provides functionality for manipulating that state.
/// </summary>
//-----------------------------------------------------------------------------
class TransformComponent : public runtime::Component
{
	COMPONENT(TransformComponent)
	SERIALIZABLE(TransformComponent)
	REFLECTABLE(TransformComponent, runtime::Component)

public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : TransformComponent ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	TransformComponent();

	//-----------------------------------------------------------------------------
	//  Name : TransformComponent ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	TransformComponent(const TransformComponent& rhs);

	//-----------------------------------------------------------------------------
	//  Name : ~TransformComponent ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~TransformComponent();


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
	const math::transform_t& get_local_transform() const;

	//-----------------------------------------------------------------------------
	//  Name : get_transform ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform_t& get_transform();

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
	TransformComponent& look_at(float x, float y, float z);

	//-----------------------------------------------------------------------------
	//  Name : look_at (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	TransformComponent& look_at(const math::vec3 & point);

	//-----------------------------------------------------------------------------
	//  Name : set_position()
	/// <summary>
	/// Set the current world space position of the node.
	/// Note : This bypasses the physics system, so should really only be used
	/// for initialization purposes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& set_position(const math::vec3 & position);

	//-----------------------------------------------------------------------------
	//  Name : set_local_position()
	/// <summary>
	/// Set the current local space position of the node.
	/// Note : This bypasses the physics system, so should really only be used
	/// for initialization purposes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& set_local_position(const math::vec3 & position);

	//-----------------------------------------------------------------------------
	//  Name : move()
	/// <summary>
	/// Move the current position of the node by the specified amount.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& move(const math::vec3 & amount);

	//-----------------------------------------------------------------------------
	//  Name : move_local()
	/// <summary>
	/// Move the current position of the node by the specified amount relative to
	/// its own local axes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& move_local(const math::vec3 & amount);

	//-----------------------------------------------------------------------------
	//  Name : rotate (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& rotate(float x, float y, float z);

	//-----------------------------------------------------------------------------
	//  Name : rotate (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& rotate(float x, float y, float z, const math::vec3 & center);

	//-----------------------------------------------------------------------------
	//  Name : rotate_local (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& rotate_local(float x, float y, float z);

	//-----------------------------------------------------------------------------
	//  Name : rotate_axis (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& rotate_axis(float degrees, const math::vec3 & axis);

	//-----------------------------------------------------------------------------
	//  Name : set_scale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& set_scale(const math::vec3 & s);

	//-----------------------------------------------------------------------------
	//  Name : set_local_scale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& set_local_scale(const math::vec3 & scale);

	//-----------------------------------------------------------------------------
	//  Name : set_rotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& set_rotation(const math::quat & rotation);

	//-----------------------------------------------------------------------------
	//  Name : set_local_rotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& set_local_rotation(const math::quat & rotation);

	//-----------------------------------------------------------------------------
	//  Name : reset_rotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& reset_rotation();

	//-----------------------------------------------------------------------------
	//  Name : reset_scale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& reset_scale();

	//-----------------------------------------------------------------------------
	//  Name : reset_local_rotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& reset_local_rotation();

	//-----------------------------------------------------------------------------
	//  Name : reset_local_scale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& reset_local_scale();

	//-----------------------------------------------------------------------------
	//  Name : reset_pivot (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& reset_pivot();

	//-----------------------------------------------------------------------------
	//  Name : set_local_transform (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& set_local_transform(const math::transform_t & trans);

	//-----------------------------------------------------------------------------
	//  Name : set_transform (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& set_transform(const math::transform_t & trans);

	//-----------------------------------------------------------------------------
	//  Name : look_at (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& look_at(const math::vec3 & eye, const math::vec3 & at);

	//-----------------------------------------------------------------------------
	//  Name : look_at (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& look_at(const math::vec3 & eye, const math::vec3 & at, const math::vec3 & up);

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
	virtual TransformComponent& set_parent(runtime::CHandle<TransformComponent> parent, bool world_position_stays, bool local_position_stays);

	//-----------------------------------------------------------------------------
	//  Name : set_parent (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& set_parent(runtime::CHandle<TransformComponent> parent);

	//-----------------------------------------------------------------------------
	//  Name : get_parent ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const runtime::CHandle<TransformComponent>& get_parent() const;

	//-----------------------------------------------------------------------------
	//  Name : get_children ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<runtime::CHandle<TransformComponent>>& get_children() const;

	//-----------------------------------------------------------------------------
	//  Name : attach_child ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void attach_child(runtime::CHandle<TransformComponent> child);

	//-----------------------------------------------------------------------------
	//  Name : remove_child ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void remove_child(runtime::CHandle<TransformComponent> child);

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
	runtime::CHandle<TransformComponent> _parent;
	/// Children object.
	std::vector<runtime::CHandle<TransformComponent>> _children;
	/// Local transformation relative to the parent
	math::transform_t _local_transform;
	/// Cached world transformation at pivot point.
	math::transform_t _world_transform;
	/// Is slow parenting enabled?
	bool _slow_parenting = false;
	/// Slow parenting speed.
	float _slow_parenting_speed = 5.0f;
};