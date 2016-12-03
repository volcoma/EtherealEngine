#pragma once
#if !defined(_INS_FRUSTUM_H_)
#define _INS_FRUSTUM_H_

#include "math_types.h"
#include "plane.h"
#include "transform.h"
#include "bbox.h"
#include "bbox_extruded.h"

namespace math
{
using namespace glm;

//-----------------------------------------------------------------------------
// Main class declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : frustum (Class)
/// <summary>
/// Storage for frustum planes / values and wraps up common functionality
/// </summary>
//-----------------------------------------------------------------------------
class frustum
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
    frustum( );
    frustum( const transform & view, const transform & proj, bool _oglNDC);
    frustum( const bbox & sourceBounds );

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
    void							update				( const transform & view, const transform & proj, bool _oglNDC);
    void							setPlanes			( const plane newPlanes[] );
    void							recomputePoints		( );
    VolumeQuery::E					classifyAABB		( const bbox & bounds ) const;
    VolumeQuery::E					classifyAABB		( const bbox & bounds, unsigned int & frustumBits, int & lastOutside ) const;
    VolumeQuery::E					classifySphere		( const vec3 & center, float radius ) const;
	VolumeQuery::E					classifyPlane		( const plane & plane ) const;
    bool							testPoint			( const vec3 & point) const;
    bool							testAABB			( const bbox & bounds ) const;
  
    bool							testExtrudedAABB	( const bbox_extruded & box ) const;
    bool							testSphere			( const vec3 & center, float radius ) const;
    bool							testSweptSphere		( const vec3 & center, float radius, const vec3 & sweepDirection ) const;
    bool							testFrustum			( const frustum & frustum ) const;
    bool							testLine			( const vec3 & v1, const vec3 & v2 ) const;
    frustum             &			mul					( const transform & t );
    //-------------------------------------------------------------------------
	// Public Static Functions
	//-------------------------------------------------------------------------
    static frustum					mul					( frustum f, const transform & t );
	static bool						testOBB             ( frustum f, const bbox & bounds, const transform & t );
	static bool						testExtrudedOBB     ( frustum f, const bbox_extruded & bounds, const transform & t );
	static VolumeQuery::E			classifyOBB			( frustum f, const bbox & bounds, const transform & t );
	static VolumeQuery::E			classifyOBB			( frustum f, const bbox & bounds, const transform & t, unsigned int & frustumBits, int & lastOutside );
    //-------------------------------------------------------------------------
	// Public Operators
	//-------------------------------------------------------------------------
    bool operator == ( const frustum & f ) const;

    //-------------------------------------------------------------------------
	// Public Variables
	//-------------------------------------------------------------------------
    plane		planes[6];      // The 6 planes of the frustum.
    vec3		points[8];      // The 8 corner points of the frustum.
    vec3		position;       // The originating position of the frustum.

private:
    //-------------------------------------------------------------------------
	// Private Static Functions
	//-------------------------------------------------------------------------
    static bool   sweptSphereIntersectPlane( float & t0, float & t1, const plane & plane, const vec3 & center, float radius, const vec3 & sweepDirection );
};

}

#endif