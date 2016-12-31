#pragma once

#include "core/ecs.h"
#include "../../rendering/model.h"

class Material;
//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : ModelComponent (Class)
/// <summary>
/// Class that contains core data for meshes.
/// </summary>
//-----------------------------------------------------------------------------
class ModelComponent : public core::Component
{
	COMPONENT(ModelComponent)
	SERIALIZABLE(ModelComponent)
	REFLECTABLE(ModelComponent, Component)

public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	ModelComponent();
	ModelComponent(const ModelComponent& component);
	virtual ~ModelComponent();

	//-------------------------------------------------------------------------
	// Public Virtual Methods (Override)

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	ModelComponent& setCastShadow(bool castShadow);
	ModelComponent& setCastReflelction(bool castReflection);
	ModelComponent& setStatic(bool bStatic);

	bool castsShadow() const;
	bool castsReflection() const;
	bool isStatic() const;

	const Model& getModel() const;
	ModelComponent& setModel(const Model& model);

private:
	//-------------------------------------------------------------------------
	// Private Member Variables.
	//-------------------------------------------------------------------------

	bool mStatic = true;
	bool mCastShadow = true;
	bool mCastReflection = true;
	Model mModel;
};
