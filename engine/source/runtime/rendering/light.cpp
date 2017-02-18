#include "light.h"

void Light::Spot::set_range(float r)
{
	if (r < 0)
		r = 0;

	range = r;
}

void Light::Spot::set_outer_angle(float angle)
{
	if (angle < inner_angle)
		angle = inner_angle;
		
	outer_angle = angle;
}

void Light::Spot::set_inner_angle(float angle)
{
	if (angle > outer_angle)
		angle = outer_angle;

	inner_angle = angle;
}
