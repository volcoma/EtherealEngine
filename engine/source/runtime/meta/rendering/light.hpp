#pragma once

#include "core/serialization/serialization.h"
#include "core/reflection/reflection.h"
#include "core/logging/logging.h"
#include "../../rendering/light.h"

REFLECT(Light)
{
	rttr::registration::class_<Light::Spot>("Spot")
		.property("Range", &Light::Spot::range)
		(
			rttr::metadata("Min", 0.1f)
		)
		.property("Inner Angle", &Light::Spot::spot_inner_angle)
		(
			rttr::metadata("Min", 1.0f),
			rttr::metadata("Max", 85.0f),
			rttr::metadata("Step", 0.1f)
		)
		.property("Outer Angle", &Light::Spot::spot_outer_angle)
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
			rttr::metadata("Min", 0.1f)
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
		.property("Type", &Light::light_type)
		.property("Shadows", &Light::sm_impl)
		.property("Depth", &Light::depth_impl)
		;
}

SAVE(Light)
{

}

LOAD(Light)
{

}