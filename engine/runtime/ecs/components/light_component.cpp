#include "light_component.h"

int light_component::compute_projected_sphere_rect(irect32_t& rect, const math::vec3& light_position,
												   const math::vec3& light_direction,
												   const math::transform& view, const math::transform& proj)
{
	if(_light.type == light_type::point)
	{
		return math::compute_projected_sphere_rect(rect.left, rect.right, rect.top, rect.bottom,
												   light_position, _light.point_data.range, view, proj);
	}
	else if(_light.type == light_type::spot)
	{
		float range = _light.spot_data.get_range();
		float clamped_inner_cone_angle =
			math::radians(math::clamp(_light.spot_data.get_inner_angle(), 0.0f, 89.0f));
		float clamped_outer_cone_angle =
			math::clamp(math::radians(_light.spot_data.get_outer_angle()), clamped_inner_cone_angle + 0.001f,
						math::radians(89.0f) + 0.001f);
		float cos_outer_cone = math::cos(clamped_outer_cone_angle);
		// Use the law of cosines to find the distance to the furthest edge of the
		// spotlight cone from a
		// position that is halfway down the spotlight direction
		const float radius = math::sqrt(1.25f * range * range - range * range * cos_outer_cone);
		math::vec3 center = light_position + 0.5f * light_direction * range;

		return math::compute_projected_sphere_rect(rect.left, rect.right, rect.top, rect.bottom, center,
												   radius, view, proj);
	}
	else
	{
		return 1;
	}
}
