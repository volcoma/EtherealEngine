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
template <typename T, qualifier Q = defaultp>
class transform_t
{
public:
	using mat4_t = mat<4, 4, T, Q>;
	using vec3_t = vec<3, T, Q>;
	using vec4_t = vec<4, T, Q>;
	using quat_t = tquat<T, Q>;
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	transform_t() = default;
	transform_t(const transform_t& t) = default;
	transform_t(transform_t&& t) = default;
	transform_t& operator=(const transform_t& m) = default;
	transform_t& operator=(transform_t&& m) = default;

	transform_t(const mat4_t& m);

	const vec3_t& get_position() const;
	void set_position(const vec3_t& position);

	vec3_t get_rotation_euler() const;
	void set_rotation_euler(const vec3_t& euler_angles);

	const vec3_t& get_scale() const;
	void set_scale(const vec3_t& scale);

	const quat_t& get_rotation() const;
	void set_rotation(const quat_t& rotation);
	void set_rotation(const vec3_t& x, const vec3_t& y, const vec3_t& z);

	vec3_t x_axis() const;
	vec3_t y_axis() const;
	vec3_t z_axis() const;
	vec3_t x_unit_axis() const;
	vec3_t y_unit_axis() const;
	vec3_t z_unit_axis() const;

	// these transform from the current state
	void rotate_axis(T a, const vec3_t& v);
	void rotate(T x, T y, T z);
	void rotate(const vec3_t& v);
	void rotate_local(T x, T y, T z);
	void rotate_local(const vec3_t& v);
	void scale(T x, T y, T z);
	void scale(const vec3_t& v);
	void translate(T x, T y, T z);
	void translate(const vec3_t& v);
	void translate_local(T x, T y, T z);
	void translate_local(const vec3_t& v);

	int compare(const transform_t& t) const;
	int compare(const transform_t& t, T tolerance) const;
	vec3_t transform_coord(const vec3_t& v) const;
	vec3_t inverse_transform_coord(const vec3_t& v) const;
	vec3_t transform_normal(const vec3_t& v) const;
	vec3_t inverse_transform_normal(const vec3_t& v) const;

	static vec3_t transform_coord(const vec3_t& v, const transform_t& t);
	static vec3_t inverse_transform_coord(const vec3_t& v, const transform_t& t);
	static vec3_t transform_normal(const vec3_t& v, const transform_t& t);
	static vec3_t inverse_transform_normal(const vec3_t& v, const transform_t& t);

	static const transform_t& identity();
	//-------------------------------------------------------------------------
	// Public Operator Overloads
	//-------------------------------------------------------------------------
	operator const mat4_t&() const;
	operator const mat4_t*() const;
	operator const typename mat4_t::value_type*() const;

	transform_t operator*(const transform_t& t) const;
	typename mat4_t::col_type const& operator[](typename mat4_t::length_type i) const
	{
		return get_matrix()[i];
	}

	vec4_t operator*(const vec4_t& v) const
	{
		vec4_t result = get_matrix() * v;
		return result;
	}

	const mat4_t& get_matrix() const
	{
		update_matrix();
		return matrix_;
	}

private:
	void update_components()
	{
		vec3_t skew;
		vec4_t perspective;

		// workaround for decompose when
		// used on projection matrix
		mat4_t m = matrix_;
		m[3][3] = 1;

		glm::decompose(m, scale_, rotation_, position_, skew, perspective);
	}

	void update_matrix() const
	{
		if(dirty_)
		{
			auto translation = glm::translate(position_);
			auto rotation = glm::mat4_cast(rotation_);
			auto scale = glm::scale(scale_);

			matrix_ = translation * rotation * scale;

			dirty_ = false;
		}
	}
	void make_dirty()
	{
		dirty_ = true;
	}
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------
	//this should be always first.
    mutable mat4_t matrix_ = mat4_t(1);

	vec3_t position_ = vec3_t(0, 0, 0);
	quat_t rotation_ = quat_t(1, 0, 0, 0);
	vec3_t scale_ = vec3_t(1, 1, 1);

	mutable bool dirty_ = false;
};

