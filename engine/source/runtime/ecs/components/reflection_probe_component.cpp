#include "reflection_probe_component.h"

ReflectionProbeComponent::ReflectionProbeComponent()
{
}

ReflectionProbeComponent::ReflectionProbeComponent(const ReflectionProbeComponent& lightComponent)
{
	_probe = lightComponent._probe;
}

ReflectionProbeComponent::~ReflectionProbeComponent()
{

}

int ReflectionProbeComponent::compute_projected_sphere_rect(iRect& rect, const math::vec3& position, const math::transform_t& view, const math::transform_t& proj)
{
	if (_probe.probe_type == ProbeType::Sphere)
	{
		return math::compute_projected_sphere_rect(
			rect.left,
			rect.right,
			rect.top,
			rect.bottom,
			position,
			_probe.extents.x,
			view,
			proj);
	}
	else if (_probe.probe_type == ProbeType::Box)
	{
		float w2 = math::pow(_probe.extents.x * 2.0f, 2.0f);
		float h2 = math::pow(_probe.extents.y * 2.0f, 2.0f);
		float l2 = math::pow(_probe.extents.z * 2.0f, 2.0f);
		float d2 = w2 + h2 + l2;
		float d = math::sqrt(d2);

		return math::compute_projected_sphere_rect(
			rect.left,
			rect.right,
			rect.top,
			rect.bottom,
			position,
			d,
			view,
			proj);
	}
	else
	{
		return 1;
	}
}
