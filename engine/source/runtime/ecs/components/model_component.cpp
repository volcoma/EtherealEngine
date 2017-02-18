#include "model_component.h"

ModelComponent::ModelComponent()
{

}

ModelComponent::ModelComponent(const ModelComponent& component)
	: _model(component._model)
	, _static(component._static)
	, _casts_shadow(component._casts_shadow)
	, _casts_reflection(component._casts_reflection)
{
}

ModelComponent::~ModelComponent()
{
}

ModelComponent& ModelComponent::set_casts_shadow(bool castShadow)
{
	if (_casts_shadow == castShadow)
		return *this;

	touch();

	_casts_shadow = castShadow;

	return *this;
}

ModelComponent& ModelComponent::set_static(bool bStatic)
{
	if (_static == bStatic)
		return *this;

	touch();

	_static = bStatic;
	return *this;
}

ModelComponent& ModelComponent::set_casts_reflection(bool castReflection)
{
	if (_casts_reflection == castReflection)
		return *this;

	touch();

	_casts_reflection = castReflection;
	return *this;
}

bool ModelComponent::casts_shadow() const
{
	return _casts_shadow;
}

bool ModelComponent::is_static() const
{
	return _static;
}

const Model& ModelComponent::get_model() const
{
	return _model;
}

ModelComponent& ModelComponent::set_model(const Model& model)
{
	_model = model;

	touch();

	return *this;
}

bool ModelComponent::casts_reflection() const
{
	return _casts_reflection;
}

