#include "bbox_extruded.h"

namespace math
{
//-----------------------------------------------------------------------------
// Module Local Variables
//-----------------------------------------------------------------------------
namespace
{
//  Friendly names for bit masked fields used throughout the various functions.
//  This list is a 3-bit field which defines 1 of 8 points in an axis-aligned
//  bounding box.
enum : unsigned int
{
	MIN_X = 1,
	MAX_X = 0,
	MIN_Y = 2,
	MAX_Y = 0,
	MIN_Z = 4,
	MAX_Z = 0,
	INVALID = 0xFFFFFFFF
};

const int HalfSpaceRemap[64] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
								-1, -1, -1, -1, -1, 0,  1,  2,  -1, 3,  4,  5,  -1, 6,  7,  8,
								-1, -1, -1, -1, -1, 9,  10, 11, -1, 12, 13, 14, -1, 15, 16, 17,
								-1, -1, -1, -1, -1, 18, 19, 20, -1, 21, 22, 23, -1, 24, 25, -1};

// Lookup table that stores which edges will be extruded for a given half-space
// configuration.
const unsigned int SilhouetteLUT[26][6][2] = {{{MIN_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z},
											   {MAX_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z},
											   {MAX_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MIN_Z},
											   {MIN_X | MIN_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z}},
											  {{MIN_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z},
											   {MAX_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z},
											   {MIN_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z}},
											  {{MIN_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z},
											   {MAX_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z}},
											  {{MAX_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MIN_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MAX_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MIN_Z},
											   {MIN_X | MIN_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z}},
											  {{MAX_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MIN_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MIN_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z}},
											  {{MAX_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MIN_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MAX_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MIN_Z},
											   {MIN_X | MIN_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z}},
											  {{MAX_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z},
											   {MAX_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MAX_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MIN_Z},
											   {MIN_X | MIN_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z}},
											  {{MAX_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MIN_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z}},
											  {{MAX_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z},
											   {INVALID, INVALID},
											   {INVALID, INVALID}},
											  {{MIN_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z},
											   {MAX_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MAX_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MIN_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z}},
											  {{MIN_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z},
											   {MAX_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z},
											   {MAX_X | MIN_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z}},
											  {{MIN_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z},
											   {MAX_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MAX_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MAX_X | MIN_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z}},
											  {{MAX_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MIN_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MAX_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MIN_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z}},
											  {{MAX_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MIN_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MIN_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z},
											   {MAX_X | MIN_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z}},
											  {{MAX_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MIN_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MAX_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MIN_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MAX_X | MIN_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z}},
											  {{MIN_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MAX_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MAX_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MIN_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z}},
											  {{MIN_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MIN_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MIN_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z},
											   {MAX_X | MIN_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z}},
											  {{MIN_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MAX_X | MIN_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z},
											   {INVALID, INVALID},
											   {INVALID, INVALID}},
											  {{MIN_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MAX_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z}},
											  {{MIN_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z}},
											  {{MIN_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MAX_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MAX_Y | MAX_Z},
											   {INVALID, INVALID},
											   {INVALID, INVALID}},
											  {{MAX_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MIN_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MAX_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z}},
											  {{MAX_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MIN_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MIN_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z}},
											  {{MAX_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MIN_Z},
											   {MIN_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MIN_Y | MAX_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MIN_Y | MIN_Z},
											   {INVALID, INVALID},
											   {INVALID, INVALID}},
											  {{MAX_X | MIN_Y | MIN_Z, MAX_X | MIN_Y | MAX_Z},
											   {MAX_X | MAX_Y | MAX_Z, MAX_X | MAX_Y | MIN_Z},
											   {MAX_X | MAX_Y | MIN_Z, MAX_X | MIN_Y | MIN_Z},
											   {MAX_X | MIN_Y | MAX_Z, MAX_X | MAX_Y | MAX_Z},
											   {INVALID, INVALID},
											   {INVALID, INVALID}},
											  {{MIN_X | MIN_Y | MAX_Z, MIN_X | MIN_Y | MIN_Z},
											   {MIN_X | MAX_Y | MIN_Z, MIN_X | MAX_Y | MAX_Z},
											   {MIN_X | MIN_Y | MIN_Z, MIN_X | MAX_Y | MIN_Z},
											   {MIN_X | MAX_Y | MAX_Z, MIN_X | MIN_Y | MAX_Z},
											   {INVALID, INVALID},
											   {INVALID, INVALID}}};

} // End Unnamed Namespace

