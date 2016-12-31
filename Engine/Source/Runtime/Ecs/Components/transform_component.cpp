#include "transform_component.h"
#include "core/logging/logging.h"
#include <algorithm>

core::CHandle<TransformComponent> createFromComponent(core::CHandle<TransformComponent> component)
{
	if (!component.expired())
	{
		auto ecs = core::get_subsystem<core::EntityComponentSystem>();
		auto entity = ecs->create_from_copy(component.lock()->getEntity());
		return entity.component<TransformComponent>();
	}
	logging::get("Log")->error("trying to clone a null component");
	return core::CHandle<TransformComponent>();
}

TransformComponent::TransformComponent()
{
}

TransformComponent::TransformComponent(const TransformComponent& rhs)
{
	//mParent = rhs.mParent;
	mChildren.reserve(rhs.mChildren.size());
	for (auto& rhsChild : rhs.mChildren)
	{
		mChildren.push_back(createFromComponent(rhsChild));
	}

	mLocalTransform = rhs.mLocalTransform;
	mWorldTransform = rhs.mWorldTransform;
	mSlowParenting = rhs.mSlowParenting;
	mSlowParentingSpeed = rhs.mSlowParentingSpeed;
}

void TransformComponent::onEntitySet()
{
	// 	if (!mParent.expired())
	// 	{
	// 		mParent.lock()->attachChild(makeHandle());
	// 	}

	for (auto& child : mChildren)
	{
		child.lock()->mParent = handle();
	}
}

TransformComponent::~TransformComponent()
{
	if (!mParent.expired())
	{
		if (getEntity())
			mParent.lock()->removeChild(handle());
	}
	for (auto& child : mChildren)
	{
		if (!child.expired())
			child.lock()->getEntity().destroy();
	}

}

TransformComponent& TransformComponent::move(const math::vec3 & amount)
{
	math::vec3 vNewPos = getPosition();
	vNewPos += getXAxis() * amount.x;
	vNewPos += getYAxis() * amount.y;
	vNewPos += getZAxis() * amount.z;

	// Pass through to setPosition so that any derived classes need not
	// override the 'move' method in order to catch this position change.
	setPosition(vNewPos);
	return *this;
}

TransformComponent& TransformComponent::moveLocal(const math::vec3 & amount)
{
	math::vec3 vNewPos = getLocalPosition();
	vNewPos += getLocalXAxis() * amount.x;
	vNewPos += getLocalYAxis() * amount.y;
	vNewPos += getLocalZAxis() * amount.z;

	// Pass through to setPosition so that any derived classes need not
	// override the 'move' method in order to catch this position change.
	setLocalPosition(vNewPos);
	return *this;
}

TransformComponent& TransformComponent::setLocalPosition(const math::vec3 & position)
{
	// Set new cell relative position
	mLocalTransform.setPosition(position);
	setLocalTransform(mLocalTransform);
	return *this;
}

TransformComponent& TransformComponent::setPosition(const math::vec3 & position)
{
	// Rotate a copy of the current math::transform_t.
	math::transform_t m = getTransform();
	m.setPosition(position);

	if (!mParent.expired())
	{
		math::transform_t invParentTransform = math::inverse(mParent.lock()->getTransform());
		m = invParentTransform * m;
	}

	setLocalTransform(m);
	return *this;
}

math::vec3 TransformComponent::getLocalScale()
{
	return mLocalTransform.getScale();
}

const math::vec3 & TransformComponent::getLocalPosition()
{
	return mLocalTransform.getPosition();
}

math::quat TransformComponent::getLocalRotation()
{
	return mLocalTransform.getRotation();
}

math::vec3 TransformComponent::getLocalXAxis()
{
	return mLocalTransform.xUnitAxis();
}

math::vec3 TransformComponent::getLocalYAxis()
{
	return mLocalTransform.yUnitAxis();

}

math::vec3 TransformComponent::getLocalZAxis()
{
	return mLocalTransform.zUnitAxis();

}

const math::vec3 & TransformComponent::getPosition()
{
	return getTransform().getPosition();
}

math::quat TransformComponent::getRotation()
{
	return getTransform().getRotation();
}

