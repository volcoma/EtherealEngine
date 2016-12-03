#pragma once

#include "Core/serialization/serialization.h"
#include "Core/reflection/reflection.h"

#include "../../Rendering/Light.h"

REFLECT(Light)
{
	rttr::registration::class_<Light::Spot>("Spot")
		.property("Range", &Light::Spot::range)
		(
			rttr::metadata("Min", 0.1f)
		)
		.property("Inner Angle", &Light::Spot::spotInnerAngle)
		(
			rttr::metadata("Min", 1.0f),
			rttr::metadata("Max", 85.0f),
			rttr::metadata("Step", 0.1f)
		)
		.property("Outer Angle", &Light::Spot::spotOuterAngle)
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
		.property("FovX adjust", &Light::Point::fovXAdjust)
		(
			rttr::metadata("Min", -20.0f),
			rttr::metadata("Max", 20.0f),
			rttr::metadata("Step", 0.0001f)
		)
		.property("FovY adjust", &Light::Point::fovYAdjust)
		(
			rttr::metadata("Min", -20.0f),
			rttr::metadata("Max", 20.0f),
			rttr::metadata("Step", 0.0001f)
		)
		.property("Stencil Pack", &Light::Point::stencilPack);

	rttr::registration::class_<Light::Directional>("Directional")
		.property("Splits", &Light::Directional::numSplits)
		(
			rttr::metadata("Min", 1),
			rttr::metadata("Max", 4)
			)
		.property("Distribution", &Light::Directional::splitDistribution)
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
	rttr::registration::class_<Light>("Light");
}

SAVE(Light)
{

}

LOAD(Light)
{

}