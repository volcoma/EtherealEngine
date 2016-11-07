#pragma once

#include "../entityx/Component.h"
#include "Core/math/math_includes.h"

using namespace entityx;
class TransformComponent;
using HTransformComponent = ComponentHandle<TransformComponent>;
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
class TransformComponent : public Component
{
	COMPONENT(TransformComponent)
	SERIALIZABLE(TransformComponent)
	REFLECTABLE(TransformComponent, Component)

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
	//  Name : resolveTransform ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void resolveTransform(bool force = false, float dt = 0.0f);

	//-----------------------------------------------------------------------------
	//  Name : isDirty (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool isDirty() const;

	//-----------------------------------------------------------------------------
	//  Name : onEntitySet (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void onEntitySet();

	//-----------------------------------------------------------------------------
	//  Name : getLocalTransform ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform& getLocalTransform() const;

	//-----------------------------------------------------------------------------
	//  Name : getTransform ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform& getTransform();

	//-----------------------------------------------------------------------------
	//  Name : getPosition ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::vec3& getPosition();

	//-----------------------------------------------------------------------------
	//  Name : getRotation ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::quat getRotation();

	//-----------------------------------------------------------------------------
	//  Name : getXAxis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 getXAxis();

	//-----------------------------------------------------------------------------
	//  Name : getYAxis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 getYAxis();

	//-----------------------------------------------------------------------------
	//  Name : getZAxis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 getZAxis();

	//-----------------------------------------------------------------------------
	//  Name : getScale()
	/// <summary>
	/// Retrieve the current scale of the node along its world space axes.
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 getScale();

	//-----------------------------------------------------------------------------
	//  Name : getLocalPosition ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::vec3& getLocalPosition();

	//-----------------------------------------------------------------------------
	//  Name : getLocalRotation ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::quat getLocalRotation();

	//-----------------------------------------------------------------------------
	//  Name : getLocalXAxis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 getLocalXAxis();

	//-----------------------------------------------------------------------------
	//  Name : getLocalYAxis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 getLocalYAxis();

	//-----------------------------------------------------------------------------
	//  Name : getLocalZAxis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 getLocalZAxis();

	//-----------------------------------------------------------------------------
	//  Name : getLocalScale ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 getLocalScale();

	//-----------------------------------------------------------------------------
	//  Name : lookAt (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	TransformComponent& lookAt(float x, float y, float z);

	//-----------------------------------------------------------------------------
	//  Name : lookAt (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	TransformComponent& lookAt(const math::vec3 & point);

	//-----------------------------------------------------------------------------
	//  Name : setPosition()
	/// <summary>
	/// Set the current world space position of the node.
	/// Note : This bypasses the physics system, so should really only be used
	/// for initialization purposes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& setPosition(const math::vec3 & position);

	//-----------------------------------------------------------------------------
	//  Name : setLocalPosition()
	/// <summary>
	/// Set the current local space position of the node.
	/// Note : This bypasses the physics system, so should really only be used
	/// for initialization purposes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& setLocalPosition(const math::vec3 & position);

	//-----------------------------------------------------------------------------
	//  Name : move()
	/// <summary>
	/// Move the current position of the node by the specified amount.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& move(const math::vec3 & amount);

	//-----------------------------------------------------------------------------
	//  Name : moveLocal()
	/// <summary>
	/// Move the current position of the node by the specified amount relative to
	/// its own local axes.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& moveLocal(const math::vec3 & amount);

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
	//  Name : rotateLocal (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& rotateLocal(float x, float y, float z);

	//-----------------------------------------------------------------------------
	//  Name : rotateAxis (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& rotateAxis(float degrees, const math::vec3 & axis);

	//-----------------------------------------------------------------------------
	//  Name : setScale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& setScale(const math::vec3 & s);

	//-----------------------------------------------------------------------------
	//  Name : setLocalScale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& setLocalScale(const math::vec3 & scale);

	//-----------------------------------------------------------------------------
	//  Name : setRotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& setRotation(const math::quat & rotation);

	//-----------------------------------------------------------------------------
	//  Name : setLocalRotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& setLocalRotation(const math::quat & rotation);

	//-----------------------------------------------------------------------------
	//  Name : resetRotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& resetRotation();

	//-----------------------------------------------------------------------------
	//  Name : resetScale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& resetScale();

	//-----------------------------------------------------------------------------
	//  Name : resetLocalRotation (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& resetLocalRotation();

	//-----------------------------------------------------------------------------
	//  Name : resetLocalScale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& resetLocalScale();

	//-----------------------------------------------------------------------------
	//  Name : resetPivot (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& resetPivot();

	//-----------------------------------------------------------------------------
	//  Name : setLocalTransform (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& setLocalTransform(const math::transform & trans);

	//-----------------------------------------------------------------------------
	//  Name : setTransform (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& setTransform(const math::transform & trans);

	//-----------------------------------------------------------------------------
	//  Name : lookAt (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& lookAt(const math::vec3 & eye, const math::vec3 & at);

	//-----------------------------------------------------------------------------
	//  Name : lookAt (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& lookAt(const math::vec3 & eye, const math::vec3 & at, const math::vec3 & up);

	//-----------------------------------------------------------------------------
	//  Name : canAdjustPivot (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool canAdjustPivot() const;

	//-----------------------------------------------------------------------------
	//  Name : canScale (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool canScale() const;

	//-----------------------------------------------------------------------------
	//  Name : canRotate (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual bool canRotate() const;

	//-----------------------------------------------------------------------------
	//  Name : setParent (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& setParent(HTransformComponent parent, bool worldPositionStays, bool localPositionStays);

	//-----------------------------------------------------------------------------
	//  Name : setParent (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual TransformComponent& setParent(HTransformComponent parent);

	//-----------------------------------------------------------------------------
	//  Name : getParent ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const HTransformComponent& getParent() const;

	//-----------------------------------------------------------------------------
	//  Name : getChildren ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const std::vector<HTransformComponent>& getChildren() const;

	//-----------------------------------------------------------------------------
	//  Name : getHierarchyLevel ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	std::uint32_t getHierarchyLevel() { return mHierarchyLevel; }

	//-----------------------------------------------------------------------------
	//  Name : attachChild ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void attachChild(HTransformComponent child);

	//-----------------------------------------------------------------------------
	//  Name : removeChild ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void removeChild(HTransformComponent child);

	//-----------------------------------------------------------------------------
	//  Name : getSlowParenting ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool getSlowParenting() const { return mSlowParenting; }

	//-----------------------------------------------------------------------------
	//  Name : setSlowParenting ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setSlowParenting(bool val) { mSlowParenting = val; }

	//-----------------------------------------------------------------------------
	//  Name : getSlowParentingSpeed ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float getSlowParentingSpeed() const { return mSlowParentingSpeed; }

	//-----------------------------------------------------------------------------
	//  Name : setSlowParentingSpeed ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setSlowParentingSpeed(float val) { mSlowParentingSpeed = val; }
protected:
	//-------------------------------------------------------------------------
	// Protected Member Variables
	//-------------------------------------------------------------------------
	/// Parent object.
	HTransformComponent	mParent;
	/// Children object.
	std::vector<HTransformComponent> mChildren;
	/// Hierarchy depth.
	std::uint32_t mHierarchyLevel = 0;
	/// Local transformation relative to the parent
	math::transform mLocalTransform;
	/// Cached world transformation at pivot point.
	math::transform mWorldTransform;
	/// Is slow parenting enabled?
	bool mSlowParenting = false;
	/// Slow parenting speed.
	float mSlowParentingSpeed = 5.0f;
};