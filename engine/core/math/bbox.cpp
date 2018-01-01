#include "bbox.h"
#include <limits>

namespace math
{
///////////////////////////////////////////////////////////////////////////////
// Static Member Definitions
///////////////////////////////////////////////////////////////////////////////
bbox bbox::empty(0, 0, 0, 0, 0, 0);

///////////////////////////////////////////////////////////////////////////////
// bbox Member Functions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  Name : bbox () (Default Constructor)
/// <summary>
/// bbox Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
bbox::bbox()
{
	// Initialize values
	reset();
}

//-----------------------------------------------------------------------------
//  Name : bbox () (Constructor)
/// <summary>
/// bbox Class Constructor, sets values from vector values passed
/// </summary>
//-----------------------------------------------------------------------------
bbox::bbox(const vec3& vecMin, const vec3& vecMax)
{
	// Copy vector values
	min = vecMin;
	max = vecMax;
}

//-----------------------------------------------------------------------------
//  Name : bbox () (Constructor)
/// <summary>
/// bbox Class Constructor, sets values from float values passed
/// </summary>
//-----------------------------------------------------------------------------
bbox::bbox(float xMin, float yMin, float zMin, float xMax, float yMax, float zMax)
{
	// Copy coordinate values
	min = vec3(xMin, yMin, zMin);
	max = vec3(xMax, yMax, zMax);
}

//-----------------------------------------------------------------------------
//  Name : reset ()
/// <summary>
/// Resets the bounding box values.
/// </summary>
//-----------------------------------------------------------------------------
void bbox::reset()
{
	min = vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
			   std::numeric_limits<float>::max());
	max = vec3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(),
			   -std::numeric_limits<float>::max());
}

//-----------------------------------------------------------------------------
//  Name : isPopulated ()
/// <summary>
/// Remains in reset state or has been populated?
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::is_populated() const
{
	if(min != vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(),
				   std::numeric_limits<float>::max()) ||
	   max != vec3(-std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(),
				   -std::numeric_limits<float>::max()))
		return true;

	// Still at reset state.
	return false;
}

//-----------------------------------------------------------------------------
// Name : isDegenerate ( )
/// <summary>
/// Determine if the bounding box is empty / degenerate.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::is_degenerate() const
{
	return (glm::abs<float>(max.x - min.x) < glm::epsilon<float>() &&
			glm::abs<float>(max.y - min.y) < glm::epsilon<float>() &&
			glm::abs<float>(max.z - min.z) < glm::epsilon<float>());
}

//-----------------------------------------------------------------------------
//  Name : getPlane ()
/// <summary>
/// Retrieves the plane for the specified side of the bounding box
/// </summary>
//-----------------------------------------------------------------------------
plane bbox::get_plane(volume_plane::e side) const
{
	plane bounds_plane;
	memset(&bounds_plane, 0, sizeof(plane));

	// Select the requested side
	switch(side)
	{
		case volume_plane::top:
			bounds_plane.data.y = 1;
			bounds_plane.data.w = -max.y;
			break;
		case volume_plane::right:
			bounds_plane.data.x = 1;
			bounds_plane.data.w = -max.x;
			break;
		case volume_plane::far_plane:
			bounds_plane.data.z = 1;
			bounds_plane.data.w = -max.z;
			break;
		case volume_plane::bottom:
			bounds_plane.data.y = -1;
			bounds_plane.data.w = min.y;
			break;
		case volume_plane::left:
			bounds_plane.data.x = -1;
			bounds_plane.data.w = min.x;
			break;
		case volume_plane::near_plane:
			bounds_plane.data.z = -1;
			bounds_plane.data.w = min.z;
			break;
	} // End side Switch

	// Return the plane
	return bounds_plane;
}

