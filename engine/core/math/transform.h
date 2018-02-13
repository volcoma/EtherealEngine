#pragma once
//-----------------------------------------------------------------------------
// transform Header Includes
//-----------------------------------------------------------------------------
#include "glm_includes.h"

namespace math
{
using namespace glm;
//-----------------------------------------------------------------------------
// Main class declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : transform (Class)
/// <summary>
/// General purpose transformation class designed to maintain each component of
/// the transformation separate (translation, rotation, scale and shear) whilst
/// providing much of the same functionality provided by standard matrices.
/// </summary>
//-----------------------------------------------------------------------------
class transform
{
public:
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	transform();
	transform(const transform& t);
	transform(const vec3& translation);
	transform(const quat& orientation, const vec3& translation);
	transform(const mat4& m);

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	vec3 transform_coord(const vec3& v) const;
	vec3 inverse_transform_coord(const vec3& v) const;
	vec3 transform_normal(const vec3& v) const;
	vec3 inverse_transform_normal(const vec3& v) const;
	bool decompose(vec3& scale, vec3& shear, quat& rotation, vec3& translation) const;
	bool decompose(vec3& scale, quat& rotation, vec3& translation) const;
	bool decompose(quat& rotation, vec3& translation) const;

	const vec3& get_position() const;
	vec3 get_scale() const;
	vec3 get_shear() const;
	quat get_rotation() const;
	const vec3& x_axis() const;
	const vec3& y_axis() const;
	const vec3& z_axis() const;
	vec3 x_unit_axis() const;
	vec3 y_unit_axis() const;
	vec3 z_unit_axis() const;
	const mat4& matrix() const;
	mat4& matrix();

	// transformation Operations
	transform& rotate_axis(float a, const vec3& v);
	transform& rotate(float x, float y, float z);
	transform& rotate(const vec3& v);
	transform& rotate_local(float x, float y, float z);
	transform& rotate_local(const vec3& v);
	transform& scale(const vec3& v);
	transform& scale(float x, float y, float z);
	transform& translate(float x, float y, float z);
	transform& translate(const vec3& v);
	transform& translate_local(float x, float y, float z);
	transform& translate_local(const vec3& v);
	transform& set_position(float x, float y, float z);
	transform& set_position(const vec3& v);
	transform& set_scale(float x, float y, float z);
	transform& set_scale(const vec3& v);
	transform& set_shear(float xy, float xz, float yz);
	transform& set_rotation(const vec3& x, const vec3& y, const vec3& z);
	transform& set_rotation(const quat& q);

	// Full Re-Populate
	transform& zero();
	transform& compose(const vec3& scale, const vec3& shear, const quat& rotation, const vec3& translation);
	transform& compose(const vec3& scale, const quat& rotation, const vec3& translation);
	transform& compose(const quat& rotation, const vec3& translation);
	transform& scaling(float x, float y, float z);
	transform& rotation(float x, float y, float z);
	transform& rotation_axis(float angle, const vec3& axis);
	transform& translation(float x, float y, float z);
	transform& translation(const vec3& v);
	transform& look_at(const vec3& eye, const vec3& at);
	transform& look_at(const vec3& eye, const vec3& at, const vec3& upAlign);

	// Comparisons
	int compare(const transform& t) const;
	int compare(const transform& t, float tolerance) const;
	bool is_identity() const;

	//-------------------------------------------------------------------------
	// Public Operator Overloads
	//-------------------------------------------------------------------------
	operator const mat4&() const;
	operator mat4*();
	operator const mat4*() const;
	operator float*();
	operator const float*() const;

	mat4::col_type& operator[](mat4::length_type i);
	mat4::col_type const& operator[](mat4::length_type i) const;

	transform& operator=(const mat4& m);
	bool operator==(const transform& t) const;
	bool operator!=(const transform& t) const;

	// Assignment operators
	transform& operator*=(const transform& t);
	transform& operator*=(float f);
	transform& operator+=(const transform& t);
	transform& operator-=(const transform& t);

	// Binary operators
	transform operator*(const transform& t) const;
	vec4 operator*(const vec4& v) const;
	transform operator*(float f) const;
	transform operator+(const transform& t) const;
	transform operator-(const transform& t) const;

	//-------------------------------------------------------------------------
	// Public Static Functions
	//-------------------------------------------------------------------------
	static vec3 transform_coord(const vec3& v, const transform& t);
	static vec3 inverse_transform_coord(const vec3& v, const transform& t);
	static vec3 transform_normal(const vec3& v, const transform& t);
	static vec3 inverse_transform_normal(const vec3& v, const transform& t);
	static bool decompose(vec3& scale, vec3& shear, quat& rotation, vec3& translation, const transform& t);
	static bool decompose(vec3& scale, quat& rotation, vec3& translation, const transform& t);
	static bool decompose(quat& rotation, vec3& translation, const transform& t);
	static transform lerp(transform& t1, transform& t2, float dt);
	//-------------------------------------------------------------------------
	// Public Static Variables
	//-------------------------------------------------------------------------
	static const transform identity;

private:
	vec3& position();
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------
	mat4 matrix_;
};

transform inverse(transform const& t);
transform transpose(transform const& t);
}