math::vec3 TransformComponent::getXAxis()
{
	return getTransform().xUnitAxis();
}

math::vec3 TransformComponent::getYAxis()
{
	return getTransform().yUnitAxis();
}

math::vec3 TransformComponent::getZAxis()
{
	return getTransform().zUnitAxis();
}

math::vec3 TransformComponent::getScale()
{
	return getTransform().getScale();
}

const math::transform_t & TransformComponent::getTransform()
{
	resolveTransform();
	return mWorldTransform;
}

const math::transform_t & TransformComponent::getLocalTransform() const
{
	// Return reference to our internal matrix
	return mLocalTransform;
}

TransformComponent& TransformComponent::lookAt(float x, float y, float z)
{
	//TODO("General", "These lookAt methods need to consider the pivot and the currently applied math::transform_t method!!!");
	lookAt(getPosition(), math::vec3(x, y, z));
	return *this;
}

TransformComponent& TransformComponent::lookAt(const math::vec3 & point)
{
	lookAt(getPosition(), point);
	return *this;
}

TransformComponent& TransformComponent::lookAt(const math::vec3 & eye, const math::vec3 & at)
{
	math::transform_t m;
	m.lookAt(eye, at);

	// Update the component position / orientation through the common base method.
	math::vec3 translation;
	math::quat orientation;

	if (m.decompose(orientation, translation))
	{
		setRotation(orientation);
		setPosition(translation);
	}
	return *this;
}

TransformComponent& TransformComponent::lookAt(const math::vec3 & eye, const math::vec3 & at, const math::vec3 & up)
{
	math::transform_t m;
	m.lookAt(eye, at, up);

	// Update the component position / orientation through the common base method.
	math::vec3 translation;
	math::quat orientation;

	if (m.decompose(orientation, translation))
	{
		setRotation(orientation);
		setPosition(translation);
	}
	return *this;
}

TransformComponent& TransformComponent::rotateLocal(float x, float y, float z)
{
	// Do nothing if rotation is disallowed.
	if (!canRotate())
		return *this;

	// No-op?
	if (!x && !y && !z)
		return *this;

	mLocalTransform.rotateLocal(math::radians(x), math::radians(y), math::radians(z));
	setLocalTransform(mLocalTransform);
	return *this;
}

TransformComponent& TransformComponent::rotateAxis(float degrees, const math::vec3 & axis)
{
	// No - op?
	if (!degrees)
		return *this;

	// If rotation is disallowed, only process position change. Otherwise
	// perform full rotation.
	if (!canRotate())
	{
		// Scale the position, but do not allow axes to scale.
		math::vec3 vPos = getPosition();
		math::transform_t t;
		t.rotateAxis(math::radians(degrees), axis);
		t.transformCoord(vPos, vPos);
		setPosition(vPos);

	} // End if !canRotate()
	else
	{
		// Rotate a copy of the current math::transform_t.
		math::transform_t m = getTransform();
		m.rotateAxis(math::radians(degrees), axis);

		if (!mParent.expired())
		{
			math::transform_t invParentTransform = math::inverse(mParent.lock()->getTransform());
			m = invParentTransform * m;
		}

		setLocalTransform(m);

	} // End if canRotate()
	return *this;
}

TransformComponent& TransformComponent::rotate(float x, float y, float z)
{
	// No - op?
	if (!x && !y && !z)
		return *this;

	// If rotation is disallowed, only process position change. Otherwise
	// perform full rotation.
	if (!canRotate())
	{
		// Scale the position, but do not allow axes to scale.
		math::transform_t t;
		math::vec3 position = getPosition();
		t.rotate(math::radians(x), math::radians(y), math::radians(z));
		t.transformCoord(position, position);
		setPosition(position);

	} // End if !canRotate()
	else
	{
		// Scale a copy of the cell math::transform_t
		// Set orientation of new math::transform_t
		math::transform_t m = getTransform();
		m.rotate(math::radians(x), math::radians(y), math::radians(z));

		if (!mParent.expired())
		{
			math::transform_t invParentTransform = math::inverse(mParent.lock()->getTransform());
			m = invParentTransform * m;
		}

		setLocalTransform(m);

	} // End if canRotate()
	return *this;
}