//-----------------------------------------------------------------------------
//  Name : getPlanePoints ()
/// <summary>
/// Retrieves the four points that form the boundary of the specified side of
/// the bounding box
/// </summary>
//-----------------------------------------------------------------------------
void bbox::get_plane_points(volume_plane::e side, vec3 points_out[]) const
{
	// Select the requested side
	switch(side)
	{
		case volume_plane::top:
			points_out[0].x = min.x;
			points_out[0].y = max.y;
			points_out[0].z = min.z;
			points_out[1].x = min.x;
			points_out[1].y = max.y;
			points_out[1].z = max.z;
			points_out[2].x = max.x;
			points_out[2].y = max.y;
			points_out[2].z = max.z;
			points_out[3].x = max.x;
			points_out[3].y = max.y;
			points_out[3].z = min.z;
			break;
		case volume_plane::right:
			points_out[0].x = max.x;
			points_out[0].y = min.y;
			points_out[0].z = min.z;
			points_out[1].x = max.x;
			points_out[1].y = max.y;
			points_out[1].z = min.z;
			points_out[2].x = max.x;
			points_out[2].y = max.y;
			points_out[2].z = max.z;
			points_out[3].x = max.x;
			points_out[3].y = min.y;
			points_out[3].z = max.z;
			break;
		case volume_plane::far_plane:
			points_out[0].x = max.x;
			points_out[0].y = min.y;
			points_out[0].z = max.z;
			points_out[1].x = max.x;
			points_out[1].y = max.y;
			points_out[1].z = max.z;
			points_out[2].x = min.x;
			points_out[2].y = max.y;
			points_out[2].z = max.z;
			points_out[3].x = min.x;
			points_out[3].y = min.y;
			points_out[3].z = max.z;
			break;
		case volume_plane::bottom:
			points_out[0].x = min.x;
			points_out[0].y = min.y;
			points_out[0].z = max.z;
			points_out[1].x = min.x;
			points_out[1].y = min.y;
			points_out[1].z = min.z;
			points_out[2].x = max.x;
			points_out[2].y = min.y;
			points_out[2].z = min.z;
			points_out[3].x = max.x;
			points_out[3].y = min.y;
			points_out[3].z = max.z;
			break;
		case volume_plane::left:
			points_out[0].x = min.x;
			points_out[0].y = min.y;
			points_out[0].z = max.z;
			points_out[1].x = min.x;
			points_out[1].y = max.y;
			points_out[1].z = max.z;
			points_out[2].x = min.x;
			points_out[2].y = max.y;
			points_out[2].z = min.z;
			points_out[3].x = min.x;
			points_out[3].y = min.y;
			points_out[3].z = min.z;
			break;
		case volume_plane::near_plane:
			points_out[0].x = min.x;
			points_out[0].y = min.y;
			points_out[0].z = min.z;
			points_out[1].x = min.x;
			points_out[1].y = max.y;
			points_out[1].z = min.z;
			points_out[2].x = max.x;
			points_out[2].y = max.y;
			points_out[2].z = min.z;
			points_out[3].x = max.x;
			points_out[3].y = min.y;
			points_out[3].z = min.z;
			break;

	} // End side Switch
}

//-----------------------------------------------------------------------------
//  Name : fromPoints ()
/// <summary>
/// Calculates the bounding box based on the points specified.
/// </summary>
//-----------------------------------------------------------------------------
bbox& bbox::from_points(const char* point_buffer, unsigned int point_count, unsigned int point_stride,
						bool reset_bounds /* = true */)
{
	// Reset the box if requested
	if(reset_bounds)
		reset();

	// Loop through all the points supplied and grow the box.
	if(point_buffer && point_count)
	{
		for(unsigned int v = 0; v < point_count; ++v, point_buffer += point_stride)
			add_point(*(vec3*)point_buffer);

	} // End if has data
	return *this;
}

bbox& bbox::from_sphere(const vec3& center, float radius)
{
	min = center - vec3(radius, radius, radius);
	max = center + vec3(radius, radius, radius);

	return *this;
}

//-----------------------------------------------------------------------------
//  Name : validate ()
/// <summary>
/// Ensures that the values placed in the min / max values never make the
/// bounding box itself inverted.
/// </summary>
//-----------------------------------------------------------------------------
void bbox::validate()
{
	float rTemp;
	if(max.x < min.x)
	{
		rTemp = max.x;
		max.x = min.x;
		min.x = rTemp;
	}
	if(max.y < min.y)
	{
		rTemp = max.y;
		max.y = min.y;
		min.y = rTemp;
	}
	if(max.z < min.z)
	{
		rTemp = max.z;
		max.z = min.z;
		min.z = rTemp;
	}
}

//-----------------------------------------------------------------------------
//  Name : intersect()
/// <summary>
/// Tests to see if this AABB is intersected by another AABB
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect(const bbox& bounds) const
{
	return (min.x <= bounds.max.x) && (min.y <= bounds.max.y) && (min.z <= bounds.max.z) &&
		   (max.x >= bounds.min.x) && (max.y >= bounds.min.y) && (max.z >= bounds.min.z);
}

