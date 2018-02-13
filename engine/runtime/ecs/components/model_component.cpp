#include "model_component.h"
#include "transform_component.h"

model_component& model_component::set_casts_shadow(bool cast_shadow)
{
	if(casts_shadow_ == cast_shadow)
		return *this;

	touch();

	casts_shadow_ = cast_shadow;

	return *this;
}

model_component& model_component::set_static(bool is_static)
{
	if(static_ == is_static)
		return *this;

	touch();

	static_ = is_static;
	return *this;
}

model_component& model_component::set_casts_reflection(bool casts_reflection)
{
	if(casts_reflection_ == casts_reflection)
		return *this;

	touch();

	casts_reflection_ = casts_reflection;
	return *this;
}

bool model_component::casts_shadow() const
{
	return casts_shadow_;
}

bool model_component::is_static() const
{
	return static_;
}

const model& model_component::get_model() const
{
	return model_;
}

model_component& model_component::set_model(const model& model)
{
	model_ = model;

	touch();

	return *this;
}

model_component& model_component::set_bone_transforms(const std::vector<math::transform>& bone_transforms)
{
	bone_transforms_ = bone_transforms;

	touch();

	return *this;
}

const std::vector<math::transform>& model_component::get_bone_transforms() const
{
	return bone_transforms_;
}

model_component& model_component::set_bone_entities(const std::vector<runtime::entity>& bone_entities)
{
	bone_entities_ = bone_entities;

	touch();

	return *this;
}

const std::vector<runtime::entity>& model_component::get_bone_entities() const
{
	return bone_entities_;
}

bool model_component::casts_reflection() const
{
	return casts_reflection_;
}
