#pragma once
#if !defined(_INS_EXTRUDED_BOUNDING_BOX_H_)
#define _INS_EXTRUDED_BOUNDING_BOX_H_
#include "math_types.h"
#include "transform.h"
#include "plane.h"
#include "bbox.h"
namespace math
{
using namespace glm;

//-----------------------------------------------------------------------------
// Main class declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : bbox_extruded (Class)
/// <summary>
/// Storage for extruded box values and wraps up common functionality
/// </summary>
//-----------------------------------------------------------------------------
struct bbox_extruded
{
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
    bbox_extruded( );
    bbox_extruded( const bbox & sourceBounds, const vec3 & origin, float range, const transform * tr = nullptr );

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
    void            reset       ( );
    void            extrude     ( const bbox & sourceBounds, const vec3 & origin, float range, const transform * tr = nullptr );
    bool            getEdge     ( unsigned int edge, vec3 & v1, vec3 & v2 ) const;
    bool            testLine    ( const vec3 & v1, const vec3 & v2 ) const;
    bool            testSphere  ( const vec3 & center, float radius ) const;
    
    //-------------------------------------------------------------------------
	// Public Variables
	//-------------------------------------------------------------------------
    vec3				sourceMin;              // Source bounding box minimum extents
	vec3				sourceMax;              // Source bounding box maximum extents
    vec3				projectionPoint;        // Origin of the extrusion / projection.
    float				projectionRange;        // Distance to extrude / project.
    plane				extrudedPlanes[6];      // The 6 final extruded planes.
	unsigned int        silhouetteEdges[6][2];  // Flags denoting the points used for each extruded (silhouette) edge.
    unsigned int        edgeCount;              // Number of edges extruded
};

}

#endif