//-----------------------------------------------------------------------------
//  Name : intersect()
/// <summary>
/// Tests to see if this AABB is intersected by another AABB with full
/// containment test.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect(const bbox& bounds, bool& contained) const
{
	// Set to true by default
	contained = true;

	// Does the point fall outside any of the AABB planes?
	if(bounds.min.x < min.x || bounds.min.x > max.x)
		contained = false;
	else if(bounds.min.y < min.y || bounds.min.y > max.y)
		contained = false;
	else if(bounds.min.z < min.z || bounds.min.z > max.z)
		contained = false;
	else if(bounds.max.x < min.x || bounds.max.x > max.x)
		contained = false;
	else if(bounds.max.y < min.y || bounds.max.y > max.y)
		contained = false;
	else if(bounds.max.z < min.z || bounds.max.z > max.z)
		contained = false;

	// Return immediately if it's fully contained
	if(contained == true)
		return true;

	// Perform full intersection test
	return (min.x <= bounds.max.x) && (min.y <= bounds.max.y) && (min.z <= bounds.max.z) &&
		   (max.x >= bounds.min.x) && (max.y >= bounds.min.y) && (max.z >= bounds.min.z);
}

//-----------------------------------------------------------------------------
//  Name : intersect()
/// <summary>
/// Tests to see if this AABB is intersected by another AABB and return
/// the resulting intersection.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect(const bbox& bounds, bbox& intersection) const
{
	intersection.min.x = glm::max(min.x, bounds.min.x);
	intersection.min.y = glm::max(min.y, bounds.min.y);
	intersection.min.z = glm::max(min.z, bounds.min.z);
	intersection.max.x = glm::min(max.x, bounds.max.x);
	intersection.max.y = glm::min(max.y, bounds.max.y);
	intersection.max.z = glm::min(max.z, bounds.max.z);

	// Test for intersection
	if(intersection.min.x > intersection.max.x || intersection.min.y > intersection.max.y ||
	   intersection.min.z > intersection.max.z)
		return false;

	// Intersecting!
	return true;
}

//-----------------------------------------------------------------------------
//  Name : intersect()
/// <summary>
/// Tests to see if this AABB is intersected by another AABB, includes
/// a tolerance for checking.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect(const bbox& bounds, const vec3& tolerance) const
{
	return ((min.x - tolerance.x) <= (bounds.max.x + tolerance.x)) &&
		   ((min.y - tolerance.y) <= (bounds.max.y + tolerance.y)) &&
		   ((min.z - tolerance.z) <= (bounds.max.z + tolerance.z)) &&
		   ((max.x + tolerance.x) >= (bounds.min.x - tolerance.x)) &&
		   ((max.y + tolerance.y) >= (bounds.min.y - tolerance.y)) &&
		   ((max.z + tolerance.z) >= (bounds.min.z - tolerance.z));
}

