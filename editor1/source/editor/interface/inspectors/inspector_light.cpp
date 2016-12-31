#include "inspector_light.h"
#include "inspectors.h"

bool Inspector_LightComponent::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<LightComponent*>();
	auto& light = data->getLight();
	bool changed = false;
	{
		PropertyLayout propName("Depth Impl");
		rttr::variant v = light.depthImpl;

		changed |= inspect_var(v);
		if (changed)
			light.depthImpl = v.get_value<DepthImpl>();
	}
	{
		PropertyLayout propName("Shadows Impl");
		rttr::variant v = light.smImpl;

		changed |= inspect_var(v);
		if (changed)
			light.smImpl = v.get_value<SmImpl>();
	}
	{
		PropertyLayout propName("Light Impl");
		rttr::variant v = light.lightType;
		
		changed |= inspect_var(v);
		if (changed)
			light.lightType = v.get_value<LightType>();
	}
	if (light.lightType == LightType::Spot)
	{
		rttr::variant v = light.spotData;
		changed |= inspect_var(v);
		if (changed)
			light.spotData = v.get_value<Light::Spot>();
	}
	else if (light.lightType == LightType::Point)
	{
		rttr::variant v = light.pointData;
		changed |= inspect_var(v);
		if (changed)
			light.pointData = v.get_value<Light::Point>();
	}
	else if (light.lightType == LightType::Directional)
	{
		rttr::variant v = light.directionalData;
		changed |= inspect_var(v);
		if (changed)
			light.directionalData = v.get_value<Light::Directional>();
	}

	if (changed)
	{
		var = data;
		return true;
	}

	return false;
}