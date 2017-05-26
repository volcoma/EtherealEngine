#pragma once

#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../rendering/light.h"

REFLECT(light)
{
	rttr::registration::class_<light::spot>("spot")
		.property("range",
			&light::spot::get_range,
			&light::spot::set_range)
		(
			rttr::metadata("pretty_name", "Range"),
			rttr::metadata("Min", 0.1f)
		)
		.property("inner_angle",
			&light::spot::get_inner_angle,
			&light::spot::set_inner_angle)
		(
			rttr::metadata("pretty_name", "Inner Angle"),
			rttr::metadata("Min", 1.0f),
			rttr::metadata("Max", 85.0f),
			rttr::metadata("Step", 0.1f)
		)
		.property("outer_angle",
			&light::spot::get_outer_angle,
			&light::spot::set_outer_angle)
		(
			rttr::metadata("pretty_name", "Outer Angle"),
			rttr::metadata("Min", 1.0f),
			rttr::metadata("Max", 90.0f),
			rttr::metadata("Step", 0.1f)
		);

	rttr::registration::class_<light::point>("point")
		.property("range", &light::point::range)
		(
			rttr::metadata("pretty_name", "Range"),
			rttr::metadata("Min", 0.1f)
		)
		.property("exponent_falloff", &light::point::exponent_falloff)
		(
			rttr::metadata("pretty_name", "Exponent Falloff"),
			rttr::metadata("Min", 0.1f),
			rttr::metadata("Max", 10.0f)
		)
		.property("fovx_adjust", &light::point::fov_x_adjust)
		(
			rttr::metadata("pretty_name", "FovX Adjust"),
			rttr::metadata("Min", -20.0f),
			rttr::metadata("Max", 20.0f),
			rttr::metadata("Step", 0.0001f)
		)
		.property("fovy_adjust", &light::point::fov_y_adjust)
		(
			rttr::metadata("pretty_name", "FovY Adjust"),
			rttr::metadata("Min", -20.0f),
			rttr::metadata("Max", 20.0f),
			rttr::metadata("Step", 0.0001f)
		)
		.property("stencil_pack", &light::point::stencil_pack)
		(
			rttr::metadata("pretty_name", "Stencil Pack")
		);

	rttr::registration::class_<light::directional>("directional")
		.property("splits", &light::directional::num_splits)
		(
			rttr::metadata("pretty_name", "Splits"),
			rttr::metadata("Min", 1),
			rttr::metadata("Max", 4)
		)
		.property("distribution", &light::directional::split_distribution)
		(
			rttr::metadata("pretty_name", "Distribution"),
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 1.0f),
			rttr::metadata("Step", 0.001f)
		)
		.property("stabilize", &light::directional::stabilize)
		(
			rttr::metadata("pretty_name", "Stabilize")
		)
		;

	rttr::registration::enumeration<light_type>("light_type")
		(
			rttr::value("spot", light_type::spot),
			rttr::value("point", light_type::point),
			rttr::value("directional", light_type::directional)
		);
	rttr::registration::enumeration<depth_impl>("depth_impl")
		(
			rttr::value("invz", depth_impl::invz),
			rttr::value("linear", depth_impl::linear)
		);
	rttr::registration::enumeration<sm_impl>("sm_impl")
		(
			rttr::value("hard", sm_impl::hard),
			rttr::value("pcf", sm_impl::pcf),
			rttr::value("vsm", sm_impl::vsm),
			rttr::value("esm", sm_impl::esm)
		);
	rttr::registration::class_<light>("light")
		.property("color", &light::color)
		(
			rttr::metadata("pretty_name", "Color")
		)
		.property("intensity", &light::intensity)
		(
			rttr::metadata("pretty_name", "Intensity"),
			rttr::metadata("Min", 0.0f),
			rttr::metadata("Max", 10.0f)
		)
		.property("light_type", &light::light_type)
		(
			rttr::metadata("pretty_name", "Light Type")
		)
		.property("sm_impl", &light::sm_impl)
		(
			rttr::metadata("pretty_name", "Shadow Impl")
		)
		.property("depth_impl", &light::depth_impl)
		(
			rttr::metadata("pretty_name", "Depth Impl")
		)
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