///////////////////////////////////////////////////////////////////////////////
// bbox_extruded Member Functions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  Name : bbox_extruded () (Default Constructor)
/// <summary>
/// bbox_extruded Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
bbox_extruded::bbox_extruded()
{
	// Initialize values
	reset();
}

//-----------------------------------------------------------------------------
//  Name : bbox_extruded () (Constructor)
/// <summary>
/// bbox_extruded Class Constructor, sets values from vector values passed
/// </summary>
//-----------------------------------------------------------------------------
bbox_extruded::bbox_extruded(const bbox& AABB, const vec3& vecOrigin, float fRange,
							 const transform* pTransform /* = nullptr */)
{
	// Generate extrude box values automatically
	extrude(AABB, vecOrigin, fRange, pTransform);
}

//-----------------------------------------------------------------------------
//  Name : reset ()
/// <summary>
/// Resets the bounding box values.
/// </summary>
//-----------------------------------------------------------------------------
void bbox_extruded::reset()
{
	source_min = vec3(0, 0, 0);
	source_max = vec3(0, 0, 0);
	projection_point = vec3(0, 0, 0);
	projection_range = 0.0f;
	edge_count = 0;
}

//-----------------------------------------------------------------------------
//  Name : extrude()
/// <summary>
/// Generate the extruded box planes / edges.
/// </summary>
//-----------------------------------------------------------------------------
void bbox_extruded::extrude(const bbox& AABB, const vec3& vecOrigin, float fRange,
							const transform* pTransform /* = nullptr */)
{
	bbox Bounds = AABB;

	// Reset any previously computed data
	reset();

	// transform bounds if matrix provided
	if(pTransform != nullptr)
		Bounds.mul(*pTransform);

	// Make a copy of the values used to generate this box
	source_min = Bounds.min;
	source_max = Bounds.max;
	projection_point = vecOrigin;
	projection_range = fRange;

	// Simple comparisons of the bounding box against the projection origin
	// quickly determine which
	// halfspaces the box exists in. From this, look up values from the
	// precomputed table that
	// define which edges of the bounding box are silhouettes.
	unsigned int nHalfSpace = 0;
	if(source_min.x <= vecOrigin.x)
		nHalfSpace |= 0x1;
	if(source_max.x >= vecOrigin.x)
		nHalfSpace |= 0x2;
	if(source_min.y <= vecOrigin.y)
		nHalfSpace |= 0x4;
	if(source_max.y >= vecOrigin.y)
		nHalfSpace |= 0x8;
	if(source_min.z <= vecOrigin.z)
		nHalfSpace |= 0x10;
	if(source_max.z >= vecOrigin.z)
		nHalfSpace |= 0x20;

	// Lookup the appropriate silhouette edges
	unsigned int nRemap = HalfSpaceRemap[nHalfSpace];
	for(edge_count = 0; (SilhouetteLUT[nRemap][edge_count][0] != INVALID) && (edge_count < 6);)
	{
		vec3 vPoint1, vPoint2;

		// Lookup correct points to use
		unsigned int nPoint1 = SilhouetteLUT[nRemap][edge_count][0];
		unsigned int nPoint2 = SilhouetteLUT[nRemap][edge_count][1];

		// Select the actual point coordinates
		vPoint1.x = (nPoint1 & MIN_X) ? source_min.x : source_max.x;
		vPoint1.y = (nPoint1 & MIN_Y) ? source_min.y : source_max.y;
		vPoint1.z = (nPoint1 & MIN_Z) ? source_min.z : source_max.z;
		vPoint2.x = (nPoint2 & MIN_X) ? source_min.x : source_max.x;
		vPoint2.y = (nPoint2 & MIN_Y) ? source_min.y : source_max.y;
		vPoint2.z = (nPoint2 & MIN_Z) ? source_min.z : source_max.z;

		// Record the edge data we used
		silhouette_edges[edge_count][0] = nPoint1;
		silhouette_edges[edge_count][1] = nPoint2;
		extruded_planes[edge_count++] = plane::from_points(vecOrigin, vPoint1, vPoint2);

	} // Next Potential
}