//-----------------------------------------------------------------------------
//  Name : intersect ()
/// <summary>
/// This function tests to see if a ray intersects the AABB.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect(const vec3& origin, const vec3& velocity, float& t,
					 bool restrict_range /* = true */) const
{
	float tMin = std::numeric_limits<float>::min();
	float tMax = std::numeric_limits<float>::max();
	float t1, t2, fTemp;

	// If ray origin is inside bounding box, just return true (treat AABB as a
	// solid box)
	if(contains_point(origin) == true)
	{
		t = 0.0f;
		return true;

	} // End if point in box

	// X Slabs
	// Is it pointing toward?
	if(glm::abs<float>(velocity.x) > glm::epsilon<float>())
	{
		fTemp = 1.0f / velocity.x;
		t1 = (max.x - origin.x) * fTemp;
		t2 = (min.x - origin.x) * fTemp;

		// Reorder if necessary
		if(t1 > t2)
		{
			fTemp = t1;
			t1 = t2;
			t2 = fTemp;
		}

		// Compare and validate
		if(t1 > tMin)
			tMin = t1;
		if(t2 < tMax)
			tMax = t2;
		if(tMin > tMax)
			return false;
		if(tMax < 0)
			return false;

	} // End if
	else
	{
		// We cannot be intersecting in this case if the origin is outside of the
		// slab
		// if ( origin.x < (min.x - origin.x) || origin.x > (max.x - origin.x) )
		if(origin.x < min.x || origin.x > max.x)
			return false;

	} // End else

	// Y Slabs
	// Is it pointing toward?
	if(glm::abs<float>(velocity.y) > glm::epsilon<float>())
	{
		fTemp = 1.0f / velocity.y;
		t1 = (max.y - origin.y) * fTemp;
		t2 = (min.y - origin.y) * fTemp;

		// Reorder if necessary
		if(t1 > t2)
		{
			fTemp = t1;
			t1 = t2;
			t2 = fTemp;
		}

		// Compare and validate
		if(t1 > tMin)
			tMin = t1;
		if(t2 < tMax)
			tMax = t2;
		if(tMin > tMax)
			return false;
		if(tMax < 0)
			return false;

	} // End if
	else
	{
		// We cannot be intersecting in this case if the origin is outside of the
		// slab
		// if ( origin.y < (min.y - origin.y) || origin.y > (max.y - origin.y) )
		if(origin.y < min.y || origin.y > max.y)
			return false;

	} // End else

	// Z Slabs
	// Is it pointing toward?
	if(glm::abs<float>(velocity.z) > glm::epsilon<float>())
	{
		fTemp = 1.0f / velocity.z;
		t1 = (max.z - origin.z) * fTemp;
		t2 = (min.z - origin.z) * fTemp;

		// Reorder if necessary
		if(t1 > t2)
		{
			fTemp = t1;
			t1 = t2;
			t2 = fTemp;
		}

		// Compare and validate
		if(t1 > tMin)
			tMin = t1;
		if(t2 < tMax)
			tMax = t2;
		if(tMin > tMax)
			return false;
		if(tMax < 0)
			return false;

	} // End if
	else
	{
		// We cannot be intersecting in this case if the origin is outside of the
		// slab
		// if ( origin.z < (min.z - origin.z) || origin.z > (max.z - origin.z) )
		if(origin.z < min.z || origin.z > max.z)
			return false;

	} // End else

	// Pick the correct t value
	if(tMin > 0)
		t = tMin;
	else
		t = tMax;

	// Outside our valid range? if yes, return no collide
	if(t < 0.0f || (restrict_range == true && t > 1.0f))
		return false;

	// We intersected!
	return true;
}

