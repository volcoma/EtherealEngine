#include "bbox.h"
#include <limits>
// #include <memory.h>
// #include <float.h>

namespace math
{
///////////////////////////////////////////////////////////////////////////////
// Static Member Definitions
///////////////////////////////////////////////////////////////////////////////
bbox bbox::Empty( 0, 0, 0, 0, 0, 0 );

///////////////////////////////////////////////////////////////////////////////
// bbox Member Functions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  Name : bbox () (Default Constructor)
/// <summary>
/// bbox Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
bbox::bbox( ) 
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
bbox::bbox( const vec3& vecMin, const vec3& vecMax ) 
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
bbox::bbox( float xMin, float yMin, float zMin, float xMax, float yMax, float zMax )
{
    // Copy coordinate values
    min = vec3( xMin, yMin, zMin );
    max = vec3( xMax, yMax, zMax );
}

//-----------------------------------------------------------------------------
//  Name : reset ()
/// <summary>
/// Resets the bounding box values.
/// </summary>
//-----------------------------------------------------------------------------
void bbox::reset()
{
    min = vec3(  std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max());
	max = vec3( -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max());
}

//-----------------------------------------------------------------------------
//  Name : isPopulated ()
/// <summary>
/// Remains in reset state or has been populated?
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::isPopulated( ) const
{
    if ( min != vec3(std::numeric_limits<float>::max(), std::numeric_limits<float>::max(), std::numeric_limits<float>::max()) ||
         max != vec3( -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max(), -std::numeric_limits<float>::max()) )
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
bool bbox::isDegenerate( ) const
{
    return ( glm::abs<float>( max.x - min.x ) < glm::epsilon<float>() &&
             glm::abs<float>( max.y - min.y ) < glm::epsilon<float>() && 
             glm::abs<float>( max.z - min.z ) < glm::epsilon<float>());
}

//-----------------------------------------------------------------------------
//  Name : getPlane ()
/// <summary>
/// Retrieves the plane for the specified side of the bounding box
/// </summary>
//-----------------------------------------------------------------------------
plane bbox::getPlane( VolumePlane::Side Side ) const
{
    plane BoundsPlane;
	memset( &BoundsPlane, 0, sizeof(plane) );
    
    // Select the requested side
    switch ( Side )
    {
        case VolumePlane::Top:
            BoundsPlane.data.y = 1;
            BoundsPlane.data.w = -max.y;
            break;
        case VolumePlane::Right:
            BoundsPlane.data.x = 1;
            BoundsPlane.data.w = -max.x;
            break;
        case VolumePlane::Far:
            BoundsPlane.data.z = 1;
            BoundsPlane.data.w = -max.z;
            break;
        case VolumePlane::Bottom:
            BoundsPlane.data.y = -1;
            BoundsPlane.data.w = min.y;
            break;
        case VolumePlane::Left:
            BoundsPlane.data.x = -1;
            BoundsPlane.data.w = min.x;
            break;
        case VolumePlane::Near:
            BoundsPlane.data.z = -1;
            BoundsPlane.data.w = min.z;
            break;
    } // End Side Switch

    // Return the plane
    return BoundsPlane;
}

//-----------------------------------------------------------------------------
//  Name : getPlanePoints ()
/// <summary>
/// Retrieves the four points that form the boundary of the specified side of 
/// the bounding box
/// </summary>
//-----------------------------------------------------------------------------
void bbox::getPlanePoints( VolumePlane::Side Side, vec3 PointsOut[] ) const
{
    // Select the requested side
    switch ( Side )
    {
        case VolumePlane::Top:
            PointsOut[0].x = min.x; PointsOut[0].y = max.y; PointsOut[0].z = min.z;
            PointsOut[1].x = min.x; PointsOut[1].y = max.y; PointsOut[1].z = max.z;
            PointsOut[2].x = max.x; PointsOut[2].y = max.y; PointsOut[2].z = max.z;
            PointsOut[3].x = max.x; PointsOut[3].y = max.y; PointsOut[3].z = min.z;
            break;
        case VolumePlane::Right:
            PointsOut[0].x = max.x; PointsOut[0].y = min.y; PointsOut[0].z = min.z;
            PointsOut[1].x = max.x; PointsOut[1].y = max.y; PointsOut[1].z = min.z;
            PointsOut[2].x = max.x; PointsOut[2].y = max.y; PointsOut[2].z = max.z;
            PointsOut[3].x = max.x; PointsOut[3].y = min.y; PointsOut[3].z = max.z;
            break;
        case VolumePlane::Far:
            PointsOut[0].x = max.x; PointsOut[0].y = min.y; PointsOut[0].z = max.z;
            PointsOut[1].x = max.x; PointsOut[1].y = max.y; PointsOut[1].z = max.z;
            PointsOut[2].x = min.x; PointsOut[2].y = max.y; PointsOut[2].z = max.z;
            PointsOut[3].x = min.x; PointsOut[3].y = min.y; PointsOut[3].z = max.z;
            break;
        case VolumePlane::Bottom:
            PointsOut[0].x = min.x; PointsOut[0].y = min.y; PointsOut[0].z = max.z;
            PointsOut[1].x = min.x; PointsOut[1].y = min.y; PointsOut[1].z = min.z;
            PointsOut[2].x = max.x; PointsOut[2].y = min.y; PointsOut[2].z = min.z;
            PointsOut[3].x = max.x; PointsOut[3].y = min.y; PointsOut[3].z = max.z;
            break;
        case VolumePlane::Left:
            PointsOut[0].x = min.x; PointsOut[0].y = min.y; PointsOut[0].z = max.z;
            PointsOut[1].x = min.x; PointsOut[1].y = max.y; PointsOut[1].z = max.z;
            PointsOut[2].x = min.x; PointsOut[2].y = max.y; PointsOut[2].z = min.z;
            PointsOut[3].x = min.x; PointsOut[3].y = min.y; PointsOut[3].z = min.z;
            break;
        case VolumePlane::Near:
            PointsOut[0].x = min.x; PointsOut[0].y = min.y; PointsOut[0].z = min.z;
            PointsOut[1].x = min.x; PointsOut[1].y = max.y; PointsOut[1].z = min.z;
            PointsOut[2].x = max.x; PointsOut[2].y = max.y; PointsOut[2].z = min.z;
            PointsOut[3].x = max.x; PointsOut[3].y = min.y; PointsOut[3].z = min.z;
            break;

    } // End Side Switch
}

//-----------------------------------------------------------------------------
//  Name : fromPoints ()
/// <summary>
/// Calculates the bounding box based on the points specified.
/// </summary>
//-----------------------------------------------------------------------------
bbox& bbox::fromPoints( const unsigned char * pointBuffer, unsigned int pointCount, unsigned int pointStride, bool resetBounds /* = true */ )
{
    // Reset the box if requested
    if ( resetBounds )
        reset();

    // Loop through all the points supplied and grow the box.
    if ( pointBuffer && pointCount )
    {
        for ( unsigned int v = 0; v < pointCount; ++v, pointBuffer += pointStride ) 
            addPoint( *(vec3*)pointBuffer );

    } // End if has data
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
    if ( max.x < min.x ) { rTemp = max.x; max.x = min.x; min.x = rTemp; }
    if ( max.y < min.y ) { rTemp = max.y; max.y = min.y; min.y = rTemp; }
    if ( max.z < min.z ) { rTemp = max.z; max.z = min.z; min.z = rTemp; }
}

//-----------------------------------------------------------------------------
//  Name : intersect()
/// <summary>
/// Tests to see if this AABB is intersected by another AABB
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect( const bbox& Bounds ) const
{
    return (min.x <= Bounds.max.x) && (min.y <= Bounds.max.y) &&
           (min.z <= Bounds.max.z) && (max.x >= Bounds.min.x) &&
           (max.y >= Bounds.min.y) && (max.z >= Bounds.min.z);
}

//-----------------------------------------------------------------------------
//  Name : intersect()
/// <summary>
/// Tests to see if this AABB is intersected by another AABB with full
/// containment test.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect( const bbox& Bounds, bool & bContained ) const
{
    // Set to true by default
    bContained = true;

    // Does the point fall outside any of the AABB planes?
         if ( Bounds.min.x < min.x || Bounds.min.x > max.x ) bContained = false;
    else if ( Bounds.min.y < min.y || Bounds.min.y > max.y ) bContained = false;
    else if ( Bounds.min.z < min.z || Bounds.min.z > max.z ) bContained = false;
    else if ( Bounds.max.x < min.x || Bounds.max.x > max.x ) bContained = false;
    else if ( Bounds.max.y < min.y || Bounds.max.y > max.y ) bContained = false;
    else if ( Bounds.max.z < min.z || Bounds.max.z > max.z ) bContained = false;

    // Return immediately if it's fully contained
    if ( bContained == true ) return true;

    // Perform full intersection test
    return (min.x <= Bounds.max.x) && (min.y <= Bounds.max.y) &&
           (min.z <= Bounds.max.z) && (max.x >= Bounds.min.x) &&
           (max.y >= Bounds.min.y) && (max.z >= Bounds.min.z);
}

//-----------------------------------------------------------------------------
//  Name : intersect()
/// <summary>
/// Tests to see if this AABB is intersected by another AABB and return
/// the resulting intersection.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect( const bbox& Bounds, bbox& Intersection ) const
{
    Intersection.min.x = glm::max( min.x, Bounds.min.x );
    Intersection.min.y = glm::max( min.y, Bounds.min.y );
    Intersection.min.z = glm::max( min.z, Bounds.min.z );
    Intersection.max.x = glm::min( max.x, Bounds.max.x );
    Intersection.max.y = glm::min( max.y, Bounds.max.y );
    Intersection.max.z = glm::min( max.z, Bounds.max.z );

    // Test for intersection
    if ( Intersection.min.x > Intersection.max.x ||
         Intersection.min.y > Intersection.max.y ||
         Intersection.min.z > Intersection.max.z ) return false;

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
bool bbox::intersect( const bbox& Bounds, const vec3& Tolerance ) const
{
	return ((min.x - Tolerance.x) <= (Bounds.max.x + Tolerance.x)) &&
           ((min.y - Tolerance.y) <= (Bounds.max.y + Tolerance.y)) &&
           ((min.z - Tolerance.z) <= (Bounds.max.z + Tolerance.z)) &&
           ((max.x + Tolerance.x) >= (Bounds.min.x - Tolerance.x)) &&
           ((max.y + Tolerance.y) >= (Bounds.min.y - Tolerance.y)) &&
           ((max.z + Tolerance.z) >= (Bounds.min.z - Tolerance.z));
}

//-----------------------------------------------------------------------------
//  Name : intersect ()
/// <summary>
/// This function tests to see if a ray intersects the AABB.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect( const vec3 & Origin, const vec3 & Velocity, float & t, bool RestrictRange /* = true */ ) const
{
    float tMin = std::numeric_limits<float>::min();
    float tMax = std::numeric_limits<float>::max();
    float t1, t2, fTemp;

    // If ray origin is inside bounding box, just return true (treat AABB as a solid box)
	if ( containsPoint( Origin ) == true ) 
	{
		t = 0.0f;
		return true;
	
    } // End if point in box

    // X Slabs
    // Is it pointing toward?
    if ( glm::abs<float>(Velocity.x) > glm::epsilon<float>() )
    {
        fTemp = 1.0f / Velocity.x;
        t1    = (max.x - Origin.x) * fTemp;
        t2    = (min.x - Origin.x) * fTemp;
        
        // Reorder if necessary
        if (t1 > t2) { fTemp = t1; t1 = t2; t2 = fTemp; }
        
        // Compare and validate
        if (t1 > tMin) tMin = t1;
        if (t2 < tMax) tMax = t2;
        if (tMin > tMax) return false;
        if (tMax < 0) return false;
    
    } // End if
    else
    {
        // We cannot be intersecting in this case if the origin is outside of the slab
        //if ( Origin.x < (min.x - Origin.x) || Origin.x > (max.x - Origin.x) )
        if ( Origin.x < min.x || Origin.x > max.x )
            return false;
    
    } // End else
    
    // Y Slabs
    // Is it pointing toward?
    if ( glm::abs<float>(Velocity.y) > glm::epsilon<float>() )
    {
        fTemp = 1.0f / Velocity.y;
        t1    = (max.y - Origin.y) * fTemp;
        t2    = (min.y - Origin.y) * fTemp;
        
        // Reorder if necessary
        if (t1 > t2) { fTemp = t1; t1 = t2; t2 = fTemp; }
        
        // Compare and validate
        if (t1 > tMin) tMin = t1;
        if (t2 < tMax) tMax = t2;
        if (tMin > tMax) return false;
        if (tMax < 0) return false;
    
    } // End if
    else
    {
        // We cannot be intersecting in this case if the origin is outside of the slab
        //if ( Origin.y < (min.y - Origin.y) || Origin.y > (max.y - Origin.y) )
        if ( Origin.y < min.y || Origin.y > max.y )
            return false;
    
    } // End else

    // Z Slabs
    // Is it pointing toward?
    if ( glm::abs<float>(Velocity.z) > glm::epsilon<float>() )
    {
        fTemp = 1.0f / Velocity.z;
        t1    = (max.z - Origin.z) * fTemp;
        t2    = (min.z - Origin.z) * fTemp;
        
        // Reorder if necessary
        if (t1 > t2) { fTemp = t1; t1 = t2; t2 = fTemp; }
        
        // Compare and validate
        if (t1 > tMin) tMin = t1;
        if (t2 < tMax) tMax = t2;
        if (tMin > tMax) return false;
        if (tMax < 0) return false;
    
    } // End if
    else
    {
        // We cannot be intersecting in this case if the origin is outside of the slab
        //if ( Origin.z < (min.z - Origin.z) || Origin.z > (max.z - Origin.z) )
        if ( Origin.z < min.z || Origin.z > max.z )
            return false;
    
    } // End else
    
    // Pick the correct t value
    if ( tMin > 0 ) t = tMin; else t = tMax;

	// Outside our valid range? if yes, return no collide
	if ( t < 0.0f || (RestrictRange == true && t > 1.0f) )
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
bool bbox::intersect( const vec3 & vTri0, const vec3 & vTri1, const vec3 & vTri2, const bbox & TriBounds ) const
{
    // Perform rough "broadphase" rejection by testing for intersection
    // between the supplied triangle bounding box and the source box.
    if ( TriBounds.min.x > max.x || TriBounds.max.x < min.x ||
         TriBounds.min.y > max.y || TriBounds.max.y < min.y ||
         TriBounds.min.z > max.z || TriBounds.max.z < min.z )
        return false;

    // Move everything such that the box center is located at <0,0,0>
    // and the entire test becomes relative to it.
    const vec3     vCenter   = getCenter();
    const vec3     vExtents  = getExtents();
    const vec3     v0        = vTri0 - vCenter;
    const vec3     v1        = vTri1 - vCenter;
    const vec3     v2        = vTri2 - vCenter;

    // Next we need to test to see if the triangle's plane intersects the 
    // source box. Begin by generating the plane itself. Note: We need
    // the edge vectors for later tests, so keep them around.
    
    vec3 vEdge0 = v1 - v0;
    vec3 vEdge1 = v2 - v1;
    vec3 vNormal = glm::cross( vEdge0, vEdge1 );
    float fPlaneDistance = -glm::dot( vNormal, v0 );
    
    // Calculate near / far extreme points
    vec3 vNearPoint, vFarPoint;
    if ( vNormal.x > 0.0f ) { vFarPoint.x  = max.x; vNearPoint.x = min.x; }
    else                    { vFarPoint.x  = min.x; vNearPoint.x = max.x; }
    if ( vNormal.y > 0.0f ) { vFarPoint.y  = max.y; vNearPoint.y = min.y; }
    else                    { vFarPoint.y  = min.y; vNearPoint.y = max.y; }
    if ( vNormal.z > 0.0f ) { vFarPoint.z  = max.z; vNearPoint.z = min.z; }
    else                    { vFarPoint.z  = min.z; vNearPoint.z = max.z; }

    // If near extreme point is outside, then the AABB is totally outside the plane
    if (glm::dot( vNormal, vNearPoint - vCenter ) + fPlaneDistance > 0.0f )
        return false;
        
    // If far extreme point is inside, then the AABB is not intersecting the plane
    if (glm::dot( vNormal, vFarPoint - vCenter ) + fPlaneDistance < 0.0f )
        return false;

    // AXISTEST macro required variables
    vec3 vAbsEdge;
    float fTemp0, fTemp1, fMin, fMax;
    #define AXISTEST( vEdge, vP0, vP1, nComponent0, nComponent1 ) \
        fTemp0 = vEdge[nComponent1] * vP0[nComponent0] - vEdge[nComponent0] * vP0[nComponent1]; \
        fTemp1 = vEdge[nComponent1] * vP1[nComponent0] - vEdge[nComponent0] * vP1[nComponent1]; \
        if ( fTemp0 < fTemp1 ) { fMin = fTemp0; fMax = fTemp1; } else { fMin = fTemp1; fMax = fTemp0; } \
        fTemp0 = vAbsEdge[nComponent1] * vExtents[nComponent0] + vAbsEdge[nComponent0] * vExtents[nComponent1]; \
        if ( fMin > fTemp0 || fMax < -fTemp0 ) return false;
    
    #define AXISTEST2( vEdge, vP0, vP1, nComponent0, nComponent1 ) \
        fTemp0 = -vEdge[nComponent1] * vP0[nComponent0] + vEdge[nComponent0] * vP0[nComponent1]; \
        fTemp1 = -vEdge[nComponent1] * vP1[nComponent0] + vEdge[nComponent0] * vP1[nComponent1]; \
        if ( fTemp0 < fTemp1 ) { fMin = fTemp0; fMax = fTemp1; } else { fMin = fTemp1; fMax = fTemp0; } \
        fTemp0 = vAbsEdge[nComponent1] * vExtents[nComponent0] + vAbsEdge[nComponent0] * vExtents[nComponent1]; \
        if ( fMin > fTemp0 || fMax < -fTemp0 ) return false;

    // Test to see if the triangle edges cross the box.
    vAbsEdge.x = glm::abs<float>(vEdge0.x);
    vAbsEdge.y = glm::abs<float>(vEdge0.y);
    vAbsEdge.z = glm::abs<float>(vEdge0.z);
    AXISTEST( vEdge0, v0, v2, 1, 2 ); // X
    AXISTEST2( vEdge0, v0, v2, 0, 2 ); // Y
    AXISTEST( vEdge0, v2, v1, 0, 1 ); // Z

    vAbsEdge.x = glm::abs<float>(vEdge1.x);
    vAbsEdge.y = glm::abs<float>(vEdge1.y);
    vAbsEdge.z = glm::abs<float>(vEdge1.z);
    AXISTEST( vEdge1, v0, v2, 1, 2 ); // X
    AXISTEST2( vEdge1, v0, v2, 0, 2 ); // Y
    AXISTEST( vEdge1, v0, v1, 0, 1 ); // Z

    const vec3 vEdge2 = v0 - v2;
    vAbsEdge.x = glm::abs<float>(vEdge2.x);
    vAbsEdge.y = glm::abs<float>(vEdge2.y);
    vAbsEdge.z = glm::abs<float>(vEdge2.z);
    AXISTEST( vEdge2, v0, v1, 1, 2 ); // X
    AXISTEST2( vEdge2, v0, v1, 0, 2 ); // Y
    AXISTEST( vEdge2, v2, v1, 0, 1 ); // Z

    // Overlapping
    return true;
}


//-----------------------------------------------------------------------------
//  Name : intersect ()
/// <summary>
/// This function tests to see if a triangle intersects the AABB.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::intersect( const vec3 & vTri0, const vec3 & vTri1, const vec3 & vTri2) const
{
	bbox TriBounds;
	TriBounds.addPoint(vTri0);
	TriBounds.addPoint(vTri1);
	TriBounds.addPoint(vTri2);

	// Perform rough "broadphase" rejection by testing for intersection
	// between the supplied triangle bounding box and the source box.
	if ( TriBounds.min.x > max.x || TriBounds.max.x < min.x ||
		TriBounds.min.y > max.y || TriBounds.max.y < min.y ||
		TriBounds.min.z > max.z || TriBounds.max.z < min.z )
		return false;

	// Move everything such that the box center is located at <0,0,0>
	// and the entire test becomes relative to it.
	const vec3     vCenter   = getCenter();
	const vec3     vExtents  = getExtents();
	const vec3     v0        = vTri0 - vCenter;
	const vec3     v1        = vTri1 - vCenter;
	const vec3     v2        = vTri2 - vCenter;

	// Next we need to test to see if the triangle's plane intersects the 
	// source box. Begin by generating the plane itself. Note: We need
	// the edge vectors for later tests, so keep them around.

	vec3 vEdge0 = v1 - v0;
	vec3 vEdge1 = v2 - v1;
	vec3 vNormal = glm::cross( vEdge0, vEdge1 );
	float fPlaneDistance = -glm::dot( vNormal, v0 );

	// Calculate near / far extreme points
	vec3 vNearPoint, vFarPoint;
	if ( vNormal.x > 0.0f ) { vFarPoint.x  = max.x; vNearPoint.x = min.x; }
	else                    { vFarPoint.x  = min.x; vNearPoint.x = max.x; }
	if ( vNormal.y > 0.0f ) { vFarPoint.y  = max.y; vNearPoint.y = min.y; }
	else                    { vFarPoint.y  = min.y; vNearPoint.y = max.y; }
	if ( vNormal.z > 0.0f ) { vFarPoint.z  = max.z; vNearPoint.z = min.z; }
	else                    { vFarPoint.z  = min.z; vNearPoint.z = max.z; }

	// If near extreme point is outside, then the AABB is totally outside the plane
	if ( glm::dot( vNormal, vNearPoint - vCenter ) + fPlaneDistance > 0.0f )
		return false;

	// If far extreme point is inside, then the AABB is not intersecting the plane
	if ( glm::dot( vNormal, vFarPoint - vCenter ) + fPlaneDistance < 0.0f )
		return false;

	// AXISTEST macro required variables
	vec3 vAbsEdge;
	float fTemp0, fTemp1, fMin, fMax;
#define AXISTEST( vEdge, vP0, vP1, nComponent0, nComponent1 ) \
	fTemp0 = vEdge[nComponent1] * vP0[nComponent0] - vEdge[nComponent0] * vP0[nComponent1]; \
	fTemp1 = vEdge[nComponent1] * vP1[nComponent0] - vEdge[nComponent0] * vP1[nComponent1]; \
	if ( fTemp0 < fTemp1 ) { fMin = fTemp0; fMax = fTemp1; } else { fMin = fTemp1; fMax = fTemp0; } \
	fTemp0 = vAbsEdge[nComponent1] * vExtents[nComponent0] + vAbsEdge[nComponent0] * vExtents[nComponent1]; \
	if ( fMin > fTemp0 || fMax < -fTemp0 ) return false;

#define AXISTEST2( vEdge, vP0, vP1, nComponent0, nComponent1 ) \
	fTemp0 = -vEdge[nComponent1] * vP0[nComponent0] + vEdge[nComponent0] * vP0[nComponent1]; \
	fTemp1 = -vEdge[nComponent1] * vP1[nComponent0] + vEdge[nComponent0] * vP1[nComponent1]; \
	if ( fTemp0 < fTemp1 ) { fMin = fTemp0; fMax = fTemp1; } else { fMin = fTemp1; fMax = fTemp0; } \
	fTemp0 = vAbsEdge[nComponent1] * vExtents[nComponent0] + vAbsEdge[nComponent0] * vExtents[nComponent1]; \
	if ( fMin > fTemp0 || fMax < -fTemp0 ) return false;

	// Test to see if the triangle edges cross the box.
	vAbsEdge.x = glm::abs<float>(vEdge0.x);
	vAbsEdge.y = glm::abs<float>(vEdge0.y);
	vAbsEdge.z = glm::abs<float>(vEdge0.z);
	AXISTEST( vEdge0, v0, v2, 1, 2 ); // X
	AXISTEST2( vEdge0, v0, v2, 0, 2 ); // Y
	AXISTEST( vEdge0, v2, v1, 0, 1 ); // Z

	vAbsEdge.x = glm::abs<float>(vEdge1.x);
	vAbsEdge.y = glm::abs<float>(vEdge1.y);
	vAbsEdge.z = glm::abs<float>(vEdge1.z);
	AXISTEST( vEdge1, v0, v2, 1, 2 ); // X
	AXISTEST2( vEdge1, v0, v2, 0, 2 ); // Y
	AXISTEST( vEdge1, v0, v1, 0, 1 ); // Z

	const vec3 vEdge2 = v0 - v2;
	vAbsEdge.x = glm::abs<float>(vEdge2.x);
	vAbsEdge.y = glm::abs<float>(vEdge2.y);
	vAbsEdge.z = glm::abs<float>(vEdge2.z);
	AXISTEST( vEdge2, v0, v1, 1, 2 ); // X
	AXISTEST2( vEdge2, v0, v1, 0, 2 ); // Y
	AXISTEST( vEdge2, v2, v1, 0, 1 ); // Z

	// Overlapping
	return true;
}

//-----------------------------------------------------------------------------
//  Name : containsPoint()
/// <summary>
/// Tests to see if a point falls within this bounding box or not
/// including a specific tolerance around the box.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::containsPoint( const vec3& Point, float Tolerance ) const
{
	if (Point.x < min.x - Tolerance || Point.x > max.x + Tolerance) return false;
	if (Point.y < min.y - Tolerance || Point.y > max.y + Tolerance) return false;
	if (Point.z < min.z - Tolerance || Point.z > max.z + Tolerance) return false;
	return true;
}


//-----------------------------------------------------------------------------
//  Name : containsPoint()
/// <summary>
/// Tests to see if a point falls within this bounding box or not.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::containsPoint( const vec3& Point ) const
{
    if (Point.x < min.x || Point.x > max.x) return false;
    if (Point.y < min.y || Point.y > max.y) return false;
    if (Point.z < min.z || Point.z > max.z) return false;
    return true;
}

//-----------------------------------------------------------------------------
//  Name : containsPoint()
/// <summary>
/// Tests to see if a point falls within this bounding box or not
/// including a specific tolerance around the box.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::containsPoint( const vec3& Point, const vec3& Tolerance ) const
{
    if (Point.x < min.x - Tolerance.x || Point.x > max.x + Tolerance.x) return false;
    if (Point.y < min.y - Tolerance.y || Point.y > max.y + Tolerance.y) return false;
    if (Point.z < min.z - Tolerance.z || Point.z > max.z + Tolerance.z) return false;
    return true;
}

//-----------------------------------------------------------------------------
//  Name : closestPoint ()
/// <summary>
/// Compute a point, on the surface of the AABB, that falls closest to 
/// the input point.
/// </summary>
//-----------------------------------------------------------------------------
vec3 bbox::closestPoint( const vec3 & TestPoint ) const
{
	vec3 Closest;
	
	// Test X extent
	if ( TestPoint.x < min.x )
		Closest.x = min.x;
	else if ( TestPoint.x > max.x )
		Closest.x = max.x;
	else
		Closest.x = TestPoint.x;
	
	// Test Y extent
	if ( TestPoint.y < min.y )
		Closest.y = min.y;
	else if ( TestPoint.y > max.y )
		Closest.y = max.y;
	else
		Closest.y = TestPoint.y;
	
	// Test Z extent
	if ( TestPoint.z < min.z )
		Closest.z = min.z;
	else if ( TestPoint.z > max.z )
		Closest.z = max.z;
	else
		Closest.z = TestPoint.z;
	
	// Return the closest TestPoint
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
bbox & bbox::mul( const transform_t & t )
{
    vec3 BoundsCenter = getCenter();
    
    // Compute new extents values
    const vec3 Ex = t.xAxis() * (max.x - BoundsCenter.x);
    const vec3 Ey = t.yAxis() * (max.y - BoundsCenter.y);
    const vec3 Ez = t.zAxis() * (max.z - BoundsCenter.z);
	
    // Calculate new extents actual
    const float fEx = glm::abs<float>(Ex.x) + glm::abs<float>(Ey.x) + glm::abs<float>(Ez.x);
    const float fEy = glm::abs<float>(Ex.y) + glm::abs<float>(Ey.y) + glm::abs<float>(Ez.y);
    const float fEz = glm::abs<float>(Ex.z) + glm::abs<float>(Ey.z) + glm::abs<float>(Ez.z);
    
    // Compute new center (we use 'transformNormal' because we only
    // want to apply rotation and scale).
    t.transformNormal( BoundsCenter, BoundsCenter );
    
    // Calculate final bounding box (add on translation)
    const vec3 & vTranslation = t.getPosition();
    min.x = (BoundsCenter.x - fEx) + vTranslation.x;
    min.y = (BoundsCenter.y - fEy) + vTranslation.y;
    min.z = (BoundsCenter.z - fEz) + vTranslation.z;
    max.x = (BoundsCenter.x + fEx) + vTranslation.x;
    max.y = (BoundsCenter.y + fEy) + vTranslation.y;
    max.z = (BoundsCenter.z + fEz) + vTranslation.z;

    // Return reference to self
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : transform () (Static)
/// <summary>
/// Transforms the specified bounding box by the provided matrix and return the
/// new resulting box as a copy.
/// </summary>
//-----------------------------------------------------------------------------
bbox bbox::mul( bbox Bounds, const transform_t & t )
{
    return Bounds.mul( t );
}

//-----------------------------------------------------------------------------
//  Name : inflate()
/// <summary>
/// Grow (or shrink if you specify a negative value) the bounding box
/// by the specified number of world space units on all three axes.
/// </summary>
//-----------------------------------------------------------------------------
void bbox::inflate( float fGrowSize )
{
    min.x -= fGrowSize;
    min.y -= fGrowSize;
    min.z -= fGrowSize;
    max.x += fGrowSize;
    max.y += fGrowSize;
    max.z += fGrowSize;
}

//-----------------------------------------------------------------------------
//  Name : inflate()
/// <summary>
/// Grow (or shrink if you specify a negative value) the bounding box
/// by the specified numbers number of world space units on each of the 
/// three axes independently.
/// </summary>
//-----------------------------------------------------------------------------
void bbox::inflate( const vec3& vecGrowSize )
{
    min.x -= vecGrowSize.x;
    min.y -= vecGrowSize.y;
    min.z -= vecGrowSize.z;
    max.x += vecGrowSize.x;
    max.y += vecGrowSize.y;
    max.z += vecGrowSize.z;
}

//-----------------------------------------------------------------------------
//  Name : operator+=()
/// <summary>
/// Moves the bounding box by the vector passed.
/// </summary>
//-----------------------------------------------------------------------------
bbox& bbox::operator+= ( const vec3& vecShift )
{
    min += vecShift;
    max += vecShift;
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator-=()
/// <summary>
/// Moves the bounding box by the vector passed.
/// </summary>
//-----------------------------------------------------------------------------
bbox& bbox::operator-= ( const vec3& vecShift )
{
    min -= vecShift;
    max -= vecShift;
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator*()
/// <summary>
/// Scales the bounding box values by the scalar passed.
/// </summary>
//-----------------------------------------------------------------------------
bbox bbox::operator* ( float fScale ) const
{
    return bbox( min * fScale, max * fScale );
}

//-----------------------------------------------------------------------------
//  Name : operator*=()
/// <summary>
/// Scales the bounding box values by the scalar passed.
/// </summary>
//-----------------------------------------------------------------------------
bbox& bbox::operator*= ( float fScale )
{
    min *= fScale;
    max *= fScale;
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator*=()
/// <summary>
/// Transforms the bounding box by the matrix passed.
/// </summary>
//-----------------------------------------------------------------------------
bbox& bbox::operator*= ( const transform_t & t )
{
    mul( t );
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator==()
/// <summary>
/// Test for quality between this bounding box and the other.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::operator== ( const bbox& Bounds ) const
{
    return (min == Bounds.min) && (max == Bounds.max);
}

//-----------------------------------------------------------------------------
//  Name : operator!=()
/// <summary>
/// Test for quality between this bounding box and the other.
/// </summary>
//-----------------------------------------------------------------------------
bool bbox::operator!= ( const bbox& Bounds ) const
{
    return (min != Bounds.min) || (max != Bounds.max);
}

}