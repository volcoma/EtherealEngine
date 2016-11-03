#include "ModelComponent.h"

//-----------------------------------------------------------------------------
//  Name : ModelComponent ()
/// <summary>
/// 
/// 
/// 
/// </summary>
//-----------------------------------------------------------------------------
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

//-----------------------------------------------------------------------------
//  Name : ~ModelComponent ()
/// <summary>
/// 
/// 
/// 
/// </summary>
//-----------------------------------------------------------------------------
ModelComponent::~ModelComponent()
{
}


//-----------------------------------------------------------------------------
//  Name : setCastShadow ()
/// <summary>
/// 
/// 
/// 
/// </summary>
//-----------------------------------------------------------------------------
ModelComponent& ModelComponent::setCastShadow(bool castShadow)
{
	if (mCastShadow == castShadow)
		return *this;

	static const std::string strContext = "CastsShadow";
	onModified(strContext);
	mCastShadow = castShadow;
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : setStatic ()
/// <summary>
/// 
/// 
/// 
/// </summary>
//-----------------------------------------------------------------------------
ModelComponent& ModelComponent::setStatic(bool bStatic)
{
	if (mStatic == bStatic)
		return *this;

	static const std::string strContext = "Static";
	onModified(strContext);
	mStatic = bStatic;
	return *this;
}


ModelComponent& ModelComponent::setCastReflelction(bool castReflection)
{
	if (mCastReflection == castReflection)
		return *this;

	static const std::string strContext = "CastReflection";
	onModified(strContext);
	mCastReflection = castReflection;
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : castsShadow ()
/// <summary>
/// 
/// 
/// 
/// </summary>
//-----------------------------------------------------------------------------
bool ModelComponent::castsShadow() const
{
	return mCastShadow;
}

//-----------------------------------------------------------------------------
//  Name : isStatic ()
/// <summary>
/// 
/// 
/// 
/// </summary>
//-----------------------------------------------------------------------------
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
	onModified(strContext);
	return *this;
}

bool ModelComponent::castsReflection() const
{
	return mCastReflection;
}