//-----------------------------------------------------------------------------
//  Name : intersect ()
/// <summary>
/// This function tests to see if a triangle intersects the AABB.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect(const vec3& v_tri0, const vec3& v_tri1, const vec3& v_tri2, const bbox& tri_bounds) const
{
	// Perform rough "broadphase" rejection by testing for intersection
	// between the supplied triangle bounding box and the source box.
	if(tri_bounds.min.x > max.x || tri_bounds.max.x < min.x || tri_bounds.min.y > max.y ||
	   tri_bounds.max.y < min.y || tri_bounds.min.z > max.z || tri_bounds.max.z < min.z)
		return false;

	// Move everything such that the box center is located at <0,0,0>
	// and the entire test becomes relative to it.
	const vec3 vCenter = get_center();
	const vec3 vExtents = get_extents();
	const vec3 v0 = v_tri0 - vCenter;
	const vec3 v1 = v_tri1 - vCenter;
	const vec3 v2 = v_tri2 - vCenter;

	// Next we need to test to see if the triangle's plane intersects the
	// source box. Begin by generating the plane itself. Note: We need
	// the edge vectors for later tests, so keep them around.

	vec3 vEdge0 = v1 - v0;
	vec3 vEdge1 = v2 - v1;
	vec3 vNormal = glm::cross(vEdge0, vEdge1);
	float fPlaneDistance = -glm::dot(vNormal, v0);

	// Calculate near / far extreme points
	vec3 vNearPoint, vFarPoint;
	if(vNormal.x > 0.0f)
	{
		vFarPoint.x = max.x;
		vNearPoint.x = min.x;
	}
	else
	{
		vFarPoint.x = min.x;
		vNearPoint.x = max.x;
	}
	if(vNormal.y > 0.0f)
	{
		vFarPoint.y = max.y;
		vNearPoint.y = min.y;
	}
	else
	{
		vFarPoint.y = min.y;
		vNearPoint.y = max.y;
	}
	if(vNormal.z > 0.0f)
	{
		vFarPoint.z = max.z;
		vNearPoint.z = min.z;
	}
	else
	{
		vFarPoint.z = min.z;
		vNearPoint.z = max.z;
	}

	// If near extreme point is outside, then the AABB is totally outside the
	// plane
	if(glm::dot(vNormal, vNearPoint - vCenter) + fPlaneDistance > 0.0f)
		return false;

	// If far extreme point is inside, then the AABB is not intersecting the plane
	if(glm::dot(vNormal, vFarPoint - vCenter) + fPlaneDistance < 0.0f)
		return false;

	// AXISTEST macro required variables
	vec3 vAbsEdge;
	float fTemp0, fTemp1, fMin, fMax;
#define AXISTEST(vEdge, vP0, vP1, nComponent0, nComponent1)                                                  \
	fTemp0 = vEdge[nComponent1] * vP0[nComponent0] - vEdge[nComponent0] * vP0[nComponent1];                  \
	fTemp1 = vEdge[nComponent1] * vP1[nComponent0] - vEdge[nComponent0] * vP1[nComponent1];                  \
	if(fTemp0 < fTemp1)                                                                                      \
	{                                                                                                        \
		fMin = fTemp0;                                                                                       \
		fMax = fTemp1;                                                                                       \
	}                                                                                                        \
	else                                                                                                     \
	{                                                                                                        \
		fMin = fTemp1;                                                                                       \
		fMax = fTemp0;                                                                                       \
	}                                                                                                        \
	fTemp0 = vAbsEdge[nComponent1] * vExtents[nComponent0] + vAbsEdge[nComponent0] * vExtents[nComponent1];  \
	if(fMin > fTemp0 || fMax < -fTemp0)                                                                      \
		return false;

#define AXISTEST2(vEdge, vP0, vP1, nComponent0, nComponent1)                                                 \
	fTemp0 = -vEdge[nComponent1] * vP0[nComponent0] + vEdge[nComponent0] * vP0[nComponent1];                 \
	fTemp1 = -vEdge[nComponent1] * vP1[nComponent0] + vEdge[nComponent0] * vP1[nComponent1];                 \
	if(fTemp0 < fTemp1)                                                                                      \
	{                                                                                                        \
		fMin = fTemp0;                                                                                       \
		fMax = fTemp1;                                                                                       \
	}                                                                                                        \
	else                                                                                                     \
	{                                                                                                        \
		fMin = fTemp1;                                                                                       \
		fMax = fTemp0;                                                                                       \
	}                                                                                                        \
	fTemp0 = vAbsEdge[nComponent1] * vExtents[nComponent0] + vAbsEdge[nComponent0] * vExtents[nComponent1];  \
	if(fMin > fTemp0 || fMax < -fTemp0)                                                                      \
		return false;

	// Test to see if the triangle edges cross the box.
	vAbsEdge.x = glm::abs<float>(vEdge0.x);
	vAbsEdge.y = glm::abs<float>(vEdge0.y);
	vAbsEdge.z = glm::abs<float>(vEdge0.z);
	AXISTEST(vEdge0, v0, v2, 1, 2);  // X
	AXISTEST2(vEdge0, v0, v2, 0, 2); // Y
	AXISTEST(vEdge0, v2, v1, 0, 1);  // Z

	vAbsEdge.x = glm::abs<float>(vEdge1.x);
	vAbsEdge.y = glm::abs<float>(vEdge1.y);
	vAbsEdge.z = glm::abs<float>(vEdge1.z);
	AXISTEST(vEdge1, v0, v2, 1, 2);  // X
	AXISTEST2(vEdge1, v0, v2, 0, 2); // Y
	AXISTEST(vEdge1, v0, v1, 0, 1);  // Z

	const vec3 vEdge2 = v0 - v2;
	vAbsEdge.x = glm::abs<float>(vEdge2.x);
	vAbsEdge.y = glm::abs<float>(vEdge2.y);
	vAbsEdge.z = glm::abs<float>(vEdge2.z);
	AXISTEST(vEdge2, v0, v1, 1, 2);  // X
	AXISTEST2(vEdge2, v0, v1, 0, 2); // Y
	AXISTEST(vEdge2, v2, v1, 0, 1);  // Z

	// Overlapping
	return true;
}