TransformComponent& TransformComponent::rotate(float x, float y, float z, const math::vec3 & center)
{
	// No - op?
	if (!x && !y && !z)
		return *this;

	// If rotation is disallowed, only process position change. Otherwise
	// perform full rotation.
	if (!canRotate())
	{
		// Scale the position, but do not allow axes to scale.
		math::transform_t t;
		math::vec3 position = getPosition() - center;
		t.rotate(math::radians(x), math::radians(y), math::radians(z));
		t.transformCoord(position, position);
		setPosition(position + center);

	} // End if !canRotate()
	else
	{

		// Scale a copy of the cell math::transform_t
		// Set orientation of new math::transform_t
		math::transform_t m = getTransform();
		m.rotate(math::radians(x), math::radians(y), math::radians(z));

		if (!mParent.expired())
		{
			math::transform_t invParentTransform = math::inverse(mParent.lock()->getTransform());
			m = invParentTransform * m;
		}

		setLocalTransform(m);

	} // End if canRotate()
	return *this;
}

TransformComponent& TransformComponent::setScale(const math::vec3& s)
{
	// If scaling is disallowed, only process position change. Otherwise
	// perform full scale.
	if (!canScale())
		return *this;

	// Scale a copy of the cell math::transform_t
	// Set orientation of new math::transform_t
	math::transform_t m = getTransform();
	m.setScale(s);

	if (!mParent.expired())
	{
		math::transform_t invParentTransform = math::inverse(mParent.lock()->getTransform());
		m = invParentTransform * m;
	}

	setLocalTransform(m);
	return *this;
}

TransformComponent& TransformComponent::setLocalScale(const math::vec3 & scale)
{
	// Do nothing if scaling is disallowed.
	if (!canScale())
		return *this;
	mLocalTransform.setScale(scale);
	setLocalTransform(mLocalTransform);
	return *this;
}

TransformComponent& TransformComponent::setRotation(const math::quat & rotation)
{
	// Do nothing if rotation is disallowed.
	if (!canRotate())
		return *this;

	// Set orientation of new math::transform_t
	math::transform_t m = getTransform();
	m.setRotation(rotation);

	if (!mParent.expired())
	{
		math::transform_t invParentTransform = math::inverse(mParent.lock()->getTransform());
		m = invParentTransform * m;
	}

	setLocalTransform(m);
	return *this;
}

TransformComponent& TransformComponent::setLocalRotation(const math::quat & rotation)
{
	// Do nothing if rotation is disallowed.
	if (!canRotate())
		return *this;

	// Set orientation of new math::transform_t
	mLocalTransform.setRotation(rotation);
	setLocalTransform(mLocalTransform);

	return *this;
}

TransformComponent& TransformComponent::resetRotation()
{
	// Do nothing if rotation is disallowed.
	if (!canRotate())
		return *this;
	setRotation(math::quat{});
	return *this;
}

TransformComponent& TransformComponent::resetScale()
{
	// Do nothing if scaling is disallowed.
	if (!canScale())
		return *this;

	setScale(math::vec3{ 1.0f, 1.0f, 1.0f });
	return *this;
}

TransformComponent& TransformComponent::resetLocalRotation()
{
	// Do nothing if rotation is disallowed.
	if (!canRotate())
		return *this;

	setLocalRotation(math::quat{});

	return *this;
}

TransformComponent& TransformComponent::resetLocalScale()
{
	// Do nothing if scaling is disallowed.
	if (!canScale())
		return *this;

	setLocalScale(math::vec3{ 1.0f, 1.0f, 1.0f });

	return *this;
}

TransformComponent& TransformComponent::resetPivot()
{
	// Do nothing if pivot adjustment is disallowed.
	if (!canAdjustPivot())
		return *this;

	return *this;
}

bool TransformComponent::canScale() const
{
	// Default is to allow scaling.
	return true;
}

bool TransformComponent::canRotate() const
{
	// Default is to allow rotation.
	return true;
}

