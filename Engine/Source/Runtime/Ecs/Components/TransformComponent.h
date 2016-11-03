#pragma once

#include "../entityx/Component.h"
#include "Core/math/math_includes.h"

using namespace entityx;
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

	using HTransformComponent = ComponentHandle<TransformComponent>;
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	TransformComponent();
	TransformComponent(const TransformComponent& rhs);
	~TransformComponent();


	//-------------------------------------------------------------------------
	// Public Static Methods
	//-------------------------------------------------------------------------
	void resolveTransform(bool force = false, float dt = 0.0f);
	virtual bool isDirty() const;
	virtual void onEntitySet();
	const math::transform& getLocalTransform() const;
	const math::transform& getTransform();
	const math::vec3& getPosition();
	math::quat getRotation();
	math::vec3 getXAxis();
	math::vec3 getYAxis();
	math::vec3 getZAxis();
	math::vec3 getScale();

	const math::vec3& getLocalPosition();
	math::quat getLocalRotation();
	math::vec3 getLocalXAxis();
	math::vec3 getLocalYAxis();
	math::vec3 getLocalZAxis();
	math::vec3 getLocalScale();

	TransformComponent& lookAt(float x, float y, float z);
	TransformComponent& lookAt(const math::vec3 & point);
	// Transformations
	virtual TransformComponent& setPosition(const math::vec3 & position);
	virtual TransformComponent& setLocalPosition(const math::vec3 & position);


	virtual TransformComponent& move(const math::vec3 & amount);
	virtual TransformComponent& moveLocal(const math::vec3 & amount);
	virtual TransformComponent& rotate(float x, float y, float z);
	virtual TransformComponent& rotate(float x, float y, float z, const math::vec3 & center);
	virtual TransformComponent& rotateLocal(float x, float y, float z);
	virtual TransformComponent& rotateAxis(float degrees, const math::vec3 & axis);
	virtual TransformComponent& setScale(const math::vec3 & s);
	virtual TransformComponent& setLocalScale(const math::vec3 & scale);
	virtual TransformComponent& setRotation(const math::quat & rotation);
	virtual TransformComponent& setLocalRotation(const math::quat & rotation);
	virtual TransformComponent& resetRotation();
	virtual TransformComponent& resetScale();
	virtual TransformComponent& resetLocalRotation();
	virtual TransformComponent& resetLocalScale();
	virtual TransformComponent& resetPivot();
	virtual TransformComponent& setLocalTransform(const math::transform & trans);
	virtual TransformComponent& setTransform(const math::transform & trans);
	virtual TransformComponent& lookAt(const math::vec3 & eye, const math::vec3 & at);
	virtual TransformComponent& lookAt(const math::vec3 & eye, const math::vec3 & at, const math::vec3 & up);

	virtual bool canAdjustPivot() const;
	virtual bool canScale() const;
	virtual bool canRotate() const;
	virtual TransformComponent& setParent(HTransformComponent parent, bool worldPositionStays, bool localPositionStays);
	virtual TransformComponent& setParent(HTransformComponent parent);
	const HTransformComponent& getParent() const;
	const std::vector<HTransformComponent>& getChildren() const;

	std::uint32_t getHierarchyLevel() { return mHierarchyLevel; }
	void attachChild(HTransformComponent child);
	void removeChild(HTransformComponent child);
	bool getSlowParenting() const { return mSlowParenting; }
	void setSlowParenting(bool val) { mSlowParenting = val; }
	float getSlowParentingSpeed() const { return mSlowParentingSpeed; }
	void setSlowParentingSpeed(float val) { mSlowParentingSpeed = val; }
protected:
	//-------------------------------------------------------------------------
	// Protected Member Variables
	//-------------------------------------------------------------------------

	// Parent object.
	HTransformComponent	mParent;

	// Children object.
	std::vector<HTransformComponent> mChildren;

	// Hierarchy depth.
	std::uint32_t mHierarchyLevel = 0;

	// Transformations
	math::transform mLocalTransform;			// Local transformation relative to the parent
	math::transform mWorldTransform;			// Cached world math::transform at pivot point.
	
	bool mSlowParenting = false;
	float mSlowParentingSpeed = 5.0f;
};