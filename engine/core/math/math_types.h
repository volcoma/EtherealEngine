#pragma once

#include <type_traits>
namespace math
{

//-----------------------------------------------------------------------------
// Common Global Enumerations
//-----------------------------------------------------------------------------
// Resulting classifications for volume queries such as AABB vs. Frustum, etc.

enum class volume_query
{
	inside = 0,
	outside,
	intersect
};

enum class plane_query
{
	front = 0,
	back,
	on,
	spanning
};

// Used to identify specific planes of volumes such as the sides of an AABB or
// Frustum
namespace volume_plane
{
enum e
{
	left = 0,
	right,
	top,
	bottom,
	near_plane,
	far_plane,
};
}

// Used to identify specific volume boundary points such as the 8 points of an
// AABB or Frustum
namespace volume_geometry_point
{
enum e
{
	right_bottom_far = 0,
	right_bottom_near,
	right_top_far,
	right_top_near,
	left_bottom_far,
	left_bottom_near,
	left_top_far,
	left_top_near
};
}
}