bool TransformComponent::canAdjustPivot() const
{
	// Default is to allow pivot adjustment.
	return true;
}

TransformComponent& TransformComponent::setParent(core::CHandle<TransformComponent> parent)
{
	setParent(parent, true, false);

	return *this;
}

TransformComponent& TransformComponent::setParent(core::CHandle<TransformComponent> parent, bool worldPositionStays, bool localPositionStays)
{
	auto parentPtr = parent.lock().get();
	auto thisParentPtr = mParent.lock().get();
	// Skip if this is a no-op.
	if (thisParentPtr == parentPtr || this == parentPtr)
		return *this;
	if (parentPtr && (parentPtr->mParent.lock().get() == this))
		return *this;
	// Before we do anything, make sure that all pending math::transform_t 
	// operations are resolved (including those applied to our parent).
	math::transform_t cachedWorldTranform;
	if (worldPositionStays)
	{
		resolveTransform(true);
		cachedWorldTranform = getTransform();
	}

	if (!mParent.expired())
	{
		mParent.lock()->removeChild(handle());
	}

	mParent = parent;

	if (!parent.expired())
	{
		auto shParent = parent.lock();
		// We're now attached / detached as required.	
		shParent->attachChild(handle());
	}


	if (worldPositionStays)
	{
		resolveTransform(true);
		setTransform(cachedWorldTranform);
	}
	else
	{
		if (!localPositionStays)
			setLocalTransform(math::transform_t::Identity);
	}


	// Success!
	return *this;
}

const core::CHandle<TransformComponent>& TransformComponent::getParent() const
{
	return mParent;
}

void TransformComponent::attachChild(core::CHandle<TransformComponent> child)
{
	mChildren.push_back(child);
}

void TransformComponent::removeChild(core::CHandle<TransformComponent> child)
{
	mChildren.erase(std::remove_if(std::begin(mChildren), std::end(mChildren),
		[&child](core::CHandle<TransformComponent> other) { return child.lock() == other.lock(); }
	), std::end(mChildren));
}

TransformComponent& TransformComponent::setTransform(const math::transform_t & tr)
{
	if (mWorldTransform.compare(tr, 0.0001f) == 0)
		return *this;

	math::vec3 position, scaling;
	math::quat orientation;
	tr.decompose(scaling, orientation, position);

	math::transform_t m = getTransform();
	m.setScale(scaling);
	m.setRotation(orientation);
	m.setPosition(position);

	if (!mParent.expired())
	{
		math::transform_t invParentTransform = math::inverse(mParent.lock()->getTransform());
		m = invParentTransform * m;
	}

	setLocalTransform(m);
	return *this;
}

TransformComponent& TransformComponent::setLocalTransform(const math::transform_t & trans)
{
	if (mLocalTransform.compare(trans, 0.0001f) == 0)
		return *this;

	static const std::string strContext = "LocalTransform";
	touch(strContext);
	mLocalTransform = trans;
	return *this;
}

void TransformComponent::resolveTransform(bool force, float dt)
{
	bool dirty = isDirty();

	if (force || dirty)
	{
		if (!mParent.expired())
		{
			auto target = mParent.lock()->getTransform() * mLocalTransform;

			if (mSlowParenting)
			{
				float t = math::clamp(mSlowParentingSpeed * dt, 0.0f, 1.0f);
				mWorldTransform.setPosition(math::lerp(mWorldTransform.getPosition(), target.getPosition(), t));
				mWorldTransform.setScale(math::lerp(mWorldTransform.getScale(), target.getScale(), t));
				mWorldTransform.setRotation(math::slerp(mWorldTransform.getRotation(), target.getRotation(), t));
			}
			else
			{
				mWorldTransform = target;
			}
		}
		else
		{
			mWorldTransform = mLocalTransform;
		}
	}

	mDirty = false;
}

bool TransformComponent::isDirty() const
{
	bool bDirty = Component::isDirty();
	if (!bDirty && !mParent.expired())
	{
		bDirty |= mParent.lock()->isDirty();
	}

	return bDirty;
}


const std::vector<core::CHandle<TransformComponent>>& TransformComponent::getChildren() const
{
	return mChildren;
}
