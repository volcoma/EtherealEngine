#pragma once

#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../rendering/light.h"

REFLECT(light)
{
	rttr::registration::class_<light::spot>("spot")
		.property("Range",
			&light::spot::get_range,
			&light::spot::set_range)
		(
			rttr::metadata("Min", 0.1f)
		)
		.property("Inner Angle",
			&light::spot::get_inner_angle,
			&light::spot::set_inner_angle)
		(
			rttr::metadata("Min", 1.0f),
			rttr::metadata("Max", 85.0f),
			rttr::metadata("Step", 0.1f)
		)
		.property("Outer Angle",
			&light::spot::get_outer_angle,
			&light::spot::set_outer_angle)
		(
			rttr::metadata("Min", 1.0f),
			rttr::metadata("Max", 90.0f),
			rttr::metadata("Step", 0.1f)
		);

	rttr::registration::class_<light::point>("point")
		.property("Range", &light::point::range)
		(
			rttr::metadata("Min", 0.1f)
		)
		.property("Exponent Falloff", &light::point::exponent_falloff)
		(
			rttr::metadata("Min", 0.1f),
			rttr::metadata("Max", 10.0f)
		)
		.property("FovX adjust", &light::point::fov_x_adjust)
		(
			rttr::metadata("Min", -20.0f),
			rttr::metadata("Max", 20.0f),
			rttr::metadata("Step", 0.0001f)
		)
		.property("FovY adjust", &light::point::fov_y_adjust)
		(
			rttr::metadata("Min", -20.0f),
			rttr::metadata("Max", 20.0f),
			rttr::metadata("Step", 0.0001f)
		)
		.property("Stencil Pack", &light::point::stencil_pack);

	rttr::registration::class_<light::directional>("directional")
		.property("Splits", &light::directional::num_splits)
		(
			rttr::metadata("Min", 1),
			rttr::metadata("Max", 4)
			)
		.property("Distribution", &light::directional::split_distribution)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f),
			rttr::metadata("Step", 0.001f)
			)
		.property("Stabilize", &light::directional::stabilize);

	rttr::registration::enumeration<light_type>("light_type")
		(
			rttr::value("Spot", light_type::spot),
			rttr::value("Point", light_type::point),
			rttr::value("Directional", light_type::directional)
			);
	rttr::registration::enumeration<depth_impl>("depth_impl")
		(
			rttr::value("InvZ", depth_impl::InvZ),
			rttr::value("Linear", depth_impl::Linear)
			);
	rttr::registration::enumeration<sm_impl>("sm_impl")
		(
			rttr::value("Hard", sm_impl::Hard),
			rttr::value("PCF", sm_impl::PCF),
			rttr::value("VSM", sm_impl::VSM),
			rttr::value("ESM", sm_impl::ESM)
			);
	rttr::registration::class_<light>("light")
		.property("Color", &light::color)
		.property("Intensity", &light::intensity)
		(
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 10.0f)
		)
		.property("Type", &light::light_type)
		.property("Shadows", &light::sm_impl)
		.property("Depth", &light::depth_impl)
		;
}

SAVE(light)
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
	try_save(ar, cereal::make_nvp("intensity", obj.intensity));
	try_save(ar, cereal::make_nvp("color", obj.color));
}

LOAD(light)
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
	try_load(ar, cereal::make_nvp("intensity", obj.intensity));
	try_load(ar, cereal::make_nvp("color", obj.color));
}