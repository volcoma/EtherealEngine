#include "plane.h"

namespace math
{
float plane::dot( const plane & p, const vec4 & v )
{
	float result = glm::dot(p.data, v);

	return result;
}
float plane::dotCoord( const plane & p, const vec3 & v )
{
	float result = p.data[0] * v[0] + p.data[1] * v[1] + p.data[2] * v[2] + p.data[3];

	return result;
}
float plane::dotNormal( const plane & p, const vec3 & v )
{
	float result = glm::dot(vec3{ p.data }, v);

	return result;
}
plane plane::fromPointNormal( const vec3 & point, const vec3 & normal )
{
	vec3 normalizedNormal = glm::normalize(normal);
	plane result(normalizedNormal.x,
		normalizedNormal.y,
		normalizedNormal.z,
		-glm::dot(point, normalizedNormal));
	return result;
}
plane plane::fromPoints( const vec3 & v1, const vec3 & v2, const vec3 & v3 )
{
	vec3 Normal = glm::normalize(glm::cross(v2 - v1, v3 - v1));
	return fromPointNormal(v1, Normal);
}

plane plane::mul( const plane & p, const mat4 & m )
{
	plane result = m * p.data;
	
	return result;
}

plane plane::normalize(const plane & p)
{
	plane result;
	float distance = sqrt(p.data.x * p.data.x + p.data.y * p.data.y + p.data.z * p.data.z);
	result.data.x = p.data.x / distance;
	result.data.y = p.data.y / distance;
	result.data.z = p.data.z / distance;
	result.data.w = p.data.w / distance;
	return result;
}

plane plane::scale(const plane & p, float s)
{
	plane result = mul(p, glm::scale(vec3{ s, s, s }));

	return result;
}

plane plane::operator*(float s) const
{
	return plane(data * s);
}

plane plane::operator/(float s) const
{
	return plane(data / s);
}

plane & plane::operator/=(float s)
{
	data /= s;
	return *this;
}

plane plane::operator+() const
{
	return *this;
}

plane & plane::operator*=(float s)
{
	data *= s;
	return *this;
}

bool plane::operator==(const plane & p) const
{
	return data == p.data;
}

plane plane::operator-() const
{
	return plane(-data);
}

plane& plane::operator=(const vec4& rhs)
{
	data = rhs;

	return *this;
}

bool plane::operator!=(const plane & p) const
{
	return data != p.data;
}

plane::plane()
{

}

plane::plane(const vec4& p) :
	data(p)
{

}

plane::plane(float _a, float _b, float _c, float _d) :
	data(_a, _b, _c, _d)
{

}

}