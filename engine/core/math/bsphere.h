#pragma once
#include "glm_includes.h"
#include "math_types.h"
namespace math
{
using namespace glm;
//-----------------------------------------------------------------------------
// Main class declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : bsphere (Class)
/// <summary>
/// Provides storage for common representation of spherical bounding volume,
/// and wraps up common functionality.
/// </summary>
//-----------------------------------------------------------------------------
class bsphere
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	bsphere() : position(0.0f, 0.0f, 0.0f), radius(0.0f)
	{
	}
	bsphere(const vec3& _position, float _radius) : position(_position), radius(_radius)
	{
	}
	bsphere(float x, float y, float z, float _radius) : position(x, y, z), radius(_radius)
	{
	}

	//-------------------------------------------------------------------------
	// Public Inline Methods
	//-------------------------------------------------------------------------
	//-------------------------------------------------------------------------
	//  Name : contains_point()
	/// <summary>
	/// Tests to see if the specified point falls within this bounding sphere
	/// or not. A point precisely on the boundary is also considered to be
	/// contained.
	/// </summary>
	//-------------------------------------------------------------------------
	inline bool contains_point(const vec3& point) const
	{
		return (glm::length2(position - point) < (radius * radius));
	}

	//-------------------------------------------------------------------------
	//  Name : contains_point()
	/// <summary>
	/// Tests to see if the specified point falls within this bounding sphere
	/// or not, taking into account the provided tolerance. A point precisely
	/// on the boundary is also considered to be contained.
	/// </summary>
	//-------------------------------------------------------------------------
	inline bool contains_point(const vec3& point, float tolerance) const
	{
		return (glm::length2(position - point) <= ((radius + tolerance) * (radius + tolerance)));
	}

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	bsphere& from_points(const char* point_buffer, unsigned int point_count, unsigned int point_stride);

	//-------------------------------------------------------------------------
	// Public Inline Operators
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : operator+=()
	/// <summary>
	/// Adjusts the position of the bounding sphere by the specified amount.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bsphere& operator+=(const vec3& shift)
	{
		position += shift;
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator-=()
	/// <summary>
	/// Adjusts the position of the bounding sphere by the specified amount.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bsphere& operator-=(const vec3& shift)
	{
		position -= shift;
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator!=()
	/// <summary>
	/// Test for inequality between this bounding sphere and another.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool operator!=(const bsphere& bounds) const
	{
		return (position != bounds.position || radius != bounds.radius);
	}

	//-----------------------------------------------------------------------------
	//  Name : operator==()
	/// <summary>
	/// Test for equality between this bounding sphere and another.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool operator==(const bsphere& bounds) const
	{
		return (position == bounds.position && radius == bounds.radius);
	}

	//-------------------------------------------------------------------------
	// Public Variables
	//-------------------------------------------------------------------------
	vec3 position;
	float radius;

	//-------------------------------------------------------------------------
	// Public Static Variables
	//-------------------------------------------------------------------------
	static bsphere empty;
};
}
