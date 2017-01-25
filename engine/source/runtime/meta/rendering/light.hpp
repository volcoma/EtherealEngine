#pragma once

#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../rendering/light.h"

REFLECT(Light)
{
	rttr::registration::class_<Light::Spot>("Spot")
		.property("Range",
			&Light::Spot::get_range,
			&Light::Spot::set_range)
		(
			rttr::metadata("Min", 0.1f)
		)
		.property("Inner Angle",
			&Light::Spot::get_inner_angle,
			&Light::Spot::set_inner_angle)
		(
			rttr::metadata("Min", 1.0f),
			rttr::metadata("Max", 85.0f),
			rttr::metadata("Step", 0.1f)
		)
		.property("Outer Angle",
			&Light::Spot::get_outer_angle,
			&Light::Spot::set_outer_angle)
		(
			rttr::metadata("Min", 1.0f),
			rttr::metadata("Max", 90.0f),
			rttr::metadata("Step", 0.1f)
		);

	rttr::registration::class_<Light::Point>("Point")
		.property("Range", &Light::Point::range)
		(
			rttr::metadata("Min", 0.1f)
		)
		.property("Exponent Falloff", &Light::Point::exponent_falloff)
		(
			rttr::metadata("Min", 0.1f),
			rttr::metadata("Max", 10.0f)
		)
		.property("FovX adjust", &Light::Point::fov_x_adjust)
		(
			rttr::metadata("Min", -20.0f),
			rttr::metadata("Max", 20.0f),
			rttr::metadata("Step", 0.0001f)
		)
		.property("FovY adjust", &Light::Point::fov_y_adjust)
		(
			rttr::metadata("Min", -20.0f),
			rttr::metadata("Max", 20.0f),
			rttr::metadata("Step", 0.0001f)
		)
		.property("Stencil Pack", &Light::Point::stencil_pack);

	rttr::registration::class_<Light::Directional>("Directional")
		.property("Splits", &Light::Directional::num_splits)
		(
			rttr::metadata("Min", 1),
			rttr::metadata("Max", 4)
			)
		.property("Distribution", &Light::Directional::split_distribution)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f),
			rttr::metadata("Step", 0.001f)
			)
		.property("Stabilize", &Light::Directional::stabilize);

	rttr::registration::enumeration<LightType>("LightType")
		(
			rttr::value("Spot", LightType::Spot),
			rttr::value("Point", LightType::Point),
			rttr::value("Directional", LightType::Directional)
			);
	rttr::registration::enumeration<DepthImpl>("DepthImpl")
		(
			rttr::value("InvZ", DepthImpl::InvZ),
			rttr::value("Linear", DepthImpl::Linear)
			);
	rttr::registration::enumeration<SmImpl>("SmImpl")
		(
			rttr::value("Hard", SmImpl::Hard),
			rttr::value("PCF", SmImpl::PCF),
			rttr::value("VSM", SmImpl::VSM),
			rttr::value("ESM", SmImpl::ESM)
			);
	rttr::registration::class_<Light>("Light")
		.property("Color", &Light::color)
		.property("Intensity", &Light::intensity)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 10.0f)
		)
		.property("Type", &Light::light_type)
		.property("Shadows", &Light::sm_impl)
		.property("Depth", &Light::depth_impl)
		;
}

SAVE(Light)
{
	try_save(ar, cereal::make_nvp("light_type", obj.light_type));
	try_save(ar, cereal::make_nvp("depth_impl", obj.depth_impl));
	try_save(ar, cereal::make_nvp("sm_impl", obj.sm_impl));
	try_save(ar, cereal::make_nvp("spot_range", obj.spot_data.range));
	try_save(ar, cereal::make_nvp("spot_inner_angle", obj.spot_data.inner_angle));
	try_save(ar, cereal::make_nvp("spot_outer_angle", obj.spot_data.outer_angle));
	try_save(ar, cereal::make_nvp("point_range", obj.point_data.range));
	try_save(ar, cereal::make_nvp("point_exponent_falloff", obj.point_data.exponent_falloff));
	try_save(ar, cereal::make_nvp("point_fov_x_adjust", obj.point_data.fov_x_adjust));
	try_save(ar, cereal::make_nvp("point_fov_y_adjust", obj.point_data.fov_y_adjust));
	try_save(ar, cereal::make_nvp("point_stencil_pack", obj.point_data.stencil_pack));
	try_save(ar, cereal::make_nvp("dir_num_splits", obj.directional_data.num_splits));
	try_save(ar, cereal::make_nvp("dir_split_distribution", obj.directional_data.split_distribution));
	try_save(ar, cereal::make_nvp("dir_stabilize", obj.directional_data.stabilize));
	try_save(ar, cereal::make_nvp("color", obj.color));
}

LOAD(Light)
{
	try_load(ar, cereal::make_nvp("light_type", obj.light_type));
	try_load(ar, cereal::make_nvp("depth_impl", obj.depth_impl));
	try_load(ar, cereal::make_nvp("sm_impl", obj.sm_impl));
	try_load(ar, cereal::make_nvp("spot_range", obj.spot_data.range));
	try_load(ar, cereal::make_nvp("spot_inner_angle", obj.spot_data.inner_angle));
	try_load(ar, cereal::make_nvp("spot_outer_angle", obj.spot_data.outer_angle));
	try_load(ar, cereal::make_nvp("point_range", obj.point_data.range));
	try_load(ar, cereal::make_nvp("point_exponent_falloff", obj.point_data.exponent_falloff));
	try_load(ar, cereal::make_nvp("point_fov_x_adjust", obj.point_data.fov_x_adjust));
	try_load(ar, cereal::make_nvp("point_fov_y_adjust", obj.point_data.fov_y_adjust));
	try_load(ar, cereal::make_nvp("point_stencil_pack", obj.point_data.stencil_pack));
	try_load(ar, cereal::make_nvp("dir_num_splits", obj.directional_data.num_splits));
	try_load(ar, cereal::make_nvp("dir_split_distribution", obj.directional_data.split_distribution));
	try_load(ar, cereal::make_nvp("dir_stabilize", obj.directional_data.stabilize));
	try_load(ar, cereal::make_nvp("color", obj.color));
}