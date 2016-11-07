#include "ModelComponent.h"

ModelComponent::ModelComponent()
{

}

ModelComponent::ModelComponent(const ModelComponent& component)
	: mModel(component.mModel)
	, mStatic(component.mStatic)
	, mCastShadow(component.mCastShadow)
	, mCastReflection(component.mCastReflection)
{
}

ModelComponent::~ModelComponent()
{
}

ModelComponent& ModelComponent::setCastShadow(bool castShadow)
{
	if (mCastShadow == castShadow)
		return *this;

	static const std::string strContext = "CastsShadow";
	touch(strContext);
	mCastShadow = castShadow;
	return *this;
}

ModelComponent& ModelComponent::setStatic(bool bStatic)
{
	if (mStatic == bStatic)
		return *this;

	static const std::string strContext = "Static";
	touch(strContext);
	mStatic = bStatic;
	return *this;
}

ModelComponent& ModelComponent::setCastReflelction(bool castReflection)
{
	if (mCastReflection == castReflection)
		return *this;

	static const std::string strContext = "CastReflection";
	touch(strContext);
	mCastReflection = castReflection;
	return *this;
}

bool ModelComponent::castsShadow() const
{
	return mCastShadow;
}

bool ModelComponent::isStatic() const
{
	return mStatic;
}

const Model& ModelComponent::getModel() const
{
	return mModel;
}

ModelComponent& ModelComponent::setModel(const Model& model)
{
	mModel = model;

	static const std::string strContext = "ModelChange";
	touch(strContext);
	return *this;
}

bool ModelComponent::castsReflection() const
{
	return mCastReflection;
}

