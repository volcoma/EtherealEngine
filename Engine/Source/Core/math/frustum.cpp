#include "frustum.h" 

#include <xutility>

namespace math
{
///////////////////////////////////////////////////////////////////////////////
// frustum Member Functions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  Name : frustum () (Default Constructor)
/// <summary>
/// frustum Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
frustum::frustum( ) 
{
	// Initialize values
    memset( planes, 0, 6 * sizeof(plane) );
    memset( points, 0, 8 * sizeof(vec3) );
    position = vec3(0,0,0);
}

//-----------------------------------------------------------------------------
//  Name : frustum () (Constructor)
/// <summary>
/// frustum Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
frustum::frustum( const transform & View, const transform & Proj )
{
	update( View, Proj );
}

//-----------------------------------------------------------------------------
//  Name : frustum () (Constructor)
/// <summary>
/// frustum Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
frustum::frustum( const bbox & AABB )
{
    using namespace VolumeGeometry;
    using namespace VolumePlane;

    // Compute planes
    planes[Left]   = AABB.getPlane( Left );
    planes[Right]  = AABB.getPlane( Right );
    planes[Top]    = AABB.getPlane( Top );
    planes[Bottom] = AABB.getPlane( Bottom );
    planes[Near]   = AABB.getPlane( Near );
    planes[Far]    = AABB.getPlane( Far );
    
    // Compute points
    vec3 e = AABB.getExtents();
    vec3 p = AABB.getCenter();
    points[LeftBottomNear]  = vec3( p.x - e.x, p.y - e.y, p.z + e.z );
    points[LeftBottomFar]   = vec3( p.x - e.x, p.y - e.y, p.z + e.z );
    points[RightBottomNear] = vec3( p.x + e.x, p.y - e.y, p.z - e.z );
    points[RightBottomFar]  = vec3( p.x + e.x, p.y - e.y, p.z + e.z );
    points[LeftTopNear]     = vec3( p.x - e.x, p.y + e.y, p.z + e.z );
    points[LeftTopFar]      = vec3( p.x - e.x, p.y + e.y, p.z + e.z );
    points[RightTopNear]    = vec3( p.x + e.x, p.y + e.y, p.z - e.z );
    points[RightTopFar]     = vec3( p.x + e.x, p.y + e.y, p.z + e.z );
    position = p;
}

//-----------------------------------------------------------------------------
//  Name : frustum ()
/// <summary>
/// Alternate constructor that builds a frustum for the 90 degree faces
/// of a cube. 
/// Note : Clip plane normals point outwards.
/// </summary>
//-----------------------------------------------------------------------------
frustum::frustum( unsigned int nFrustumIndex, const vec3 & vOrigin, float fFarDistance, float fNearDistance /* = 1.0f */ )
{
    using namespace VolumePlane;

	const float c = 0.7071067812f; // cosine of 45 degrees
	vec3     vNear, vFar;
	plane       ClipPlanes[ 6 ];

	// Choose normals and near/far plane offsets based on frustum index
    switch ( nFrustumIndex )
	{
		case 0: // + X
			ClipPlanes[ Left ]   = plane(  -c,   0,  c, 0);
			ClipPlanes[ Right ]  = plane(  -c,   0, -c, 0);
			ClipPlanes[ Top ]    = plane(  -c,   c,  0, 0);
			ClipPlanes[ Bottom ] = plane(  -c,  -c,  0, 0);
			ClipPlanes[ Near ]   = plane( -1.0f, 0,  0, 0);
			ClipPlanes[ Far ]    = plane(  1.0f, 0,  0, 0);
			vNear = vec3( fNearDistance, 0, 0 );
			vFar  = vec3( fFarDistance, 0, 0 );
		    break;
		case 1: // - X 
			ClipPlanes[ Left ]   = plane(   c,   0, -c, 0);
			ClipPlanes[ Right ]  = plane(   c,   0,  c, 0);
			ClipPlanes[ Top ]    = plane(   c,   c,  0, 0);
			ClipPlanes[ Bottom ] = plane(   c,  -c,  0, 0);
			ClipPlanes[ Near ]   = plane(  1.0f, 0,  0, 0);
			ClipPlanes[ Far ]    = plane( -1.0f, 0,  0, 0);
			vNear = vec3( -fNearDistance, 0, 0 );
			vFar  = vec3( -fFarDistance, 0, 0 );
		    break;
		case 2: // +Y 
			ClipPlanes[ Left ]   = plane(  -c,  -c,  0,  0);
			ClipPlanes[ Right ]  = plane(   c,  -c,  0,  0);
			ClipPlanes[ Top ]    = plane(   0,  -c, -c,  0);
			ClipPlanes[ Bottom ] = plane(   0,  -c,  c,  0);
			ClipPlanes[ Near ]   = plane(   0, -1.0f, 0, 0);
			ClipPlanes[ Far ]    = plane(   0,  1.0f, 0, 0);
			vNear = vec3( 0, fNearDistance, 0 );
			vFar  = vec3( 0, fFarDistance, 0 );
		    break;
		case 3: // -Y
			ClipPlanes[ Left ]   = plane(  -c,   c,     0, 0);
			ClipPlanes[ Right ]  = plane(   c,   c,     0, 0);
			ClipPlanes[ Top ]    = plane(   0,   c,     c, 0);
			ClipPlanes[ Bottom ] = plane(   0,   c,    -c, 0);
			ClipPlanes[ Near ]   = plane(   0,   1.0f,  0, 0);
			ClipPlanes[ Far ]    = plane(   0,  -1.0f,  0, 0);
			vNear = vec3( 0, -fNearDistance, 0 );
			vFar  = vec3( 0, -fFarDistance, 0 );
		    break;
		case 4: // + Z
			ClipPlanes[ Left ]   = plane(  -c,   0, -c,    0);
			ClipPlanes[ Right ]  = plane(   c,   0, -c,    0);
			ClipPlanes[ Top ]    = plane(   0,   c, -c,    0);
			ClipPlanes[ Bottom ] = plane(   0,  -c, -c,    0);
			ClipPlanes[ Near ]   = plane(   0,   0, -1.0f, 0);
			ClipPlanes[ Far ]    = plane(   0,   0,  1.0f, 0);
			vNear = vec3( 0, 0, fNearDistance );
			vFar  = vec3( 0, 0, fFarDistance );
		    break;
		case 5: // - Z
			ClipPlanes[ Left ]   = plane(   c,   0,  c,    0);
			ClipPlanes[ Right ]  = plane(  -c,   0,  c,    0);
			ClipPlanes[ Top ]    = plane(   0,   c,  c,    0);
			ClipPlanes[ Bottom ] = plane(   0,  -c,  c,    0);
			ClipPlanes[ Near ]   = plane(   0,   0,  1.0f, 0);
			ClipPlanes[ Far ]    = plane(   0,   0, -1.0f, 0);
			vNear = vec3( 0, 0, -fNearDistance );
			vFar  = vec3( 0, 0, -fFarDistance );
		    break;

    } // End Switch

	// Compute distances
	ClipPlanes[ Left ].data.w   = -glm::dot( (vec3&)ClipPlanes[ Left ],   vOrigin );
	ClipPlanes[ Right ].data.w = -glm::dot( (vec3&)ClipPlanes[ Right ],  vOrigin );
	ClipPlanes[ Top ].data.w = -glm::dot( (vec3&)ClipPlanes[ Top ],    vOrigin );
	ClipPlanes[ Bottom ].data.w = -glm::dot( (vec3&)ClipPlanes[ Bottom ], vOrigin );
	ClipPlanes[ Near ].data.w = -glm::dot( (vec3&)ClipPlanes[ Near ],   vOrigin + vNear );
	ClipPlanes[ Far ].data.w = -glm::dot( (vec3&)ClipPlanes[ Far ],    vOrigin + vFar  );
	
	// Update with the new clip planes
	setPlanes( ClipPlanes );
}

//-----------------------------------------------------------------------------
//  Name : update ()
/// <summary>
/// Compute the new frustum details based on the matrices specified.
/// </summary>
//-----------------------------------------------------------------------------
void frustum::update( const transform & View, const transform & Proj )
{
    using namespace VolumePlane;

    // Build a combined view & projection matrix
	transform m = Proj * View;


    // Left clipping plane
	planes[Left].data.x = -(m[0][3] + m[0][0]);
	planes[Left].data.y = -(m[1][3] + m[1][0]);
	planes[Left].data.z = -(m[2][3] + m[2][0]);
	planes[Left].data.w = -(m[3][3] + m[3][0]);
	
	   // Right clipping plane
	planes[Right].data.x = -(m[0][3] - m[0][0]);
	planes[Right].data.y = -(m[1][3] - m[1][0]);
	planes[Right].data.z = -(m[2][3] - m[2][0]);
	planes[Right].data.w = -(m[3][3] - m[3][0]);
	
	   // Top clipping plane
	planes[Top].data.x = -(m[0][3] - m[0][1]);
	planes[Top].data.y = -(m[1][3] - m[1][1]);
	planes[Top].data.z = -(m[2][3] - m[2][1]);
	planes[Top].data.w = -(m[3][3] - m[3][1]);
	
	   // Bottom clipping plane
	planes[Bottom].data.x = -(m[0][3] + m[0][1]);
	planes[Bottom].data.y = -(m[1][3] + m[1][1]);
	planes[Bottom].data.z = -(m[2][3] + m[2][1]);
	planes[Bottom].data.w = -(m[3][3] + m[3][1]);
	
	   // Near clipping plane
	planes[Near].data.x = -(m[0][2]);
	planes[Near].data.y = -(m[1][2]);
	planes[Near].data.z = -(m[2][2]);
	planes[Near].data.w = -(m[3][2]);
	
	   // Far clipping plane
	planes[Far].data.x = -(m[0][3] - m[0][2]);
	planes[Far].data.y = -(m[1][3] - m[1][2]);
	planes[Far].data.z = -(m[2][3] - m[2][2]);
	planes[Far].data.w = -(m[3][3] - m[3][2]);

    // Normalize and compute additional information.
    setPlanes( planes );

    // Compute the originating position of the frustum.
    position  = vec3( View[0][0], View[1][0], View[2][0] ) * -View[3][0];
    position += vec3( View[0][1], View[1][1], View[2][1] ) * -View[3][1];
    position += vec3( View[0][2], View[1][2], View[2][2] ) * -View[3][2];
}

//-----------------------------------------------------------------------------
//  Name : setPlanes ()
/// <summary>
/// Compute the new frustum details based on the six planes specified. 
/// This method automatically recomputes the 8 corner points of the frustum
/// based on the supplied planes.
/// </summary>
//-----------------------------------------------------------------------------
void frustum::setPlanes( const plane _Planes[] )
{
    // Copy and normalize the planes
    for ( int i = 0; i < 6; i++ )
        planes[i] = plane::normalize( _Planes[i] );

    // Recompute the frustum corner points.
    recomputePoints();
}

//-----------------------------------------------------------------------------
//  Name : recomputePoints ()
/// <summary>
/// Recompute the 8 corner points of the frustum based on the supplied planes.
/// </summary>
//-----------------------------------------------------------------------------
void frustum::recomputePoints( )
{

    // Compute the 8 corner points
    for ( int i = 0; i < 8; ++i )
    {
		const plane& p0 = plane::normalize((i & 1) ? planes[VolumePlane::Near] : planes[VolumePlane::Far]);
		const plane& p1 = plane::normalize((i & 2) ? planes[VolumePlane::Top] : planes[VolumePlane::Bottom]);
		const plane& p2 = plane::normalize((i & 4) ? planes[VolumePlane::Left] : planes[VolumePlane::Right]);

        // Compute the point at which the three planes intersect
        float cosTheta, secTheta;
        vec3 n1_n2, n2_n0, n0_n1;  
        vec3 n0( p0.data );
        vec3 n1( p1.data );
        vec3 n2( p2.data );
        
		n1_n2 = glm::cross( n1, n2 );
		n2_n0 = glm::cross( n2, n0 );
		n0_n1 = glm::cross( n0, n1 );
        
        cosTheta = glm::dot( n0, n1_n2 );
        secTheta = 1.0f / cosTheta;
        
        n1_n2     = n1_n2 * p0.data.w;
        n2_n0     = n2_n0 * p1.data.w;
        n0_n1     = n0_n1 * p2.data.w;

        points[i] = -(n1_n2 + n2_n0 + n0_n1) * secTheta;
    
    } // Next Corner
}

//-----------------------------------------------------------------------------
//  Name : classifyAABB ()
/// <summary>
/// Determine whether or not the box passed is within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
VolumeQuery::E frustum::classifyAABB( const bbox & AABB ) const
{
    VolumeQuery::E  Result = VolumeQuery::Inside;
    vec3 NearPoint, FarPoint;
    for ( size_t i = 0; i < 6; i++ )
    {
        // Store the plane
        const plane & plane = planes[i];

        // Calculate near / far extreme points
        if ( plane.data.x > 0.0f ) { FarPoint.x  = AABB.max.x; NearPoint.x = AABB.min.x; }
        else                  { FarPoint.x  = AABB.min.x; NearPoint.x = AABB.max.x; }

        if ( plane.data.y > 0.0f ) { FarPoint.y  = AABB.max.y; NearPoint.y = AABB.min.y; }
        else                  { FarPoint.y  = AABB.min.y; NearPoint.y = AABB.max.y; }

        if ( plane.data.z > 0.0f ) { FarPoint.z  = AABB.max.z; NearPoint.z = AABB.min.z; }
        else                  { FarPoint.z  = AABB.min.z; NearPoint.z = AABB.max.z; }

        // If near extreme point is outside, then the AABB is totally outside the frustum
        if ( plane::dotCoord( plane, NearPoint ) > 0.0f )
            return VolumeQuery::Outside;

        // If far extreme point is outside, then the AABB is intersecting the frustum
        if ( plane::dotCoord( plane, FarPoint ) > 0.0f )
            Result = VolumeQuery::Intersect;

    } // Next plane
    return Result;
}

//-----------------------------------------------------------------------------
//  Name : classifyAABB ()
/// <summary>
/// Determine whether or not the box passed is within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
VolumeQuery::E frustum::classifyOBB( frustum frustum, const bbox & AABB, const transform & t )
{
	transform invTransform = inverse(t);

	frustum.mul(invTransform);

	return frustum.classifyAABB(AABB);
}

//-----------------------------------------------------------------------------
//  Name : classifyAABB ()
/// <summary>
/// Determine whether or not the box passed is within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
VolumeQuery::E frustum::classifyAABB( const bbox & AABB, unsigned int & FrustumBits, int & LastOutside ) const
{
    // If the 'last outside plane' index was specified, test it first!
    vec3 NearPoint, FarPoint;
    VolumeQuery::E Result = VolumeQuery::Inside;
    if ( LastOutside >= 0 && ( ((FrustumBits >> LastOutside) & 0x1) == 0x0 ) )
    {
        const plane & plane = planes[LastOutside];

        // Calculate near / far extreme points
        if ( plane.data.x > 0.0f ) { FarPoint.x  = AABB.max.x; NearPoint.x = AABB.min.x; }
        else                  { FarPoint.x  = AABB.min.x; NearPoint.x = AABB.max.x; }

        if ( plane.data.y > 0.0f ) { FarPoint.y  = AABB.max.y; NearPoint.y = AABB.min.y; }
        else                  { FarPoint.y  = AABB.min.y; NearPoint.y = AABB.max.y; }

        if ( plane.data.z > 0.0f ) { FarPoint.z  = AABB.max.z; NearPoint.z = AABB.min.z; }
        else                  { FarPoint.z  = AABB.min.z; NearPoint.z = AABB.max.z; }

        // If near extreme point is outside, then the AABB is totally outside the frustum
        if ( plane::dotCoord( plane, NearPoint ) > 0.0f )
            return VolumeQuery::Outside;

        // If far extreme point is outside, then the AABB is intersecting the frustum
        if ( plane::dotCoord( plane, FarPoint ) > 0.0f )
            Result = VolumeQuery::Intersect;
        else
            FrustumBits |= (0x1 << LastOutside); // We were totally inside this frustum plane, update our bit set

    } // End if last outside plane specified

    // Loop through all the planes
    for ( size_t i = 0; i < 6; i++ )
    {
        // Check the bit in the uchar passed to see if it should be tested (if it's 1, it's already passed)
        if ( ((FrustumBits >> i) & 0x1) == 0x1 )
            continue;

        // If 'last outside plane' index was specified, skip if it matches the plane index
        if ( LastOutside >= 0 && LastOutside == (int)i )
            continue;

        // Calculate near / far extreme points
        const plane & plane = planes[i];
        if ( plane.data.x > 0.0f ) { FarPoint.x  = AABB.max.x; NearPoint.x = AABB.min.x; }
        else                  { FarPoint.x  = AABB.min.x; NearPoint.x = AABB.max.x; }

        if ( plane.data.y > 0.0f ) { FarPoint.y  = AABB.max.y; NearPoint.y = AABB.min.y; }
        else                  { FarPoint.y  = AABB.min.y; NearPoint.y = AABB.max.y; }

        if ( plane.data.z > 0.0f ) { FarPoint.z  = AABB.max.z; NearPoint.z = AABB.min.z; }
        else                  { FarPoint.z  = AABB.min.z; NearPoint.z = AABB.max.z; }

        // If near extreme point is outside, then the AABB is totally outside the frustum
        if ( plane::dotCoord( plane, NearPoint ) > 0.0f )
        {
            // Update the 'last outside' index and return.
            LastOutside = (int)i;
            return VolumeQuery::Outside;

        } // End if outside frustum plane

        // If far extreme point is outside, then the AABB is intersecting the frustum
        if ( plane::dotCoord( plane, FarPoint ) > 0.0f )
            Result = VolumeQuery::Intersect;
        else
            FrustumBits |= (0x1 << i); // We were totally inside this frustum plane, update our bit set

    } // Next plane

    // None outside
    LastOutside = -1;
    return Result;
}

//-----------------------------------------------------------------------------
//  Name : classifyAABB ()
/// <summary>
/// Determine whether or not the box passed is within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
VolumeQuery::E frustum::classifyOBB(frustum frustum, const bbox & AABB, const transform & t, unsigned int & FrustumBits, int & LastOutside)
{
	transform invTransform = inverse(t);

	frustum.mul(invTransform);

	return frustum.classifyAABB(AABB, FrustumBits, LastOutside);
}

//-----------------------------------------------------------------------------
//  Name : testAABB ()
/// <summary>
/// Determine whether or not the box passed is within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
bool frustum::testAABB( const bbox & AABB ) const
{
    // Loop through all the planes
    vec3 NearPoint;
    for ( size_t i = 0; i < 6; i++ )
    {
        const plane & plane = planes[i];

        // Calculate near / far extreme points
        if ( plane.data.x > 0.0f ) NearPoint.x = AABB.min.x;
        else                  NearPoint.x = AABB.max.x;

        if ( plane.data.y > 0.0f ) NearPoint.y = AABB.min.y;
        else                  NearPoint.y = AABB.max.y;

        if ( plane.data.z > 0.0f ) NearPoint.z = AABB.min.z;
        else                  NearPoint.z = AABB.max.z;

        // If near extreme point is outside, then the AABB is totally outside the frustum
        if ( plane::dotCoord( plane, NearPoint ) > 0.0f )
            return false;

    } // Next plane

    // Intersecting / inside
    return true;
}

//-----------------------------------------------------------------------------
//  Name : testAABB ()
/// <summary>
/// Determine whether or not the box passed is within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
bool frustum::testOBB( frustum frustum, const bbox & AABB, const transform & t )
{
	transform invTransform = inverse(t);

	frustum.mul(invTransform);

	return frustum.testAABB(AABB);
}


//-----------------------------------------------------------------------------
//  Name : testExtrudedOBB ()
/// <summary>
/// Determine whether or not the box passed is within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
bool frustum::testExtrudedOBB(frustum frustum, const bbox_extruded & AABB, const transform & t)
{
	transform invTransform = inverse(t);

	frustum.mul(invTransform);

	return frustum.testExtrudedAABB(AABB);
}

//-----------------------------------------------------------------------------
//  Name : testExtrudedAABB()
/// <summary>
/// Determine whether or not the box passed, extruded out away from the
/// specified origin by a required distance, falls within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
bool frustum::testExtrudedAABB( const bbox_extruded & Box ) const
{
    bool     bIntersect1, bIntersect2;
    unsigned int i;

    //  Build an imaginary sphere around the origin, representing the volume of
	//  max attenuation -- if this doesn't intersect the view frustum, then
	//  this caster can be trivially rejected.
	if ( testSphere( Box.projectionPoint, Box.projectionRange ) == false )
		return false;

    // Test frustum edges against extruded box.
    using namespace VolumeGeometry;
    bIntersect1 = (Box.testLine( points[LeftBottomFar], points[LeftBottomNear] )) ||
                  (Box.testLine( points[LeftBottomNear], points[RightBottomNear] )) ||
                  (Box.testLine( points[RightBottomNear], points[RightBottomFar] )) ||
                  (Box.testLine( points[RightBottomFar], points[LeftBottomFar] )) ||
                  (Box.testLine( points[RightBottomFar], points[LeftTopFar] )) ||
                  (Box.testLine( points[RightBottomNear], points[RightTopNear] )) ||
                  (Box.testLine( points[LeftBottomFar], points[LeftTopFar] )) ||
                  (Box.testLine( points[LeftBottomNear], points[LeftTopNear] )) ||
                  (Box.testLine( points[LeftTopNear], points[LeftTopFar] )) ||
                  (Box.testLine( points[LeftTopFar], points[RightTopNear] )) ||
                  (Box.testLine( points[RightTopFar], points[RightTopNear] )) ||
                  (Box.testLine( points[RightTopNear], points[LeftTopNear] ));

	
    // Test extruded box edges against frustum
    bIntersect2 = false;
	for ( i = 0; (i < Box.edgeCount) && (bIntersect1 == false && bIntersect2 == false); ++i )
	{
        vec3 vRay, vPoint1, vPoint2;

        // Retrieve this silhouette edge from the extruded box
        Box.getEdge( i, vPoint1, vPoint2 );
        
        // Build an edge that extends for Box.ProjectionLength distance from 
        // the projection point and test for an intersection against the frustum.
        vRay = glm::normalize( vPoint1 - Box.projectionPoint );
		vRay = Box.projectionPoint + (vRay * Box.projectionRange);    
		bIntersect2 |= testLine( vPoint1, vRay );                     
		vRay = glm::normalize( vPoint2 - Box.projectionPoint );
		vRay = Box.projectionPoint + (vRay * Box.projectionRange);    
		bIntersect2 |= testLine( vPoint2, vRay );
	
    } // Next Extruded Edge

    // Intersects?
	return (bIntersect1 || bIntersect2);

}

//-----------------------------------------------------------------------------
//  Name : classifySphere ()
/// <summary>
/// Determine whether or not the sphere passed is within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
VolumeQuery::E frustum::classifySphere( const vec3 & vecCenter, float fRadius ) const
{
    VolumeQuery::E Result = VolumeQuery::Inside;
    
    // Test frustum planes
    for ( unsigned int i = 0; i < 6; ++i )
    {
        float fDot = plane::dotCoord( planes[i], vecCenter );

        // Sphere entirely in front of plane
        if ( fDot >= fRadius )
            return VolumeQuery::Outside;
        
        // Sphere spans plane
        if ( fDot >= -fRadius )
            Result = VolumeQuery::Intersect;

    } // Next plane

    // Return the result
    return Result;
}

//-----------------------------------------------------------------------------
//  Name : testSphere ()
/// <summary>
/// Determine whether or not the sphere passed is within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
bool frustum::testSphere( const vec3 & vecCenter, float fRadius ) const
{
    // Test frustum planes
    for ( int i = 0; i < 6; ++i )
    {
        float fDot = plane::dotCoord( planes[i], vecCenter );

        // Sphere entirely in front of plane
        if ( fDot >= fRadius )
            return false;

    } // Next plane

    // Intersects
    return true;
}

//-----------------------------------------------------------------------------
//  Name : sweptSphereIntersectPlane () (Private, Static)
/// <summary>
/// Determine whether or not the specified sphere, swept along the
/// provided direction vector, intersects a plane.
/// </summary>
//-----------------------------------------------------------------------------
bool frustum::sweptSphereIntersectPlane( float & t0, float & t1, const plane & plane, const vec3 & vecCenter, float fRadius, const vec3 & vecSweepDirection )
{
    float b_dot_n = plane::dotCoord( plane, vecCenter );
    float d_dot_n = plane::dotNormal( plane, vecSweepDirection );

    if ( d_dot_n == 0.0f )
    {
        if ( b_dot_n <= fRadius )
        {
            //  Effectively infinity
            t0 = 0.0f;
            t1 = FLT_MAX;
            return true;
        
        } // End if infinity
        else
            return false;
    
    } // End if runs parallel to plane
    else
    {
        // Compute the two possible intersections
        float tmp0 = ( fRadius - b_dot_n) / d_dot_n;
        float tmp1 = (-fRadius - b_dot_n) / d_dot_n;
        t0 = glm::min<float>(tmp0, tmp1);
        t1 = glm::max<float>(tmp0, tmp1);
        return true;
    
    } // End if intersection
}

//-----------------------------------------------------------------------------
//  Name : testSweptSphere ()
/// <summary>
/// Determine whether or not the specified sphere, swept along the
/// provided direction vector, intersects the frustum in some way.
/// </summary>
//-----------------------------------------------------------------------------
bool frustum::testSweptSphere( const vec3 & vecCenter, float fRadius, const vec3 & vecSweepDirection ) const
{
    unsigned int  i, nCount = 0;
    float   t0, t1, fDisplacedRadius;
    float   pDisplacements[12];
    vec3 vDisplacedCenter;

    // Determine all 12 intersection points of the swept sphere with the view frustum.
    for ( i = 0; i < 6; ++i )
    {
        // Intersects frustum plane?
        if ( sweptSphereIntersectPlane( t0, t1, planes[i], vecCenter, fRadius, vecSweepDirection) == true )
        {
            // TODO: Possibly needs to be < 0?
            if ( t0 >= 0.0f )
                pDisplacements[nCount++] = t0;
            if ( t1 >= 0.0f )
                pDisplacements[nCount++] = t1;
        
        } // End if intersects
    
    } // Next plane

    // For all points > 0, displace the sphere along the sweep direction. If the displaced
    // sphere falls inside the frustum then we have an intersection.
    for ( i = 0; i < nCount; ++i )
    {
        vDisplacedCenter = vecCenter + (vecSweepDirection * pDisplacements[i]);
        fDisplacedRadius = fRadius * 1.1f; // Tolerance.
        if ( testSphere( vDisplacedCenter, fDisplacedRadius ) == true )
            return true;
    
    } // Next Intersection
    
    // None of the displaced spheres intersected the frustum
    return false;
}

//-----------------------------------------------------------------------------
//  Name : testPoint ()
/// <summary>
/// Determine whether or not the specified point falls within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
bool frustum::testPoint( const vec3 & vecPoint ) const
{
    return testSphere( vecPoint, 0.0f );
}

//-----------------------------------------------------------------------------
//  Name : testLine ()
/// <summary>
/// Determine whether or not the line passed is within the frustum.
/// </summary>
//-----------------------------------------------------------------------------
bool frustum::testLine( const vec3 & v1, const vec3 & v2 ) const
{
    unsigned int   nCode1 = 0, nCode2 = 0;
    float   fDist1, fDist2, t;
    int       nSide1, nSide2;
    vec3 vDir, vIntersect;
    unsigned int  i;

    // Test each plane
    for ( i = 0; i < 6; ++i )
    {
        // Classify each point of the line against the plane.
        fDist1 = plane::dotCoord( planes[i], v1 );
        fDist2 = plane::dotCoord( planes[i], v2 );
        nSide1 = (fDist1 >= 0) ? 1 : 0;
        nSide2 = (fDist2 >= 0) ? 1 : 0;

        // Accumulate the classification info to determine 
        // if the edge was spanning any of the planes.
        nCode1 |= (nSide1 << i);
        nCode2 |= (nSide2 << i);

        // If the line is completely in front of any plane
        // then it cannot possibly be intersecting.
        if ( nSide1 == 1 && nSide2 == 1 )
            return false;

        // The line is potentially spanning?
        if ( nSide1 ^ nSide2 )
        {
            // Compute the point at which the line intersects this plane.
            vDir = v2 - v1;
            t    = -plane::dotCoord( planes[i], v1 ) / plane::dotNormal( planes[i], vDir );
            
            // Truly spanning?
            if ( (t >= 0.0f) && (t <= 1.0f) )
            {
                vIntersect = v1 + (vDir * t);
                if ( testSphere( vIntersect, 0.01f ) )
                    return true;
            
            } // End if spanning
        
        } // End if different sides
    
    } // Next plane
    
    // Intersecting?
    return (nCode1 == 0) || (nCode2 == 0);
}

//-----------------------------------------------------------------------------
//  Name : classifyPlane ()
/// <summary>
/// Classify the frustum with respect to the plane
/// </summary>
//-----------------------------------------------------------------------------
VolumeQuery::E frustum::classifyPlane( const plane & plane ) const
{
	unsigned int nInFrontCount = 0;
	unsigned int nBehindCount  = 0;

    // Test frustum points
    for ( unsigned int i = 0; i < 8; ++i )
    {
		float fDot = plane::dotCoord( plane, points[ i ] );
        if ( fDot > 0.0f )
			nInFrontCount++;
		else if ( fDot < 0.0f )
			nBehindCount++;

    } // Next plane

    // frustum entirely in front of plane
    if ( nInFrontCount == 8 )
        return VolumeQuery::Outside;
    
    // frustum entire behind plane
    if ( nBehindCount == 8 )
        return VolumeQuery::Inside;

    // Return intersection (spanning the plane)
    return VolumeQuery::Intersect;
}


//-----------------------------------------------------------------------------
//  Name : testFrustum ()
/// <summary>
/// Determine whether or not the frustum passed is within this one.
/// </summary>
//-----------------------------------------------------------------------------
bool frustum::testFrustum( const frustum & f ) const
{
    using namespace VolumeGeometry;
    
    // A -> B
    bool bIntersect1;
    bIntersect1 = testLine( f.points[LeftBottomFar], f.points[LeftBottomNear] ) ||
                  testLine( f.points[LeftBottomNear], f.points[RightBottomNear] ) ||
                  testLine( f.points[RightBottomNear], f.points[RightBottomFar] ) ||
                  testLine( f.points[RightBottomFar], f.points[LeftBottomFar] ) ||
                  testLine( f.points[RightBottomFar], f.points[RightTopFar] ) ||
                  testLine( f.points[RightBottomNear], f.points[RightTopNear] ) ||
                  testLine( f.points[LeftBottomFar], f.points[LeftTopFar] ) ||
                  testLine( f.points[LeftBottomNear], f.points[LeftTopNear] ) ||
                  testLine( f.points[LeftTopNear], f.points[LeftTopFar] ) ||
                  testLine( f.points[LeftTopFar], f.points[RightTopFar] ) ||
                  testLine( f.points[RightTopFar], f.points[RightTopNear] ) ||
                  testLine( f.points[RightTopNear], f.points[LeftTopNear] );
    
    // Early out
    if ( bIntersect1 )
        return true;
    
    // B -> A
    bool bIntersect2;
    bIntersect2 = f.testLine( points[LeftBottomFar], points[LeftBottomNear] ) ||
                  f.testLine( points[LeftBottomNear], points[RightBottomNear] ) ||
                  f.testLine( points[RightBottomNear], points[RightBottomFar] ) ||
                  f.testLine( points[RightBottomFar], points[LeftBottomFar] ) ||
                  f.testLine( points[RightBottomFar], points[LeftTopFar] ) ||
                  f.testLine( points[RightBottomNear], points[RightTopNear] ) ||
                  f.testLine( points[LeftBottomFar], points[LeftTopFar] ) ||
                  f.testLine( points[LeftBottomNear], points[LeftTopNear] ) ||
                  f.testLine( points[LeftTopNear], points[LeftTopFar] ) ||
                  f.testLine( points[LeftTopFar], points[RightTopNear] ) ||
                  f.testLine( points[RightTopFar], points[RightTopNear] ) ||
                  f.testLine( points[RightTopNear], points[LeftTopNear] );
    
    // Return intersection result
    return bIntersect2;

}

//-----------------------------------------------------------------------------
//  Name : transform ()
/// <summary>
/// Transforms this frustum by the specified matrix.
/// </summary>
//-----------------------------------------------------------------------------
frustum& frustum::mul( const transform & t )
{
    plane NewPlane;
    transform mtxIT, mtx = t;
	mtxIT = inverse(mtx);
	mtxIT = transpose(mtxIT);

    // transform planes
    for ( unsigned int i = 0; i < 6; ++i )
    {   
        NewPlane = plane::mul( planes[i], mtxIT );
        planes[i] = plane::normalize( NewPlane );
    
    } // Next plane

    // transform points
    for ( unsigned int i = 0; i < 8; ++i )
        points[i] = transform::transformCoord( points[i], mtx );

    // transform originating position.
    position = transform::transformCoord( position, mtx );

    // Return reference to self.
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : transform () (Static)
/// <summary>
/// Transforms the specified frustum by the provide matrix and return the new
/// resulting frustum as a copy.
/// </summary>
//-----------------------------------------------------------------------------
frustum frustum::mul( frustum f, const transform & t )
{
    return f.mul( t );
}

//-----------------------------------------------------------------------------
//  Name : operator== ( const frustum& )
/// <summary>
/// Determine whether or not the two frustums match.
/// </summary>
//-----------------------------------------------------------------------------
bool frustum::operator == ( const frustum & frustum ) const
{
    // Compare planes.
    for ( int i = 0; i < 6; ++i )
    {
        const plane & p1 = planes[i];
        const plane & p2 = frustum.planes[i];
        if ( (glm::abs<float>( p1.data.x - p2.data.x ) <= glm::epsilon<float>() 
			&& glm::abs<float>( p1.data.y - p2.data.y ) <= glm::epsilon<float>()
			&& glm::abs<float>( p1.data.z - p2.data.z ) <= glm::epsilon<float>()
			&& glm::abs<float>( p1.data.w - p2.data.w ) <= glm::epsilon<float>()) == false )
              return false;

    } // Next plane

    // Match
    return true;
}

}