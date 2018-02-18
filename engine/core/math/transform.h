#pragma once
//-----------------------------------------------------------------------------
// transform Header Includes
//-----------------------------------------------------------------------------
#include "glm_includes.h"
#include "transform_t.hpp"
namespace math
{
using transform = transform_t<float>;

// using namespace glm;
////-----------------------------------------------------------------------------
//// Main class declarations
////-----------------------------------------------------------------------------
////-----------------------------------------------------------------------------
////  Name : transform (Class)
///// <summary>
///// General purpose transformation class designed to maintain each component of
///// the transformation separate (translation, rotation, scale and shear) whilst
///// providing much of the same functionality provided by standard matrices.
///// </summary>
////-----------------------------------------------------------------------------
// class transform
//{
// public:
//	//-------------------------------------------------------------------------
//	// Constructors & Destructors
//	//-------------------------------------------------------------------------
//	transform();
//	transform(const transform& t);
//	transform(const vec3& translation);
//	transform(const quat& orientation, const vec3& translation);
//	transform(const mat4& m);

//	//-------------------------------------------------------------------------
//	// Public Methods
//	//-------------------------------------------------------------------------
//	vec3 transform_coord(const vec3& v) const;
//	vec3 inverse_transform_coord(const vec3& v) const;
//	vec3 transform_normal(const vec3& v) const;
//	vec3 inverse_transform_normal(const vec3& v) const;
//	bool decompose(vec3& scale, vec3& shear, quat& rotation, vec3& translation) const;
//	bool decompose(vec3& scale, quat& rotation, vec3& translation) const;
//	bool decompose(quat& rotation, vec3& translation) const;

//	const vec3& get_position() const;
//	vec3 get_scale() const;
//	vec3 get_shear() const;
//	quat get_rotation() const;
//	const vec3& x_axis() const;
//	const vec3& y_axis() const;
//	const vec3& z_axis() const;
//	vec3 x_unit_axis() const;
//	vec3 y_unit_axis() const;
//	vec3 z_unit_axis() const;
//	const mat4& matrix() const;
//	mat4& matrix();

//	// transformation Operations
//	void rotate_axis(float a, const vec3& v);
//	void rotate(float x, float y, float z);
//	void rotate(const vec3& v);
//	void rotate_local(float x, float y, float z);
//	void rotate_local(const vec3& v);
//	void scale(const vec3& v);
//	void scale(float x, float y, float z);
//	void translate(float x, float y, float z);
//	void translate(const vec3& v);
//	void translate_local(float x, float y, float z);
//	void translate_local(const vec3& v);
//	void set_position(float x, float y, float z);
//	void set_position(const vec3& v);
//	void set_scale(float x, float y, float z);
//	void set_scale(const vec3& v);
//	void set_shear(float xy, float xz, float yz);
//	void set_rotation(const vec3& x, const vec3& y, const vec3& z);
//	void set_rotation(const quat& q);

//	// Full Re-Populate
//	void zero();
//	void compose(const vec3& scale, const vec3& shear, const quat& rotation, const vec3& translation);
//	void compose(const vec3& scale, const quat& rotation, const vec3& translation);
//	void compose(const quat& rotation, const vec3& translation);
//	void scaling(float x, float y, float z);
//	void rotation(float x, float y, float z);
//	void rotation_axis(float angle, const vec3& axis);
//	void translation(float x, float y, float z);
//	void translation(const vec3& v);
//	void look_at(const vec3& eye, const vec3& at);
//	void look_at(const vec3& eye, const vec3& at, const vec3& upAlign);

//	// Comparisons
//	int compare(const transform& t) const;
//	int compare(const transform& t, float tolerance) const;
//	bool is_identity() const;

//	//-------------------------------------------------------------------------
//	// Public Operator Overloads
//	//-------------------------------------------------------------------------
//	operator const mat4&() const;
//	operator mat4*();
//	operator const mat4*() const;
//	operator float*();
//	operator const float*() const;

//	mat4::col_type& operator[](mat4::length_type i);
//	mat4::col_type const& operator[](mat4::length_type i) const;

//	transform& operator=(const mat4& m);
//	bool operator==(const transform& t) const;
//	bool operator!=(const transform& t) const;

//	// Assignment operators
//	transform& operator*=(const transform& t);
//	transform& operator*=(float f);
//	transform& operator+=(const transform& t);
//	transform& operator-=(const transform& t);

//	// Binary operators
//	transform operator*(const transform& t) const;
//	vec4 operator*(const vec4& v) const;
//	transform operator*(float f) const;
//	transform operator+(const transform& t) const;
//	transform operator-(const transform& t) const;

//	//-------------------------------------------------------------------------
//	// Public Static Functions
//	//-------------------------------------------------------------------------
//	static vec3 transform_coord(const vec3& v, const transform& t);
//	static vec3 inverse_transform_coord(const vec3& v, const transform& t);
//	static vec3 transform_normal(const vec3& v, const transform& t);
//	static vec3 inverse_transform_normal(const vec3& v, const transform& t);
//	static bool decompose(vec3& scale, vec3& shear, quat& rotation, vec3& translation, const transform& t);
//	static bool decompose(vec3& scale, quat& rotation, vec3& translation, const transform& t);
//	static bool decompose(quat& rotation, vec3& translation, const transform& t);
//	static transform lerp(transform& t1, transform& t2, float dt);
//	//-------------------------------------------------------------------------
//	// Public Static Variables
//	//-------------------------------------------------------------------------
//	static const transform& identity()
//    {
//        static const transform identity;
//        return identity;
//    }

// private:
//	vec3& position();
//	//-------------------------------------------------------------------------
//	// Protected Variables
//	//-------------------------------------------------------------------------
//	mat4 matrix_;
//};

// transform inverse(transform const& t);
// transform transpose(transform const& t);
}
