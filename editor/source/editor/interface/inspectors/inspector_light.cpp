#include "inspector_light.h"
#include "inspectors.h"

bool Inspector_LightComponent::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<LightComponent*>();
	auto& light = data->get_light();
	bool changed = false;
	{
		PropertyLayout propName("Depth Impl");
		rttr::variant v = light.depth_impl;

		changed |= inspect_var(v);
		if (changed)
			light.depth_impl = v.get_value<DepthImpl>();
	}
	{
		PropertyLayout propName("Shadows Impl");
		rttr::variant v = light.sm_impl;

		changed |= inspect_var(v);
		if (changed)
			light.sm_impl = v.get_value<SmImpl>();
	}
	{
		PropertyLayout propName("Light Impl");
		rttr::variant v = light.light_type;
		
		changed |= inspect_var(v);
		if (changed)
			light.light_type = v.get_value<LightType>();
	}
	if (light.light_type == LightType::Spot)
	{
		rttr::variant v = light.spot_data;
		changed |= inspect_var(v);
		if (changed)
			light.spot_data = v.get_value<Light::Spot>();
	}
	else if (light.light_type == LightType::Point)
	{
		rttr::variant v = light.point_data;
		changed |= inspect_var(v);
		if (changed)
			light.point_data = v.get_value<Light::Point>();
	}
	else if (light.light_type == LightType::Directional)
	{
		rttr::variant v = light.directional_data;
		changed |= inspect_var(v);
		if (changed)
			light.directional_data = v.get_value<Light::Directional>();
	}

	if (changed)
	{
		var = data;
		return true;
	}

	return false;
}