template <typename T, qualifier Q>
transform_t<T, Q> inverse(transform_t<T, Q> const& t)
{
	const auto& m = t.get_matrix();
	return glm::inverse(m);
}

template <typename T, qualifier Q>
transform_t<T, Q> transpose(transform_t<T, Q> const& t)
{
	const auto& m = t.get_matrix();
	return glm::transpose(m);
}

template <typename T, qualifier Q>
inline transform_t<T, Q>::transform_t(const typename transform_t::mat4_t& m)
	: matrix_(m)
	, dirty_(false)
{
	update_components();
}

template <typename T, qualifier Q>
inline const typename transform_t<T, Q>::vec3_t& transform_t<T, Q>::get_position() const
{
	return position_;
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::set_position(const typename transform_t::vec3_t& position)
{
	position_ = position;
	make_dirty();
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t transform_t<T, Q>::get_rotation_euler() const
{
	return eulerAngles(rotation_);
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::set_rotation_euler(const typename transform_t::vec3_t& euler_angles)
{
	set_rotation(quat_t(euler_angles));
}

template <typename T, qualifier Q>
inline const typename transform_t<T, Q>::vec3_t& transform_t<T, Q>::get_scale() const
{
	return scale_;
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::set_scale(const typename transform_t::vec3_t& scale)
{
	scale_ = scale;
	make_dirty();
}

template <typename T, qualifier Q>
inline const typename transform_t<T, Q>::quat_t& transform_t<T, Q>::get_rotation() const
{
	return rotation_;
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::set_rotation(const typename transform_t::quat_t& rotation)
{
	rotation_ = rotation;
	make_dirty();
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::set_rotation(const typename transform_t::vec3_t& x,
											const typename transform_t::vec3_t& y,
											const typename transform_t::vec3_t& z)
{
	// Get current scale so that it can be preserved.
	const auto& scale = get_scale();

	update_matrix();
	// Set the new axis vectors (normalized)
	reinterpret_cast<vec3_t&>(matrix_[0]) = glm::normalize(x);
	reinterpret_cast<vec3_t&>(matrix_[1]) = glm::normalize(y);
	reinterpret_cast<vec3_t&>(matrix_[2]) = glm::normalize(z);

	// Scale back to original length
	reinterpret_cast<vec3_t&>(matrix_[0]) *= scale.x;
	reinterpret_cast<vec3_t&>(matrix_[1]) *= scale.y;
	reinterpret_cast<vec3_t&>(matrix_[2]) *= scale.z;

	update_components();
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t transform_t<T, Q>::x_axis() const
{
	return get_rotation() * vec3_t(1, 0, 0);
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t transform_t<T, Q>::y_axis() const
{
	return get_rotation() * vec3_t(0, 1, 0);
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t transform_t<T, Q>::z_axis() const
{
	return get_rotation() * vec3_t(0, 0, 1);
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t transform_t<T, Q>::x_unit_axis() const
{
	return normalize(x_axis());
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t transform_t<T, Q>::y_unit_axis() const
{
	return normalize(y_axis());
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t transform_t<T, Q>::z_unit_axis() const
{
	return normalize(z_axis());
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::rotate_axis(T a, const typename transform_t::vec3_t& v)
{
	quat_t q = glm::angleAxis(a, v) * get_rotation();
	set_rotation(q);
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::rotate(T x, T y, T z)
{
	rotate({x, y, z});
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::rotate(const typename transform_t::vec3_t& v)
{
	quat_t qx = glm::angleAxis(v.x, vec3_t{1, 0, 0});
	quat_t qy = glm::angleAxis(v.y, vec3_t{0, 1, 0});
	quat_t qz = glm::angleAxis(v.z, vec3_t{0, 0, 1});
	quat_t q = qz * qy * qx * get_rotation();
	set_rotation(q);
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::rotate_local(T x, T y, T z)
{
	rotate_local({x, y, z});
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::rotate_local(const typename transform_t::vec3_t& v)
{
	quat_t qx = glm::angleAxis(v.x, x_unit_axis());
	quat_t qy = glm::angleAxis(v.y, y_unit_axis());
	quat_t qz = glm::angleAxis(v.z, z_unit_axis());
	quat_t q = qz * qy * qx * get_rotation();
	set_rotation(q);
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::scale(T x, T y, T z)
{
	scale({x, y, z});
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::scale(const typename transform_t::vec3_t& v)
{
	set_scale(get_scale() * v);
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::translate(T x, T y, T z)
{
	translate({x, y, z});
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::translate(const typename transform_t::vec3_t& v)
{
	set_position(get_position() + v);
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::translate_local(T x, T y, T z)
{
	translate_local({x, y, z});
}

template <typename T, qualifier Q>
inline void transform_t<T, Q>::translate_local(const typename transform_t::vec3_t& v)
{
	set_position(get_position() + (x_unit_axis() * v.x));
	set_position(get_position() + (y_unit_axis() * v.y));
	set_position(get_position() + (z_unit_axis() * v.z));
}

template <typename T, qualifier Q>
inline int transform_t<T, Q>::compare(const transform_t& t) const
{
	return static_cast<int>(get_matrix() == t.get_matrix());
}

template <typename T, qualifier Q>
inline int transform_t<T, Q>::compare(const transform_t& t, T tolerance) const
{
	const auto& m1 = get_matrix();
	const auto& m2 = t.get_matrix();
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			float diff = m1[i][j] - m2[i][j];
			if(glm::abs<T>(diff) > tolerance)
			{
				return (diff < 0) ? -1 : 1;
			}
		}
	}

	// Equivalent
	return 0;
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t
transform_t<T, Q>::transform_coord(const typename transform_t::vec3_t& v) const
{
	return transform_coord(v, *this);
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t
transform_t<T, Q>::inverse_transform_coord(const typename transform_t::vec3_t& v) const
{
	return inverse_transform_coord(v, *this);
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t
transform_t<T, Q>::transform_normal(const typename transform_t::vec3_t& v) const
{
	return transform_normal(v, *this);
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t
transform_t<T, Q>::inverse_transform_normal(const typename transform_t::vec3_t& v) const
{
	return inverse_transform_normal(v, *this);
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t
transform_t<T, Q>::transform_coord(const typename transform_t::vec3_t& v, const transform_t& t)
{
	const mat4_t& m = t.get_matrix();
	vec4_t result = m * vec4_t{v, 1.0f};
	result /= result.w;
	return result;
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t
transform_t<T, Q>::inverse_transform_coord(const typename transform_t::vec3_t& v, const transform_t& t)
{
	const mat4_t& m = t.get_matrix();
	mat4_t im = glm::inverse(m);
	vec3_t result = im * vec4_t{v, 1.0f};
	return result;
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t
transform_t<T, Q>::transform_normal(const typename transform_t::vec3_t& v, const transform_t& t)
{
	const mat4_t& m = t.get_matrix();
	vec4_t result = m * vec4_t{v, 0.0f};
	result /= result.w;
	return result;
}

template <typename T, qualifier Q>
inline typename transform_t<T, Q>::vec3_t
transform_t<T, Q>::inverse_transform_normal(const typename transform_t::vec3_t& v, const transform_t& t)
{
	const mat4_t& m = t.get_matrix();
	mat4_t im = glm::inverse(m);
	vec3_t result = im * vec4_t{v, 0.0f};
	return result;
}

template <typename T, qualifier Q>
inline const transform_t<T, Q>& transform_t<T, Q>::identity()
{
	static transform_t identity;
	return identity;
}

template <typename T, qualifier Q>
inline transform_t<T, Q> transform_t<T, Q>::operator*(const transform_t& t) const
{
	transform_t result(get_matrix() * t.get_matrix());
	return result;
}

template <typename T, qualifier Q>
inline transform_t<T, Q>::operator const typename transform_t<T, Q>::mat4_t::value_type*() const
{
	return value_ptr(get_matrix());
}

template <typename T, qualifier Q>
inline transform_t<T, Q>::operator const mat4_t*() const
{
	return &get_matrix();
}

template <typename T, qualifier Q>
inline transform_t<T, Q>::operator const mat4_t&() const
{
	return get_matrix();
}

using affine_transform = transform_t<float>;
}