//-----------------------------------------------------------------------------
//  Name : intersect ()
/// <summary>
/// This function tests to see if a triangle intersects the AABB.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect(const vec3& v_tri0, const vec3& v_tri1, const vec3& v_tri2) const
{
	bbox tri_bounds;
	tri_bounds.add_point(v_tri0);
	tri_bounds.add_point(v_tri1);
	tri_bounds.add_point(v_tri2);

	// Perform rough "broadphase" rejection by testing for intersection
	// between the supplied triangle bounding box and the source box.
	if(tri_bounds.min.x > max.x || tri_bounds.max.x < min.x || tri_bounds.min.y > max.y ||
	   tri_bounds.max.y < min.y || tri_bounds.min.z > max.z || tri_bounds.max.z < min.z)
		return false;

	// Move everything such that the box center is located at <0,0,0>
	// and the entire test becomes relative to it.
	const vec3 vCenter = get_center();
	const vec3 vExtents = get_extents();
	const vec3 v0 = v_tri0 - vCenter;
	const vec3 v1 = v_tri1 - vCenter;
	const vec3 v2 = v_tri2 - vCenter;

	// Next we need to test to see if the triangle's plane intersects the
	// source box. Begin by generating the plane itself. Note: We need
	// the edge vectors for later tests, so keep them around.

	vec3 vEdge0 = v1 - v0;
	vec3 vEdge1 = v2 - v1;
	vec3 vNormal = glm::cross(vEdge0, vEdge1);
	float fPlaneDistance = -glm::dot(vNormal, v0);

	// Calculate near / far extreme points
	vec3 vNearPoint, vFarPoint;
	if(vNormal.x > 0.0f)
	{
		vFarPoint.x = max.x;
		vNearPoint.x = min.x;
	}
	else
	{
		vFarPoint.x = min.x;
		vNearPoint.x = max.x;
	}
	if(vNormal.y > 0.0f)
	{
		vFarPoint.y = max.y;
		vNearPoint.y = min.y;
	}
	else
	{
		vFarPoint.y = min.y;
		vNearPoint.y = max.y;
	}
	if(vNormal.z > 0.0f)
	{
		vFarPoint.z = max.z;
		vNearPoint.z = min.z;
	}
	else
	{
		vFarPoint.z = min.z;
		vNearPoint.z = max.z;
	}

	// If near extreme point is outside, then the AABB is totally outside the
	// plane
	if(glm::dot(vNormal, vNearPoint - vCenter) + fPlaneDistance > 0.0f)
		return false;

	// If far extreme point is inside, then the AABB is not intersecting the plane
	if(glm::dot(vNormal, vFarPoint - vCenter) + fPlaneDistance < 0.0f)
		return false;

	// AXISTEST macro required variables
	vec3 vAbsEdge;
	float fTemp0, fTemp1, fMin, fMax;
#define AXISTEST(vEdge, vP0, vP1, nComponent0, nComponent1)                                                  \
	fTemp0 = vEdge[nComponent1] * vP0[nComponent0] - vEdge[nComponent0] * vP0[nComponent1];                  \
	fTemp1 = vEdge[nComponent1] * vP1[nComponent0] - vEdge[nComponent0] * vP1[nComponent1];                  \
	if(fTemp0 < fTemp1)                                                                                      \
	{                                                                                                        \
		fMin = fTemp0;                                                                                       \
		fMax = fTemp1;                                                                                       \
	}                                                                                                        \
	else                                                                                                     \
	{                                                                                                        \
		fMin = fTemp1;                                                                                       \
		fMax = fTemp0;                                                                                       \
	}                                                                                                        \
	fTemp0 = vAbsEdge[nComponent1] * vExtents[nComponent0] + vAbsEdge[nComponent0] * vExtents[nComponent1];  \
	if(fMin > fTemp0 || fMax < -fTemp0)                                                                      \
		return false;

#define AXISTEST2(vEdge, vP0, vP1, nComponent0, nComponent1)                                                 \
	fTemp0 = -vEdge[nComponent1] * vP0[nComponent0] + vEdge[nComponent0] * vP0[nComponent1];                 \
	fTemp1 = -vEdge[nComponent1] * vP1[nComponent0] + vEdge[nComponent0] * vP1[nComponent1];                 \
	if(fTemp0 < fTemp1)                                                                                      \
	{                                                                                                        \
		fMin = fTemp0;                                                                                       \
		fMax = fTemp1;                                                                                       \
	}                                                                                                        \
	else                                                                                                     \
	{                                                                                                        \
		fMin = fTemp1;                                                                                       \
		fMax = fTemp0;                                                                                       \
	}                                                                                                        \
	fTemp0 = vAbsEdge[nComponent1] * vExtents[nComponent0] + vAbsEdge[nComponent0] * vExtents[nComponent1];  \
	if(fMin > fTemp0 || fMax < -fTemp0)                                                                      \
		return false;

	// Test to see if the triangle edges cross the box.
	vAbsEdge.x = glm::abs<float>(vEdge0.x);
	vAbsEdge.y = glm::abs<float>(vEdge0.y);
	vAbsEdge.z = glm::abs<float>(vEdge0.z);
	AXISTEST(vEdge0, v0, v2, 1, 2);  // X
	AXISTEST2(vEdge0, v0, v2, 0, 2); // Y
	AXISTEST(vEdge0, v2, v1, 0, 1);  // Z

	vAbsEdge.x = glm::abs<float>(vEdge1.x);
	vAbsEdge.y = glm::abs<float>(vEdge1.y);
	vAbsEdge.z = glm::abs<float>(vEdge1.z);
	AXISTEST(vEdge1, v0, v2, 1, 2);  // X
	AXISTEST2(vEdge1, v0, v2, 0, 2); // Y
	AXISTEST(vEdge1, v0, v1, 0, 1);  // Z

	const vec3 vEdge2 = v0 - v2;
	vAbsEdge.x = glm::abs<float>(vEdge2.x);
	vAbsEdge.y = glm::abs<float>(vEdge2.y);
	vAbsEdge.z = glm::abs<float>(vEdge2.z);
	AXISTEST(vEdge2, v0, v1, 1, 2);  // X
	AXISTEST2(vEdge2, v0, v1, 0, 2); // Y
	AXISTEST(vEdge2, v2, v1, 0, 1);  // Z

	// Overlapping
	return true;
}

