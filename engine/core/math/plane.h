#pragma once
#include "glm_includes.h"
namespace math
{
using namespace glm;
//-----------------------------------------------------------------------------
// Main class declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : plane (Class)
/// <summary>
/// Storage for infinite plane.
/// </summary>
//-----------------------------------------------------------------------------
struct plane
{
	//-------------------------------------------------------------------------
	// Friend List
	//-------------------------------------------------------------------------
	friend plane operator*(float, const plane&);

	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	plane();
	plane(const vec4& p);
	plane(float _a, float _b, float _c, float _d);

	//-------------------------------------------------------------------------
	// Public Static Methods
	//-------------------------------------------------------------------------
	static float dot(const plane& p, const vec4& v);
	static float dot_coord(const plane& p, const vec3& v);
	static float dot_normal(const plane& p, const vec3& v);
	static plane from_point_normal(const vec3& point, const vec3& normal);
	static plane from_points(const vec3& v1, const vec3& v2, const vec3& v3);
	static plane mul(const plane& p, const mat4& m);
	static plane normalize(const plane& p);
	static plane scale(const plane& p, float s);

	//-------------------------------------------------------------------------
	// Public Operators
	//-------------------------------------------------------------------------
	plane operator*(float s) const;
	plane operator/(float s) const;
	plane& operator*=(float s);
	plane& operator/=(float s);
	plane operator+() const;
	plane operator-() const;
	bool operator==(const plane& p) const;
	bool operator!=(const plane& p) const;
	plane& operator=(const vec4& rhs);

	//-------------------------------------------------------------------------
	// Public Members
	//-------------------------------------------------------------------------
	vec4 data = { 0.0f, 0.0f, 0.0f, 0.0f };
};

//-----------------------------------------------------------------------------
// Global Inline Operators (plane)
//-----------------------------------------------------------------------------
inline plane operator*(float s, const plane& p)
{
	return plane(p.data * s);
}
}
