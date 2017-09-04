#include "model_component.h"
#include "transform_component.h"


model_component& model_component::set_casts_shadow(bool cast_shadow)
{
	if(_casts_shadow == cast_shadow)
		return *this;

	touch();

	_casts_shadow = cast_shadow;

	return *this;
}

model_component& model_component::set_static(bool is_static)
{
	if(_static == is_static)
		return *this;

	touch();

	_static = is_static;
	return *this;
}

model_component& model_component::set_casts_reflection(bool casts_reflection)
{
	if(_casts_reflection == casts_reflection)
		return *this;

	touch();

	_casts_reflection = casts_reflection;
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

model_component& model_component::set_bone_transforms(const std::vector<math::transform>& bone_transforms)
{
	_bone_transforms = bone_transforms;

	touch();

	return *this;
}

const std::vector<math::transform>& model_component::get_bone_transforms() const
{
	return _bone_transforms;
}

model_component& model_component::set_bone_entities(const std::vector<runtime::entity>& bone_entities)
{
	_bone_entities = bone_entities;

	touch();

	return *this;
}

const std::vector<runtime::entity>& model_component::get_bone_entities() const
{
	return _bone_entities;
}

bool model_component::casts_reflection() const
{
	return _casts_reflection;
}
