#pragma once

namespace math
{
//-----------------------------------------------------------------------------
// Common Global Enumerations
//-----------------------------------------------------------------------------
// Resulting classifications for volume queries such as AABB vs. Frustum, etc.
namespace VolumeQuery
{
    enum E
    {
        Inside = 0,
        Outside,
        Intersect
    };

} // End Namespace : VolumeQuery

namespace PlaneQuery
{
    enum E
    {
        Front = 0,
        Back,
        On,
        Spanning
    };

} // End Namespace : PlaneQuery

// Used to identify specific planes of volumes such as the sides of an AABB or Frustum
namespace VolumePlane
{
    enum Side
    {
        Left    = 0,
        Right,
        Top,
        Bottom,
        Near,
        Far
    };

}; // End Namespace : VolumePlane

// Used to identify specific volume boundary points such as the 8 points of an AABB or Frustum
namespace VolumeGeometry
{
    enum Point
    {
        RightBottomFar  = 0,
        RightBottomNear,
        RightTopFar,
        RightTopNear,
        LeftBottomFar,
        LeftBottomNear,
        LeftTopFar,
        LeftTopNear
    };

}; // End Namespace : VolumeGeometry
}