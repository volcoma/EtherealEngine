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
	class transform_t
	{
	public:
		//-------------------------------------------------------------------------
		// Constructors & Destructors
		//-------------------------------------------------------------------------
		transform_t();
		transform_t(const transform_t & t);
		transform_t(const vec3 & translation);
		transform_t(const quat & orientation, const vec3 & translation);
		transform_t(const mat4 & m);

		//-------------------------------------------------------------------------
		// Public Methods
		//-------------------------------------------------------------------------
		vec3 transform_coord(const vec3 & v) const;
		vec3 inverse_transform_coord(const vec3 & v) const;
		vec3 transform_normal(const vec3 & v) const;
		vec3 inverse_transform_normal(const vec3 & v) const;
		bool decompose(vec3 & scale, vec3 & shear, quat & rotation, vec3 & translation) const;
		bool decompose(vec3 & scale, quat & rotation, vec3 & translation) const;
		bool decompose(quat & rotation, vec3 & translation) const;
		transform_t& invert();

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
		transform_t& rotate_axis(float a, const vec3 & v);
		transform_t& rotate(float x, float y, float z);
		transform_t& rotate(const vec3 & v);
		transform_t& rotate_local(float x, float y, float z);
		transform_t& rotate_local(const vec3 & v);
		transform_t& scale(const vec3 & v);
		transform_t& scale(float x, float y, float z);
		transform_t& translate(float x, float y, float z);
		transform_t& translate(const vec3 & v);
		transform_t& translate_local(float x, float y, float z);
		transform_t& translate_local(const vec3 & v);
		transform_t& set_position(float x, float y, float z);
		transform_t& set_position(const vec3 & v);
		transform_t& set_scale(float x, float y, float z);
		transform_t& set_scale(const vec3 & v);
		transform_t& set_shear(float xy, float xz, float yz);
		transform_t& set_rotation(const vec3 & x, const vec3 & y, const vec3 & z);
		transform_t& set_rotation(const quat & q);

		// Full Re-Populate
		transform_t& zero();
		transform_t& compose(const vec3 & scale, const vec3 & shear, const quat & rotation, const vec3 & translation);
		transform_t& compose(const vec3 & scale, const quat & rotation, const vec3 & translation);
		transform_t& compose(const quat & rotation, const vec3 & translation);
		transform_t& scaling(float x, float y, float z);
		transform_t& rotation(float x, float y, float z);
		transform_t& rotation_axis(float angle, const vec3 & axis);
		transform_t& translation(float x, float y, float z);
		transform_t& translation(const vec3 & v);
		transform_t& look_at(const vec3 & eye, const vec3 & at);
		transform_t& look_at(const vec3 & eye, const vec3 & at, const vec3 & upAlign);

		// Comparisons
		int compare(const transform_t & t) const;
		int compare(const transform_t & t, float tolerance) const;
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

		transform_t& operator=(const mat4 & m);
		bool operator==(const transform_t & t) const;
		bool operator!=(const transform_t & t) const;

		// Assignment operators
		transform_t& operator *=(const transform_t & t);
		transform_t& operator *=(float f);
		transform_t& operator +=(const transform_t & t);
		transform_t& operator -=(const transform_t & t);

		// Binary operators
		transform_t operator *(const transform_t & t) const;
		vec4 operator *(const vec4& v) const;
		transform_t operator *(float f) const;
		transform_t operator +(const transform_t & t) const;
		transform_t operator -(const transform_t & t) const;

		//-------------------------------------------------------------------------
		// Public Static Functions
		//-------------------------------------------------------------------------
		static vec3 transform_coord(const vec3 & v, const transform_t & t);
		static vec3 inverse_transform_coord(const vec3 & v, const transform_t & t);
		static vec3 transform_normal(const vec3 & v, const transform_t & t);
		static vec3 inverse_transform_normal(const vec3 & v, const transform_t & t);
		static bool decompose(vec3 & scale, vec3 & shear, quat & rotation, vec3 & translation, const transform_t & t);
		static bool decompose(vec3 & scale, quat & rotation, vec3 & translation, const transform_t & t);
		static bool decompose(quat & rotation, vec3 & translation, const transform_t & t);
		static transform_t lerp(transform_t & t1, transform_t & t2, float dt);
		//-------------------------------------------------------------------------
		// Public Static Variables
		//-------------------------------------------------------------------------
		static const transform_t identity;

	private:
		vec3& position();
		//-------------------------------------------------------------------------
		// Protected Variables
		//-------------------------------------------------------------------------
		mat4 _matrix;
	};

	transform_t inverse(transform_t const& t);
	transform_t transpose(transform_t const& t);

}