//-----------------------------------------------------------------------------
//  Name : getEdge()
/// <summary>
/// Retrieve the specified edge points.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox_extruded::get_edge(unsigned int nEdge, vec3& vPoint1, vec3& vPoint2) const
{
	// Valid index?
	if(nEdge >= edge_count)
		return false;

	// Lookup correct points to use
	unsigned int nPoint1 = silhouette_edges[nEdge][0];
	unsigned int nPoint2 = silhouette_edges[nEdge][1];

	// Select the actual point coordinates
	vPoint1.x = (nPoint1 & MIN_X) ? source_min.x : source_max.x;
	vPoint1.y = (nPoint1 & MIN_Y) ? source_min.y : source_max.y;
	vPoint1.z = (nPoint1 & MIN_Z) ? source_min.z : source_max.z;
	vPoint2.x = (nPoint2 & MIN_X) ? source_min.x : source_max.x;
	vPoint2.y = (nPoint2 & MIN_Y) ? source_min.y : source_max.y;
	vPoint2.z = (nPoint2 & MIN_Z) ? source_min.z : source_max.z;

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
//  Name : testLine ()
/// <summary>
/// Determine whether or not the line passed is within the box.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox_extruded::test_line(const vec3& v1, const vec3& v2) const
{
	unsigned int nCode1 = 0, nCode2 = 0;
	float fDist1, fDist2, t;
	int nSide1, nSide2;
	vec3 vDir, vIntersect;
	unsigned int i;

	// Test each plane
	for(i = 0; i < edge_count; ++i)
	{
		// Classify each point of the line against the plane.
		fDist1 = plane::dot_coord(extruded_planes[i], v1);
		fDist2 = plane::dot_coord(extruded_planes[i], v2);
		nSide1 = (fDist1 >= 0) ? 1 : 0;
		nSide2 = (fDist2 >= 0) ? 1 : 0;

		// Accumulate the classification info to determine
		// if the edge was spanning any of the planes.
		nCode1 |= (nSide1 << i);
		nCode2 |= (nSide2 << i);

		// If the line is completely in front of any plane
		// then it cannot possibly be intersecting.
		if(nSide1 == 1 && nSide2 == 1)
			return false;

		// The line is potentially spanning?
		if(nSide1 ^ nSide2)
		{
			// Compute the point at which the line intersects this plane.
			vDir = v2 - v1;
			t = -plane::dot_coord(extruded_planes[i], v1) / plane::dot_normal(extruded_planes[i], vDir);

			// Truly spanning?
			if((t >= 0.0f) && (t <= 1.0f))
			{
				vIntersect = v1 + (vDir * t);
				if(test_sphere(vIntersect, 0.01f))
					return true;

			} // End if spanning

		} // End if different sides

	} // Next plane

	// Intersecting?
	return (nCode1 == 0) || (nCode2 == 0);
}

//-----------------------------------------------------------------------------
//  Name : testSphere ()
/// <summary>
/// Determine whether or not the sphere passed is within the box.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox_extruded::test_sphere(const vec3& vecCenter, float fRadius) const
{
	unsigned int i;

	// Test box planes
	for(i = 0; i < edge_count; ++i)
	{
		float fDot = plane::dot_coord(extruded_planes[i], vecCenter);

		// Sphere entirely in front of plane
		if(fDot >= fRadius)
			return false;

	} // Next plane

	// Intersects
	return true;
}
}
