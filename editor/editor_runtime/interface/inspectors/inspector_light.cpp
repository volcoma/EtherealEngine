#include "inspector_light.h"
#include "inspectors.h"

bool inspector_light_component::inspect(rttr::variant& var, bool read_only, const meta_getter& get_metadata)
{
	auto data = var.get_value<light_component*>();
	auto light_val = data->get_light();

	rttr::variant light_var = light_val;
	bool changed = inspect_var(light_var);
	if(changed)
		light_val = light_var.get_value<light>();

	if(light_val.type == light_type::spot)
	{
		rttr::variant v = light_val.spot_data;
		changed |= inspect_var(v);
		if(changed)
			light_val.spot_data = v.get_value<light::spot>();
	}
	else if(light_val.type == light_type::point)
	{
		rttr::variant v = light_val.point_data;
		changed |= inspect_var(v);
		if(changed)
			light_val.point_data = v.get_value<light::point>();
	}
	else if(light_val.type == light_type::directional)
	{
		rttr::variant v = light_val.directional_data;
		changed |= inspect_var(v);
		if(changed)
			light_val.directional_data = v.get_value<light::directional>();
	}

	if(changed)
	{
		data->set_light(light_val);
		var = data;
		return true;
	}

	return false;
}

bool inspector_reflection_probe_component::inspect(rttr::variant& var, bool read_only,
												   const meta_getter& get_metadata)
{
	auto data = var.get_value<reflection_probe_component*>();
	auto probe = data->get_probe();

	rttr::variant probe_var = probe;
	bool changed = inspect_var(probe_var);
	if(changed)
		probe = probe_var.get_value<reflection_probe>();

	if(probe.type == probe_type::box)
	{
		rttr::variant v = probe.box_data;
		changed |= inspect_var(v);
		if(changed)
			probe.box_data = v.get_value<reflection_probe::box>();
	}
	else if(probe.type == probe_type::sphere)
	{
		rttr::variant v = probe.sphere_data;
		changed |= inspect_var(v);
		if(changed)
			probe.sphere_data = v.get_value<reflection_probe::sphere>();
	}

	if(changed)
	{
		data->set_probe(probe);
		var = data;
		return true;
	}

	return false;
}
