#include "bsphere.h"
#include <limits>
namespace math
{
///////////////////////////////////////////////////////////////////////////////
// Static Member Definitions
///////////////////////////////////////////////////////////////////////////////
bsphere bsphere::empty( 0, 0, 0, 0 );
#define BIGNUMBER 100000000.0  		// Hundred million

///////////////////////////////////////////////////////////////////////////////
// BoundingSphere Member Functions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  Name : fromPoints ()
/// <summary>
/// Calculates a tight fitting bounding sphere from the points supplied.
/// </summary>
//-----------------------------------------------------------------------------
bsphere& bsphere::from_points( const char * point_buffer, unsigned int point_count, unsigned int point_stride )
{
	static const float big_number = std::numeric_limits<float>::max();
    vec3 xmin(big_number, big_number, big_number),
              xmax(-big_number, -big_number, -big_number),
              ymin(big_number, big_number, big_number),
              ymax(-big_number, -big_number, -big_number),
              zmin(big_number, big_number, big_number),
              zmax(-big_number, -big_number, -big_number),
              dia1,dia2;

    // FIRST PASS: find 6 minima/maxima points
    const char * points = point_buffer;
    for ( unsigned int i = 0; i < point_count; ++i, points += point_stride)
    {
        const vec3 & p = *(vec3*)points;
        if (p.x<xmin.x) xmin = p; // New xminimum point
        if (p.x>xmax.x) xmax = p;
        if (p.y<ymin.y) ymin = p;
        if (p.y>ymax.y) ymax = p;
        if (p.z<zmin.z) zmin = p;
        if (p.z>zmax.z) zmax = p;
    }

    // Set xspan = distance between the 2 points xmin & xmax (squared)
    float dx = xmax.x - xmin.x;
    float dy = xmax.y - xmin.y;
    float dz = xmax.z - xmin.z;
    float xspan = dx*dx + dy*dy + dz*dz;

    // Same for y & z spans
    dx = ymax.x - ymin.x;
    dy = ymax.y - ymin.y;
    dz = ymax.z - ymin.z;
    float yspan = dx*dx + dy*dy + dz*dz;

    dx = zmax.x - zmin.x;
    dy = zmax.y - zmin.y;
    dz = zmax.z - zmin.z;
    float zspan = dx*dx + dy*dy + dz*dz;

    // Set points dia1 & dia2 to the maximally separated pair
    dia1 = xmin;
    dia2 = xmax; // assume xspan biggest
    float maxspan = xspan;

    if (yspan > maxspan)
    {
        maxspan = yspan;
        dia1 = ymin;
        dia2 = ymax;
    }

    if (zspan > maxspan)
    {
        dia1 = zmin;
        dia2 = zmax;
    }


    // dia1,dia2 is a diameter of initial sphere
    // calc initial center
    position.x = (dia1.x+dia2.x)*0.5f;
    position.y = (dia1.y+dia2.y)*0.5f;
    position.z = (dia1.z+dia2.z)*0.5f;
    // calculate initial radius**2 and radius
    dx = dia2.x-position.x; // x component of radius vector
    dy = dia2.y-position.y; // y component of radius vector
    dz = dia2.z-position.z; // z component of radius vector
    float radiusSq = dx*dx + dy*dy + dz*dz;
	radius = glm::sqrt(radiusSq);

    // SECOND PASS: increment current sphere
    points = point_buffer;
    for ( unsigned int i = 0; i < point_count; ++i, points += point_stride )
    {
        const vec3 & p = *(vec3*)points;
        dx = p.x-position.x;
        dy = p.y-position.y;
        dz = p.z-position.z;
        float old_to_p_sq = dx*dx + dy*dy + dz*dz;
        if (old_to_p_sq > radiusSq) // do r**2 test first
        {
            // this point is outside of current sphere
            float old_to_p = glm::sqrt(old_to_p_sq);
            // calc radius of new sphere
            radius = (radius + old_to_p) * 0.5f;
            radiusSq = radius*radius; 	// for next r**2 compare
            float old_to_new = old_to_p - radius;
            // calc center of new sphere
            float recip = 1.0f /old_to_p;

            position.x = (radius*position.x + old_to_new*p.x) * recip;
            position.y = (radius*position.y + old_to_new*p.y) * recip;
            position.z = (radius*position.z + old_to_new*p.z) * recip;
        }
    }
    return *this;
}

}