#include "transform.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
namespace math
{
//-----------------------------------------------------------------------------
// Static Member Definitions
//-----------------------------------------------------------------------------
const transform transform::identity; // Defaults to identity

///////////////////////////////////////////////////////////////////////////////
// transform Member Functions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  Name : transform () (Default Constructor)
/// <summary>
/// transform Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
transform::transform()
	: matrix_(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
{
}

//-----------------------------------------------------------------------------
//  Name : transform () (Constructor)
/// <summary>
/// transform Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
transform::transform(const transform& t)
	: matrix_(t.matrix_)
{
}

//-----------------------------------------------------------------------------
//  Name : transform () (Constructor)
/// <summary>
/// transform Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
transform::transform(const mat4& m)
	: matrix_(m)
{
}

//-----------------------------------------------------------------------------
//  Name : transform () (Constructor)
/// <summary>
/// transform Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
transform::transform(const vec3& translation)
{
	position() = translation;
}

//-----------------------------------------------------------------------------
//  Name : transform () (Constructor)
/// <summary>
/// transform Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
transform::transform(const quat& qOrientation, const vec3& translation)
{
	matrix_ = glm::mat4_cast(qOrientation);
	position() = translation;
}

//-----------------------------------------------------------------------------
//  Name : operator= () (mat4&)
/// <summary>
/// Overloaded assignment operator. This operator decomposes the specified
/// matrix. The matrix must be a non perspective matrix where the fourth
/// column is identity (<0,0,0,1>)!
/// </summary>
//-----------------------------------------------------------------------------
transform& transform::operator=(const mat4& m)
{
	matrix_ = m;

	// Return reference to self in order to allow multiple assignments (i.e.
	// a=b=c)
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator== () (const transform&)
/// <summary>
/// Equality comparison operator
/// </summary>
//-----------------------------------------------------------------------------
bool transform::operator==(const transform& t) const
{
	return matrix_ == t.matrix_;
}

//-----------------------------------------------------------------------------
//  Name : operator!= () (const transform&)
/// <summary>
/// Inequality comparison operator
/// </summary>
//-----------------------------------------------------------------------------
bool transform::operator!=(const transform& t) const
{
	return matrix_ != t.matrix_;
}

//-----------------------------------------------------------------------------
//  Name : operator const mat4 &()
/// <summary>
/// Overloaded cast operator.
/// </summary>
//-----------------------------------------------------------------------------
transform::operator const mat4&() const
{
	// Return internal matrix
	return matrix_;
}

//-----------------------------------------------------------------------------
//  Name : operator mat4*()
/// <summary>
/// Overloaded cast operator.
/// </summary>
//-----------------------------------------------------------------------------
transform::operator mat4*()
{
	// Return address of internal matrix
	return &matrix_;
}

//-----------------------------------------------------------------------------
//  Name : operator const mat4*()
/// <summary>
/// Overloaded cast operator.
/// </summary>
//-----------------------------------------------------------------------------
transform::operator const mat4*() const
{
	// Return address of internal matrix
	return &matrix_;
}

transform::operator float*()
{
	return &matrix_[0][0];
}

transform::operator const float*() const
{
	return &matrix_[0][0];
}

mat4::col_type& transform::operator[](mat4::length_type i)
{
	return matrix_[i];
}

mat4::col_type const& transform::operator[](mat4::length_type i) const
{
	return matrix_[i];
}

//-----------------------------------------------------------------------------
//  Name : operator * (transform)
/// <summary>
/// transformation concatenation operator.
/// </summary>
//-----------------------------------------------------------------------------
transform transform::operator*(const transform& t) const
{
	transform result(matrix_ * t.matrix_);
	return result;
}

vec4 transform::operator*(const vec4& v) const
{
	vec4 result = matrix_ * v;
	return result;
}

//-----------------------------------------------------------------------------
//  Name : operator * (transform)
/// <summary>
/// transformation scale operator.
/// </summary>
//-----------------------------------------------------------------------------
transform transform::operator*(float f) const
{
	transform result(matrix_ * f);
	return result;
}

//-----------------------------------------------------------------------------
//  Name : operator + (transform)
/// <summary>
/// transformation addition operator.
/// </summary>
//-----------------------------------------------------------------------------
transform transform::operator+(const transform& t) const
{
	transform result(matrix_ + t.matrix_);
	return result;
}

//-----------------------------------------------------------------------------
//  Name : operator - (transform)
/// <summary>
/// transformation addition operator.
/// </summary>
//-----------------------------------------------------------------------------
transform transform::operator-(const transform& t) const
{
	transform result(matrix_ - t.matrix_);
	return result;
}

//-----------------------------------------------------------------------------
//  Name : operator *= (transform)
/// <summary>
/// transformation concatenation operator.
/// </summary>
//-----------------------------------------------------------------------------
transform& transform::operator*=(const transform& t)
{
	matrix_ *= t.matrix_;
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator *= (float)
/// <summary>
/// transformation scale operator.
/// </summary>
//-----------------------------------------------------------------------------
transform& transform::operator*=(float f)
{
	matrix_ *= f;
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator += (transform)
/// <summary>
/// transformation addition operator.
/// </summary>
//-----------------------------------------------------------------------------
transform& transform::operator+=(const transform& t)
{
	matrix_ += t.matrix_;
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator += (transform)
/// <summary>
/// transformation addition operator.
/// </summary>
//-----------------------------------------------------------------------------
transform& transform::operator-=(const transform& t)
{
	matrix_ -= t.matrix_;
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : get_scale()
/// <summary>
/// Retrieve the scale of the transformation along its local axes.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::get_scale() const
{
	return vec3(glm::length(x_axis()), glm::length(y_axis()), glm::length(z_axis()));
}

//-----------------------------------------------------------------------------
//  Name : get_shear()
/// <summary>
/// Retrieve the shear of the transformation.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::get_shear() const
{
	vec3 scale, shear, translation;
	quat rotation;
	if(decompose(scale, shear, rotation, translation))
	{
		return shear;
	}
	return vec3(0.0f, 0.0f, 0.0f);
}

//-----------------------------------------------------------------------------
//  Name : position()
/// <summary>
/// Retrieve the parent relative position of this transform.
/// </summary>
//-----------------------------------------------------------------------------
vec3& transform::position()
{
	return reinterpret_cast<vec3&>(matrix_[3]);
}

//-----------------------------------------------------------------------------
//  Name : position()
/// <summary>
/// Retrieve the parent relative position of this transform.
/// </summary>
//-----------------------------------------------------------------------------
const vec3& transform::get_position() const
{
	return reinterpret_cast<const vec3&>(matrix_[3]);
}

//-----------------------------------------------------------------------------
//  Name : x_axis()
/// <summary>
/// Retrieve the X axis orientation vector for this transform.
/// This axis vector is not necessarily unit length and may be scaled.
/// </summary>
//-----------------------------------------------------------------------------
const vec3& transform::x_axis() const
{
	return reinterpret_cast<const vec3&>(matrix_[0]);
}

//-----------------------------------------------------------------------------
//  Name : y_axis()
/// <summary>
/// Retrieve the Y axis orientation vector for this transform.
/// This axis vector is not necessarily unit length and may be scaled.
/// </summary>
//-----------------------------------------------------------------------------
const vec3& transform::y_axis() const
{
	return reinterpret_cast<const vec3&>(matrix_[1]);
}

//-----------------------------------------------------------------------------
//  Name : z_axis()
/// <summary>
/// Retrieve the Z axis orientation vector for this transform.
/// This axis vector is not necessarily unit length and may be scaled.
/// </summary>
//-----------------------------------------------------------------------------
const vec3& transform::z_axis() const
{
	return reinterpret_cast<const vec3&>(matrix_[2]);
}

//-----------------------------------------------------------------------------
//  Name : xUnitAxis()
/// <summary>
/// Retrieve the unit length X axis orientation vector for this transform.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::x_unit_axis() const
{
	vec3 v = glm::normalize(x_axis());
	return v;
}

//-----------------------------------------------------------------------------
//  Name : yUnitAxis()
/// <summary>
/// Retrieve the unit length Y axis orientation vector for this transform.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::y_unit_axis() const
{
	vec3 v = glm::normalize(y_axis());
	return v;
}

//-----------------------------------------------------------------------------
//  Name : zUnitAxis()
/// <summary>
/// Retrieve the unit length Z axis orientation vector for this transform.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::z_unit_axis() const
{
	vec3 v = glm::normalize(z_axis());
	return v;
}

//-----------------------------------------------------------------------------
//  Name : matrix()
/// <summary>
/// Retrieve matrix representation of the transform
/// </summary>
//-----------------------------------------------------------------------------
const mat4& transform::matrix() const
{
	return matrix_;
}
mat4& transform::matrix()
{
	return matrix_;
}

//-----------------------------------------------------------------------------
//  Name : get_rotation()
/// <summary>
/// Retrieve the orientation quat for this transform.
/// </summary>
//-----------------------------------------------------------------------------
quat transform::get_rotation() const
{
	vec3 scale, shear, translation;
	quat rotation;
	if(decompose(scale, shear, rotation, translation))
	{
		return rotation;
	}
	return quat(0.0f, 0.0f, 0.0f, 1.0f);
}

//-----------------------------------------------------------------------------
//  Name : zero()
/// <summary>
/// Reset the transform back to a completely empty state.
/// </summary>
//-----------------------------------------------------------------------------
void transform::zero()
{
	memset(&matrix_, 0, sizeof(mat4));
}

//-----------------------------------------------------------------------------
//  Name : decompose()
/// <summary>
/// Decompose a transform into its component parts: scale, rotation and
/// translation.
/// </summary>
//-----------------------------------------------------------------------------
bool transform::decompose(vec3& scale, quat& rotation, vec3& translation) const
{
	vec3 shear;
	return decompose(scale, shear, rotation, translation);
}

//-----------------------------------------------------------------------------
//  Name : decompose()
/// <summary>
/// Decompose a transform into its component parts: rotation and translation.
/// </summary>
//-----------------------------------------------------------------------------
bool transform::decompose(quat& rotation, vec3& translation) const
{
	vec3 scale, shear;
	return decompose(scale, shear, rotation, translation);
}

//-----------------------------------------------------------------------------
//  Name : decompose() (Static)
/// <summary>
/// Decompose a transform into its component parts: scale, rotation and
/// translation.
/// </summary>
//-----------------------------------------------------------------------------
bool transform::decompose(vec3& scale, quat& rotation, vec3& translation, const transform& t)
{
	vec3 shear;
	return t.decompose(scale, shear, rotation, translation);
}

//-----------------------------------------------------------------------------
//  Name : decompose() (Static)
/// <summary>
/// Decompose a transform into its component parts: rotation and translation.
/// </summary>
//-----------------------------------------------------------------------------
bool transform::decompose(quat& rotation, vec3& translation, const transform& t)
{
	vec3 scale, shear;
	return t.decompose(scale, shear, rotation, translation);
}

//-----------------------------------------------------------------------------
//  Name : decompose()
/// <summary>
/// Decompose a transform into its component parts: local scale, local shear,
/// rotation and translation.
/// </summary>
//-----------------------------------------------------------------------------
bool transform::decompose(vec3& scale, vec3& shear, quat& rotation, vec3& translation) const
{
	vec4 persp;
	glm::decompose(matrix_, scale, rotation, translation, shear, persp);

	// Success!
	return true;
}

//-----------------------------------------------------------------------------
//  Name : decompose() (Static)
/// <summary>
/// Decompose a transform into its component parts: local scale, local shear,
/// rotation and translation.
/// </summary>
//-----------------------------------------------------------------------------
bool transform::decompose(vec3& scale, vec3& shear, quat& rotation, vec3& translation, const transform& t)
{
	return t.decompose(scale, shear, rotation, translation);
}

//-----------------------------------------------------------------------------
//  Name : compose()
/// <summary>
/// Compose a new transform from its component parts: local scale, local shear,
/// rotation and translation.
/// </summary>
//-----------------------------------------------------------------------------
void transform::compose(const vec3& scale, const vec3& shear, const quat& rotation, const vec3& translation)
{
	// Convert rotation quat to matrix form.
	mat4 mR = glm::mat4_cast(rotation);

	// (localScale * LocalShear) * rotation
	// 1st row * 1st column
	matrix_[0][0] = scale.x * mR[0][0];
	matrix_[0][1] = scale.x * mR[0][1];
	matrix_[0][2] = scale.x * mR[0][2];

	// 2nd row * 2nd column
	float a1 = scale.y * shear.x;
	matrix_[1][0] = a1 * mR[0][0] + scale.y * mR[1][0];
	matrix_[1][1] = a1 * mR[0][1] + scale.y * mR[1][1];
	matrix_[1][2] = a1 * mR[0][2] + scale.y * mR[1][2];

	// 3rd row * 3rd column
	a1 = scale.z * shear.y;
	float a2 = scale.z * shear.z;
	matrix_[2][0] = a1 * mR[0][0] + a2 * mR[1][0] + scale.z * mR[2][0];
	matrix_[2][1] = a1 * mR[0][1] + a2 * mR[1][1] + scale.z * mR[2][1];
	matrix_[2][2] = a1 * mR[0][2] + a2 * mR[1][2] + scale.z * mR[2][2];

	// translation
	matrix_[3][0] = translation.x;
	matrix_[3][1] = translation.y;
	matrix_[3][2] = translation.z;

	// identity fourth column.
	matrix_[0][3] = 0.0f;
	matrix_[1][3] = 0.0f;
	matrix_[2][3] = 0.0f;
	matrix_[3][3] = 1.0f;
}

void transform::compose(const vec3& scale, const quat& rotation, const vec3& translation)
{
	compose(scale, vec3{0.0f, 0.0f, 0.0f}, rotation, translation);
}

//-----------------------------------------------------------------------------
//  Name : compose()
/// <summary>
/// Compose a new transform from its component parts: rotation and translation.
/// </summary>
//-----------------------------------------------------------------------------
void transform::compose(const quat& rotation, const vec3& translation)
{
	// Convert rotation quat to matrix form.
	matrix_ = glm::mat4_cast(rotation);

	// translation
	matrix_[3][0] = translation.x;
	matrix_[3][1] = translation.y;
	matrix_[3][2] = translation.z;
}

//-----------------------------------------------------------------------------
//  Name : transform_coord()
/// <summary>
/// transforms a 3D vector by the values in this transform object, projecting
/// the result back into w = 1.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::transform_coord(const vec3& v) const
{
	vec4 result = matrix_ * vec4{v, 1.0f};
	result /= result.w;
	return result;
}

//-----------------------------------------------------------------------------
//  Name : transform_coord() (Static)
/// <summary>
/// transforms a 3D vector by a given transform object, projecting the result
/// back into w = 1.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::transform_coord(const vec3& v, const transform& t)
{
	return t.transform_coord(v);
}

//-----------------------------------------------------------------------------
//  Name : inverse_transform_coord()
/// <summary>
/// transforms a 3D vector by the inverse of the values in this transform
/// object, projecting the result back into w = 1.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::inverse_transform_coord(const vec3& v) const
{
	mat4 im = glm::inverse(matrix_);
	vec3 result = im * vec4{v, 1.0f};
	return result;
}

//-----------------------------------------------------------------------------
//  Name : inverse_transform_coord() (Static)
/// <summary>
/// transforms a 3D vector by the inverse of a given transform object,
/// projecting the result back into w = 1.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::inverse_transform_coord(const vec3& v, const transform& t)
{
	return t.inverse_transform_coord(v);
}

//-----------------------------------------------------------------------------
//  Name : transform_normal()
/// <summary>
/// transforms the 3D vector normal by the values in this transform object.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::transform_normal(const vec3& v) const
{
	mat4 im = matrix_;
	vec3 result = im * vec4{v, 0.0f};
	return result;
}

//-----------------------------------------------------------------------------
//  Name : transform_normal() (Static)
/// <summary>
/// transforms the 3D vector normal by the given transform object.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::transform_normal(const vec3& v, const transform& t)
{
	return t.transform_normal(v);
}

//-----------------------------------------------------------------------------
//  Name : inversetransform_normal()
/// <summary>
/// transforms the 3D vector normal by inverse of the values in this transform
/// object.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::inverse_transform_normal(const vec3& v) const
{
	mat4 im = glm::inverse(matrix_);
	vec3 result = im * vec4{v, 0.0f};
	return result;
}

//-----------------------------------------------------------------------------
//  Name : inversetransform_normal() (Static)
/// <summary>
/// transforms the 3D vector normal by the inverse of the given transform
/// object.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform::inverse_transform_normal(const vec3& v, const transform& t)
{
	return t.inverse_transform_normal(v);
}

//-----------------------------------------------------------------------------
//  Name : rotate ()
/// <summary>
/// rotate the transform around its own local axes.
/// </summary>
//-----------------------------------------------------------------------------
void transform::rotate(float x, float y, float z)
{
	quat qx = glm::angleAxis(x, vec3{1.0f, 0.0f, 0.0f});
	quat qy = glm::angleAxis(y, vec3{0.0f, 1.0f, 0.0f});
	quat qz = glm::angleAxis(z, vec3{0.0f, 0.0f, 1.0f});
	quat q = qz * qy * qx * get_rotation();
	set_rotation(q);
}
void transform::rotate(const vec3& v)
{
	rotate(v.x, v.y, v.z);
}

void transform::rotate_local(float x, float y, float z)
{
	quat qx = glm::angleAxis(x, x_unit_axis());
	quat qy = glm::angleAxis(y, y_unit_axis());
	quat qz = glm::angleAxis(z, z_unit_axis());
	quat q = qz * qy * qx * get_rotation();
	set_rotation(q);
}

void transform::rotate_local(const vec3& v)
{
	rotate_local(v.x, v.y, v.z);
}
//-----------------------------------------------------------------------------
//  Name : rotateAxis ()
/// <summary>
/// Rotate the transform around the parent origin and the specified parent
/// relative axis.
/// </summary>
//-----------------------------------------------------------------------------
void transform::rotate_axis(float a, const vec3& v)
{
	quat q = glm::angleAxis(a, v) * get_rotation();
	set_rotation(q);
}
void transform::scale(const vec3& v)
{
	// No - op?
	if(v.x == 1.0f && v.y == 1.0f && v.z == 1.0f)
	{
		return;
	}

	// Apply scale
	matrix_ = glm::scale(matrix_, v);
}
//-----------------------------------------------------------------------------
//  Name : scale ()
/// <summary>
/// Scale the transform along its own local axes.
/// </summary>
//-----------------------------------------------------------------------------
void transform::scale(float x, float y, float z)
{
	scale(vec3(x, y, z));
}

//-----------------------------------------------------------------------------
//  Name : translate ()
/// <summary>
/// Apply a translation to this transform in its parent space.
/// </summary>
//-----------------------------------------------------------------------------
void transform::translate(float x, float y, float z)
{
	// Return reference to self in order to allow multiple operations (i.e.
	// a.rotate(...).scale(...))
	translate(vec3{x, y, z});
}

//-----------------------------------------------------------------------------
//  Name : translate ()
/// <summary>
/// Apply a translation to this transform in its parent space.
/// </summary>
//-----------------------------------------------------------------------------
void transform::translate(const vec3& v)
{
	position() += v;
}

//-----------------------------------------------------------------------------
//  Name : translate_local ()
/// <summary>
/// Apply a translation to this transform along its own local axes.
/// </summary>
//-----------------------------------------------------------------------------
void transform::translate_local(float x, float y, float z)
{
	position() += x_unit_axis() * x;
	position() += y_unit_axis() * y;
	position() += z_unit_axis() * z;
}

//-----------------------------------------------------------------------------
//  Name : translate_local ()
/// <summary>
/// Apply a translation to this transform along its own local axes.
/// </summary>
//-----------------------------------------------------------------------------
void transform::translate_local(const vec3& v)
{
	position() += x_unit_axis() * v.x;
	position() += y_unit_axis() * v.y;
	position() += z_unit_axis() * v.z;
}

//-----------------------------------------------------------------------------
//  Name : set_position ()
/// <summary>
/// Set the position of the origin of this transform with respect its parent.
/// </summary>
//-----------------------------------------------------------------------------
void transform::set_position(float x, float y, float z)
{
	// Return reference to self in order to allow multiple operations (i.e.
	// a.rotate(...).scale(...))
	set_position(vec3{x, y, z});
}

//-----------------------------------------------------------------------------
//  Name : set_position ()
/// <summary>
/// Set the position of the origin of this transform with respect its parent.
/// </summary>
//-----------------------------------------------------------------------------
void transform::set_position(const vec3& v)
{
	position() = v;
}

//-----------------------------------------------------------------------------
//  Name : set_scale ()
/// <summary>
/// Set the scale of the local axes defined by this transform.
/// </summary>
//-----------------------------------------------------------------------------
void transform::set_scale(float x, float y, float z)
{
	const float min_axis_length = glm::epsilon<float>();
	// Clamp the various scales to the minimum length.
	x = glm::max<float>(min_axis_length, x);
	y = glm::max<float>(min_axis_length, y);
	z = glm::max<float>(min_axis_length, z);

	// Generate the new axis vectors;
	reinterpret_cast<vec3&>(matrix_[0]) = x_unit_axis() * x;
	reinterpret_cast<vec3&>(matrix_[1]) = y_unit_axis() * y;
	reinterpret_cast<vec3&>(matrix_[2]) = z_unit_axis() * z;
}

//-----------------------------------------------------------------------------
//  Name : set_scale ()
/// <summary>
/// Set the scale of the local axes defined by this transform.
/// </summary>
//-----------------------------------------------------------------------------
void transform::set_scale(const vec3& v)
{
	set_scale(v.x, v.y, v.z);
}

//-----------------------------------------------------------------------------
//  Name : set_shear ()
/// <summary>
/// Set the shear of the local axes defined by this transform.
/// </summary>
//-----------------------------------------------------------------------------
void transform::set_shear(float xy, float xz, float yz)
{
	vec3 scale, shear, translation;
	quat rotation;

	// Decompose the matrix into its component parts
	decompose(scale, shear, rotation, translation);

	// Replace the shear
	shear.x = xy;
	shear.y = xz;
	shear.z = yz;

	// Recompose a new matrix
	compose(scale, shear, rotation, translation);
}

//-----------------------------------------------------------------------------
//  Name : set_rotation ()
/// <summary>
/// Update the transform's orientation using the quat provided. This will
/// also have the effect of removing any shear applied to the transform, but
/// local axis scales will be maintained.
/// </summary>
//-----------------------------------------------------------------------------
void transform::set_rotation(const quat& q)
{
	mat4 m = glm::mat4_cast(glm::normalize(q));

	// Maintain relative scale.
	vec3 length = get_scale();

	// Scale to original length
	reinterpret_cast<vec3&>(m[0]) *= length.x;
	reinterpret_cast<vec3&>(m[1]) *= length.y;
	reinterpret_cast<vec3&>(m[2]) *= length.z;

	// Duplicate position.
	reinterpret_cast<vec3&>(m[3]) = get_position();

	matrix_ = m;
}

//-----------------------------------------------------------------------------
//  Name : set_rotation ()
/// <summary>
/// Update the transform's orientation using the axis vectors provided. This
/// will also have the effect of replacing any shear applied to the transform
/// with that of the axes supplied, but local axis scales will be maintained.
/// </summary>
//-----------------------------------------------------------------------------
void transform::set_rotation(const vec3& vX, const vec3& vY, const vec3& vZ)
{
	// Get current scale so that it can be preserved.
	vec3 scale = get_scale();

	// Set the new axis vectors (normalized)
	reinterpret_cast<vec3&>(matrix_[0]) = glm::normalize(vX);
	reinterpret_cast<vec3&>(matrix_[1]) = glm::normalize(vY);
	reinterpret_cast<vec3&>(matrix_[2]) = glm::normalize(vZ);

	// Scale back to original length
	reinterpret_cast<vec3&>(matrix_[0]) *= scale.x;
	reinterpret_cast<vec3&>(matrix_[1]) *= scale.y;
	reinterpret_cast<vec3&>(matrix_[2]) *= scale.z;
}

//-----------------------------------------------------------------------------
//  Name : look_at()
/// <summary>
/// Generate a transform oriented toward the specified point.
/// </summary>
//-----------------------------------------------------------------------------
void transform::look_at(const vec3& vEye, const vec3& vAt)
{
	look_at(vEye, vAt, vec3{0.0f, 1.0f, 0.0f});
}

//-----------------------------------------------------------------------------
//  Name : look_at()
/// <summary>
/// Generate a transform oriented toward the specified point.
/// </summary>
//-----------------------------------------------------------------------------
void transform::look_at(const vec3& vEye, const vec3& vAt, const vec3& vUpAlign)
{
	matrix_ = glm::lookAt(vEye, vAt, vUpAlign);
}

//-----------------------------------------------------------------------------
//  Name : scaling()
/// <summary>
/// Generate a new scaling transform, replacing the existing transformation.
/// </summary>
//-----------------------------------------------------------------------------
void transform::scaling(float x, float y, float z)
{
	matrix_ = glm::scale(vec3{x, y, z});
}

//-----------------------------------------------------------------------------
//  Name : rotation()
/// <summary>
/// Generate a new rotation transform, replacing the existing transformation.
/// </summary>
//-----------------------------------------------------------------------------
void transform::rotation(float x, float y, float z)
{
	quat qx, qy, qz;
	qx = glm::angleAxis(x, vec3{1.0f, 0.0f, 0.0f});
	qy = glm::angleAxis(y, vec3{0.0f, 1.0f, 0.0f});
	qz = glm::angleAxis(z, vec3{0.0f, 0.0f, 1.0f});
	quat q = qx * qy * qz;
	matrix_ = glm::mat4_cast(q);
}

//-----------------------------------------------------------------------------
//  Name : rotationAxis()
/// <summary>
/// Generate a new axis rotation transform, replacing the existing
/// transformation.
/// </summary>
//-----------------------------------------------------------------------------
void transform::rotation_axis(float a, const vec3& v)
{
	quat q = glm::angleAxis(a, v);
	matrix_ = glm::mat4_cast(q);
}

//-----------------------------------------------------------------------------
//  Name : translation()
/// <summary>
/// Generate a new translation transform, replacing the existing transformation.
/// </summary>
//-----------------------------------------------------------------------------
void transform::translation(float x, float y, float z)
{
	matrix_ = glm::translate(vec3{x, y, z});
}

//-----------------------------------------------------------------------------
//  Name : translation()
/// <summary>
/// Generate a new translation transform, replacing the existing transformation.
/// </summary>
//-----------------------------------------------------------------------------
void transform::translation(const vec3& v)
{
	matrix_ = glm::translate(v);
}

transform transform::lerp(transform& t1, transform& t2, float dt)
{
	transform result;

	result.matrix_ = t1.matrix_ + (t2.matrix_ - t1.matrix_) * dt;

	return result;
}

//-----------------------------------------------------------------------------
//  Name : compare()
/// <summary>
/// Compare this transform against the provided transform in order to determine
/// if they are equivalent (within the specified tolerance).
/// </summary>
//-----------------------------------------------------------------------------
int transform::compare(const transform& t, float tolerance) const
{
	for(int i = 0; i < 4; ++i)
	{
		for(int j = 0; j < 4; ++j)
		{
			float diff = matrix_[i][j] - t.matrix_[i][j];
			if(glm::abs<float>(diff) > tolerance)
			{
				return (diff < 0) ? -1 : 1;
			}
		}
	}

	// Equivalent
	return 0;
}

//-----------------------------------------------------------------------------
//  Name : compare()
/// <summary>
/// Compare this transform against the provided transform in order to determine
/// if they are equivalent.
/// </summary>
//-----------------------------------------------------------------------------
int transform::compare(const transform& t) const
{
	return static_cast<int>(matrix_ == t.matrix_);
}

//-----------------------------------------------------------------------------
//  Name : isIdentity()
/// <summary>
/// Determine if the transform is in an identity state.
/// </summary>
//-----------------------------------------------------------------------------
bool transform::is_identity() const
{
	static const transform identity;
	return (matrix_ == identity.matrix_);
}

math::transform inverse(transform const& t)
{
	glm::mat4 m = t.matrix();
	glm::mat4 inv = glm::inverse(m);
	return inv;
}

math::transform transpose(transform const& t)
{
	glm::mat4 m = t.matrix();
	glm::mat4 trans = glm::transpose(m);
	return trans;
}
}
