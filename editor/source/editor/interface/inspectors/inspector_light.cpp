#include "inspector_light.h"
#include "inspectors.h"

bool Inspector_LightComponent::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<LightComponent*>();
	auto light = data->get_light();

	rttr::variant light_var = light;
	bool changed = inspect_var(light_var);
	if (changed)
		light = light_var.get_value<Light>();

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
		data->set_light(light);
		var = data;
		return true;
	}

	return false;
}

bool Inspector_ReflectionProbeComponent::inspect(rttr::variant& var, bool readOnly, std::function<rttr::variant(const rttr::variant&)> get_metadata)
{
	auto data = var.get_value<ReflectionProbeComponent*>();
	auto probe = data->get_probe();

	rttr::variant probe_var = probe;
	bool changed = inspect_var(probe_var);
	if (changed)
		probe = probe_var.get_value<ReflectionProbe>();

	if (probe.probe_type == ProbeType::Box)
	{
		rttr::variant v = probe.box_data;
		changed |= inspect_var(v);
		if (changed)
			probe.box_data = v.get_value<ReflectionProbe::Box>();
	}
	else if (probe.probe_type == ProbeType::Sphere)
	{
		rttr::variant v = probe.sphere_data;
		changed |= inspect_var(v);
		if (changed)
			probe.sphere_data = v.get_value<ReflectionProbe::Sphere>();
	}


	if (changed)
	{
		data->set_probe(probe);
		var = data;
		return true;
	}

	return false;
}