//-----------------------------------------------------------------------------
//  Name : contains()
/// <summary>
/// Tests to see if a point falls within this bounding box or not
/// including a specific tolerance around the box.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::contains_point(const vec3& point, float tolerance) const
{
	if(point.x < min.x - tolerance || point.x > max.x + tolerance)
		return false;
	if(point.y < min.y - tolerance || point.y > max.y + tolerance)
		return false;
	if(point.z < min.z - tolerance || point.z > max.z + tolerance)
		return false;
	return true;
}

//-----------------------------------------------------------------------------
//  Name : contains()
/// <summary>
/// Tests to see if a point falls within this bounding box or not.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::contains_point(const vec3& point) const
{
	if(point.x < min.x || point.x > max.x)
		return false;
	if(point.y < min.y || point.y > max.y)
		return false;
	if(point.z < min.z || point.z > max.z)
		return false;
	return true;
}

//-----------------------------------------------------------------------------
//  Name : contains()
/// <summary>
/// Tests to see if a point falls within this bounding box or not
/// including a specific tolerance around the box.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::contains_point(const vec3& point, const vec3& tolerance) const
{
	if(point.x < min.x - tolerance.x || point.x > max.x + tolerance.x)
		return false;
	if(point.y < min.y - tolerance.y || point.y > max.y + tolerance.y)
		return false;
	if(point.z < min.z - tolerance.z || point.z > max.z + tolerance.z)
		return false;
	return true;
}

//-----------------------------------------------------------------------------
//  Name : closestPoint ()
/// <summary>
/// Compute a point, on the surface of the AABB, that falls closest to
/// the input point.
/// </summary>
//-----------------------------------------------------------------------------
vec3 bbox::closest_point(const vec3& test_point) const
{
	vec3 Closest;

	// Test X extent
	if(test_point.x < min.x)
		Closest.x = min.x;
	else if(test_point.x > max.x)
		Closest.x = max.x;
	else
		Closest.x = test_point.x;

	// Test Y extent
	if(test_point.y < min.y)
		Closest.y = min.y;
	else if(test_point.y > max.y)
		Closest.y = max.y;
	else
		Closest.y = test_point.y;

	// Test Z extent
	if(test_point.z < min.z)
		Closest.z = min.z;
	else if(test_point.z > max.z)
		Closest.z = max.z;
	else
		Closest.z = test_point.z;

	// Return the closest test_point
	return Closest;
}

