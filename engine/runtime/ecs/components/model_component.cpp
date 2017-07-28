#include "model_component.h"

model_component::model_component()
{
}

model_component::model_component(const model_component& component)
	: _model(component._model)
	, _static(component._static)
	, _casts_shadow(component._casts_shadow)
	, _casts_reflection(component._casts_reflection)
{
}

model_component::~model_component()
{
}

model_component& model_component::set_casts_shadow(bool castShadow)
{
	if(_casts_shadow == castShadow)
		return *this;

	touch();

	_casts_shadow = castShadow;

	return *this;
}

model_component& model_component::set_static(bool bStatic)
{
	if(_static == bStatic)
		return *this;

	touch();

	_static = bStatic;
	return *this;
}

model_component& model_component::set_casts_reflection(bool castReflection)
{
	if(_casts_reflection == castReflection)
		return *this;

	touch();

	_casts_reflection = castReflection;
	return *this;
}

bool model_component::casts_shadow() const
{
	return _casts_shadow;
}

bool model_component::is_static() const
{
	return _static;
}

const model& model_component::get_model() const
{
	return _model;
}

model_component& model_component::set_model(const model& model)
{
	_model = model;

	touch();

	return *this;
}

bool model_component::casts_reflection() const
{
	return _casts_reflection;
}
