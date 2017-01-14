#pragma once
#if !defined(_INS_BOUNDING_BOX_H_)
#define _INS_BOUNDING_BOX_H_
//-----------------------------------------------------------------------------
// bbox Header Includes
//-----------------------------------------------------------------------------
#include "math_types.h"
#include "plane.h"
#include "transform.h"
namespace math
{

//-----------------------------------------------------------------------------
// Main class declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : bbox (Class)
/// <summary>
/// Storage for box vector values and wraps up common functionality
/// </summary>
//-----------------------------------------------------------------------------
struct bbox
{
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	bbox							( );
	bbox							( const vec3 & minimum, const vec3 & maximum );
	bbox							( float xMin, float yMin, float zMin, float xMax, float yMax, float zMax );

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
    plane				getPlane				( VolumePlane::Side side ) const;
    void				getPlanePoints			( VolumePlane::Side side, vec3 pointsOut[] ) const;
    bbox&				fromPoints				( const char * pointBuffer, unsigned int pointCount, unsigned int pointStride, bool reset = true );
    bool				intersect				( const bbox & bounds ) const;
    bool				intersect				( const bbox & bounds, bool & contained ) const;
    bool				intersect				( const bbox & bounds, bbox & intersection ) const;
    bool				intersect				( const bbox & bounds, const vec3 & tolerance ) const;
    bool				intersect				( const vec3 & origin, const vec3 & velocity, float & t, bool restrictRange = true ) const;
    bool				intersect				( const vec3 & v0, const vec3 & v1, const vec3 & v2, const bbox & triangleBounds ) const;
	bool				intersect				( const vec3 & v0, const vec3 & v1, const vec3 & v2 ) const;
    bool				containsPoint			( const vec3 & point ) const;
    bool				containsPoint			( const vec3 & point, const vec3 & tolerance ) const;
	bool				containsPoint			( const vec3& Point, float Tolerance ) const;
    vec3				closestPoint			( const vec3 & sourcePoint ) const;
    void				validate				( );
    void				reset					( );
    bbox&				mul						( const transform_t & t );
    void				inflate					( float amount);
    void				inflate					( const vec3 & amount);
    bool				isPopulated				( ) const;
    bool				isDegenerate			( ) const;
    inline float		x						( ) const { return min.x; }
    inline float		y						( ) const { return min.y; }
    inline float		z						( ) const { return min.z; }
    inline float		width					( ) const { return max.x - min.x; }
    inline float		height					( ) const { return max.y - min.y; }
    inline float		depth					( ) const { return max.z - min.z; }

    //-------------------------------------------------------------------------
	// Public Static Functions
	//-------------------------------------------------------------------------
    static bbox			mul						( bbox bounds, const transform_t & t );

    //-------------------------------------------------------------------------
	// Public Operators
	//-------------------------------------------------------------------------
	bbox				operator *				( float scale ) const;
	bbox&				operator +=				( const vec3 & shift );
	bbox&				operator -=				( const vec3 & shift );
	bbox&				operator *=				( const transform_t & t );
	bbox&				operator *=				( float scale );
    bool				operator !=				( const bbox & bounds ) const;
    bool				operator ==				( const bbox & bounds ) const;

    //-------------------------------------------------------------------------
	// Public Inline Methods
	//-------------------------------------------------------------------------
    //-------------------------------------------------------------------------
    //  Name : addPoint ()
    /// <summary>
    /// Grows the bounding box based on the point passed.
    /// </summary>
    //-------------------------------------------------------------------------
    inline bbox & addPoint( const vec3 & Point )
    {
        // Grow by this point
        if ( Point.x < min.x ) min.x = Point.x;
        if ( Point.y < min.y ) min.y = Point.y;
        if ( Point.z < min.z ) min.z = Point.z;
        if ( Point.x > max.x ) max.x = Point.x;
        if ( Point.y > max.y ) max.y = Point.y;
        if ( Point.z > max.z ) max.z = Point.z;
        return *this;
    }

    //-------------------------------------------------------------------------
    //  Name : getDimensions ()
    /// <summary>
    /// Returns a vector containing the dimensions of the bounding box
    /// </summary>
    //-------------------------------------------------------------------------
    inline vec3 getDimensions( ) const
    {
        return max - min;
    }

    //-------------------------------------------------------------------------
    //  Name : getCenter ()
    /// <summary>
    /// Returns a vector containing the exact centre point of the box
    /// </summary>
    //-------------------------------------------------------------------------
    inline vec3 getCenter() const
    {
        return vec3( (max.x + min.x) * 0.5f, (max.y + min.y) * 0.5f, (max.z + min.z) * 0.5f );
    }

    //-------------------------------------------------------------------------
    //  Name : getExtents ()
    /// <summary>
    /// Returns a vector containing the extents of the bounding box (the
    /// half-dimensions).
    /// </summary>
    //-------------------------------------------------------------------------
    inline vec3 getExtents( ) const
    {
        return vec3( (max.x - min.x) * 0.5f, (max.y - min.y) * 0.5f, (max.z - min.z) * 0.5f );
    }
    
    //-------------------------------------------------------------------------
	// Public Variables
	//-------------------------------------------------------------------------
    vec3       min;
	vec3       max;

    //-------------------------------------------------------------------------
	// Public Static Variables
	//-------------------------------------------------------------------------
    static bbox Empty;
};

}
#endif