//-----------------------------------------------------------------------------
//  Name : transform ()
/// <summary>
/// Transforms an axis aligned bounding box, by the specified matrix,
/// outputting new AAB values which are a best fit about that 'virtual
/// transformation'.
/// </summary>
//-----------------------------------------------------------------------------
bbox& bbox::mul(const transform& t)
{
	*this = mul(*this, t);

	// Return reference to self
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : mul () (Static)
/// <summary>
/// Transforms the specified bounding box by the provided matrix and return the
/// new resulting box as a copy.
/// </summary>
//-----------------------------------------------------------------------------
bbox bbox::mul(const bbox& bounds, const transform& t)
{
	auto xa = t.x_axis() * bounds.min.x;
	auto xb = t.x_axis() * bounds.max.x;
	auto ya = t.y_axis() * bounds.min.y;
	auto yb = t.y_axis() * bounds.max.y;
	auto za = t.z_axis() * bounds.min.z;
	auto zb = t.z_axis() * bounds.max.z;

	return bbox(math::min(xa, xb) + math::min(ya, yb) + math::min(za, zb) + t.get_position(),
				math::max(xa, xb) + math::max(ya, yb) + math::max(za, zb) + t.get_position());

	//	bbox result;
	//	vec3 bounds_center = bounds.get_center();

	//	// Compute new extents values
	//	const vec3 Ex = t.x_axis() * (bounds.max.x - bounds_center.x);
	//	const vec3 Ey = t.y_axis() * (bounds.max.y - bounds_center.y);
	//	const vec3 Ez = t.z_axis() * (bounds.max.z - bounds_center.z);

	//	// Calculate new extents actual
	//	const float fEx = glm::abs<float>(Ex.x) + glm::abs<float>(Ey.x) + glm::abs<float>(Ez.x);
	//	const float fEy = glm::abs<float>(Ex.y) + glm::abs<float>(Ey.y) + glm::abs<float>(Ez.y);
	//	const float fEz = glm::abs<float>(Ex.z) + glm::abs<float>(Ey.z) + glm::abs<float>(Ez.z);

	//	// Compute new center (we use 'transformNormal' because we only
	//	// want to apply rotation and scale).
	//	bounds_center = t.transform_normal(bounds_center);

	//	// Calculate final bounding box (add on translation)
	//	const vec3& vTranslation = t.get_position();
	//	result.min.x = (bounds_center.x - fEx) + vTranslation.x;
	//	result.min.y = (bounds_center.y - fEy) + vTranslation.y;
	//	result.min.z = (bounds_center.z - fEz) + vTranslation.z;
	//	result.max.x = (bounds_center.x + fEx) + vTranslation.x;
	//	result.max.y = (bounds_center.y + fEy) + vTranslation.y;
	//	result.max.z = (bounds_center.z + fEz) + vTranslation.z;
	//	return result;
}

//-----------------------------------------------------------------------------
//  Name : inflate()
/// <summary>
/// Grow (or shrink if you specify a negative value) the bounding box
/// by the specified number of world space units on all three axes.
/// </summary>
//-----------------------------------------------------------------------------
void bbox::inflate(float grow_size)
{
	min.x -= grow_size;
	min.y -= grow_size;
	min.z -= grow_size;
	max.x += grow_size;
	max.y += grow_size;
	max.z += grow_size;
}

//-----------------------------------------------------------------------------
//  Name : inflate()
/// <summary>
/// Grow (or shrink if you specify a negative value) the bounding box
/// by the specified numbers number of world space units on each of the
/// three axes independently.
/// </summary>
//-----------------------------------------------------------------------------
void bbox::inflate(const vec3& grow_size)
{
	min.x -= grow_size.x;
	min.y -= grow_size.y;
	min.z -= grow_size.z;
	max.x += grow_size.x;
	max.y += grow_size.y;
	max.z += grow_size.z;
}

//-----------------------------------------------------------------------------
//  Name : operator+=()
/// <summary>
/// Moves the bounding box by the vector passed.
/// </summary>
//-----------------------------------------------------------------------------
bbox& bbox::operator+=(const vec3& shift)
{
	min += shift;
	max += shift;
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator-=()
/// <summary>
/// Moves the bounding box by the vector passed.
/// </summary>
//-----------------------------------------------------------------------------
bbox& bbox::operator-=(const vec3& shift)
{
	min -= shift;
	max -= shift;
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator*()
/// <summary>
/// Scales the bounding box values by the scalar passed.
/// </summary>
//-----------------------------------------------------------------------------
bbox bbox::operator*(float scale) const
{
	return bbox(min * scale, max * scale);
}

//-----------------------------------------------------------------------------
//  Name : operator*=()
/// <summary>
/// Scales the bounding box values by the scalar passed.
/// </summary>
//-----------------------------------------------------------------------------
bbox& bbox::operator*=(float scale)
{
	min *= scale;
	max *= scale;
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator*=()
/// <summary>
/// Transforms the bounding box by the matrix passed.
/// </summary>
//-----------------------------------------------------------------------------
bbox& bbox::operator*=(const transform& t)
{
	mul(t);
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator==()
/// <summary>
/// Test for quality between this bounding box and the other.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::operator==(const bbox& bounds) const
{
	return (min == bounds.min) && (max == bounds.max);
}

//-----------------------------------------------------------------------------
//  Name : operator!=()
/// <summary>
/// Test for quality between this bounding box and the other.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::operator!=(const bbox& bounds) const
{
	return (min != bounds.min) || (max != bounds.max);
}
}
