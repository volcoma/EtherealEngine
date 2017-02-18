#include "transform.h"
#include "math_types.h"

//-----------------------------------------------------------------------------
// Defines
//-----------------------------------------------------------------------------
#define MinAxisLength 1e-5f
namespace math
{
	//-----------------------------------------------------------------------------
	// Static Member Definitions
	//-----------------------------------------------------------------------------
	const transform_t transform_t::identity; // Defaults to identity

	///////////////////////////////////////////////////////////////////////////////
	// transform Member Functions
	///////////////////////////////////////////////////////////////////////////////
	//-----------------------------------------------------------------------------
	//  Name : transform () (Default Constructor)
	/// <summary>
	/// transform Class Constructor
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t::transform_t() :
		_matrix(1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1)
	{
	}

	//-----------------------------------------------------------------------------
	//  Name : transform () (Constructor)
	/// <summary>
	/// transform Class Constructor
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t::transform_t(const transform_t & t) :
		_matrix(t._matrix)
	{
	}

	//-----------------------------------------------------------------------------
	//  Name : transform () (Constructor)
	/// <summary>
	/// transform Class Constructor
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t::transform_t(const mat4 & m) :
		_matrix(m)
	{
	}

	//-----------------------------------------------------------------------------
	//  Name : transform () (Constructor)
	/// <summary>
	/// transform Class Constructor
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t::transform_t(const vec3 & vTranslation)
	{
		position() = vTranslation;
	}

	//-----------------------------------------------------------------------------
	//  Name : transform () (Constructor)
	/// <summary>
	/// transform Class Constructor
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t::transform_t(const quat & qOrientation, const vec3 & vTranslation)
	{
		_matrix = glm::mat4_cast(qOrientation);
		position() = vTranslation;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator= () (mat4&)
	/// <summary>
	/// Overloaded assignment operator. This operator decomposes the specified
	/// matrix. The matrix must be a non perspective matrix where the fourth 
	/// column is identity (<0,0,0,1>)!
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::operator=(const mat4 & m)
	{
		_matrix = m;

		// Return reference to self in order to allow multiple assignments (i.e. a=b=c)
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator== () (const transform&)
	/// <summary>
	/// Equality comparison operator
	/// </summary>
	//-----------------------------------------------------------------------------
	bool transform_t::operator==(const transform_t & t) const
	{
		return ((_matrix == t._matrix) != 0);
	}

	//-----------------------------------------------------------------------------
	//  Name : operator!= () (const transform&)
	/// <summary>
	/// Inequality comparison operator
	/// </summary>
	//-----------------------------------------------------------------------------
	bool transform_t::operator!=(const transform_t & t) const
	{
		return ((_matrix != t._matrix) != 0);
	}

	//-----------------------------------------------------------------------------
	//  Name : operator const mat4 &()
	/// <summary>
	/// Overloaded cast operator.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t::operator const mat4&() const
	{
		// Return internal matrix
		return _matrix;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator mat4*()
	/// <summary>
	/// Overloaded cast operator.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t::operator mat4*()
	{
		// Return address of internal matrix
		return &_matrix;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator const mat4*()
	/// <summary>
	/// Overloaded cast operator.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t::operator const mat4*() const
	{
		// Return address of internal matrix
		return &_matrix;
	}

	transform_t::operator float*()
	{
		return &_matrix[0][0];
	}

	transform_t::operator const float*() const
	{
		return &_matrix[0][0];
	}

	mat4::col_type & transform_t::operator[](mat4::length_type i)
	{
		return _matrix[i];
	}

	mat4::col_type const & transform_t::operator[](mat4::length_type i) const
	{
		return _matrix[i];
	}

	//-----------------------------------------------------------------------------
	//  Name : operator * (transform)
	/// <summary>
	/// transformation concatenation operator.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t transform_t::operator* (const transform_t & t) const
	{

		transform_t tOut;
		tOut._matrix = _matrix * t._matrix;

		return tOut;
	}

	vec4 transform_t::operator* (const vec4 & v) const
	{

		vec4 tOut;
		tOut = _matrix * v;

		return tOut;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator * (transform)
	/// <summary>
	/// transformation scale operator.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t transform_t::operator* (float f) const
	{
		transform_t tOut(_matrix * f);

		return tOut;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator + (transform)
	/// <summary>
	/// transformation addition operator.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t transform_t::operator+ (const transform_t & t) const
	{
		transform_t tOut;
		tOut._matrix = _matrix + t._matrix;
		return tOut;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator - (transform)
	/// <summary>
	/// transformation addition operator.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t transform_t::operator- (const transform_t & t) const
	{
		transform_t tOut;
		tOut._matrix = _matrix - t._matrix;
		return tOut;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator *= (transform)
	/// <summary>
	/// transformation concatenation operator.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::operator*= (const transform_t & t)
	{
		_matrix *= t._matrix;
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator *= (float)
	/// <summary>
	/// transformation scale operator.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::operator*= (float f)
	{
		_matrix *= f;

		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator += (transform)
	/// <summary>
	/// transformation addition operator.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::operator+= (const transform_t & t)
	{
		_matrix += t._matrix;
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : operator += (transform)
	/// <summary>
	/// transformation addition operator.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::operator-= (const transform_t & t)
	{
		_matrix -= t._matrix;
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_scale()
	/// <summary>
	/// Retrieve the scale of the transformation along its local axes.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 transform_t::get_scale() const
	{
		return vec3(glm::length(x_axis()), glm::length(y_axis()), glm::length(z_axis()));
	}

	//-----------------------------------------------------------------------------
	//  Name : get_shear()
	/// <summary>
	/// Retrieve the shear of the transformation.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 transform_t::get_shear() const
	{
		vec3 vScale, vShear, vTranslation;
		quat qRotation;
		if (decompose(vScale, vShear, qRotation, vTranslation))
			return vShear;
		else
			return vec3(0, 0, 0);
	}

	//-----------------------------------------------------------------------------
	//  Name : position()
	/// <summary>
	/// Retrieve the parent relative position of this transform.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 & transform_t::position()
	{
		return (vec3&)_matrix[3];
	}

	//-----------------------------------------------------------------------------
	//  Name : position()
	/// <summary>
	/// Retrieve the parent relative position of this transform.
	/// </summary>
	//-----------------------------------------------------------------------------
	const vec3 & transform_t::get_position() const
	{
		return (vec3&)_matrix[3];
	}

	//-----------------------------------------------------------------------------
	//  Name : xAxis()
	/// <summary>
	/// Retrieve the X axis orientation vector for this transform.
	/// This axis vector is not necessarily unit length and may be scaled.
	/// </summary>
	//-----------------------------------------------------------------------------
	const vec3 & transform_t::x_axis() const
	{
		return (vec3&)_matrix[0];
	}

	//-----------------------------------------------------------------------------
	//  Name : YXAxis()
	/// <summary>
	/// Retrieve the Y axis orientation vector for this transform.
	/// This axis vector is not necessarily unit length and may be scaled.
	/// </summary>
	//-----------------------------------------------------------------------------
	const vec3 & transform_t::y_axis() const
	{
		return (vec3&)_matrix[1];
	}

	//-----------------------------------------------------------------------------
	//  Name : ZXAxis()
	/// <summary>
	/// Retrieve the Z axis orientation vector for this transform.
	/// This axis vector is not necessarily unit length and may be scaled.
	/// </summary>
	//-----------------------------------------------------------------------------
	const vec3 & transform_t::z_axis() const
	{
		return (vec3&)_matrix[2];
	}

	//-----------------------------------------------------------------------------
	//  Name : xUnitAxis()
	/// <summary>
	/// Retrieve the unit length X axis orientation vector for this transform.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 transform_t::x_unit_axis() const
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
	vec3 transform_t::y_unit_axis() const
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
	vec3 transform_t::z_unit_axis() const
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
	const mat4& transform_t::matrix() const
	{
		return _matrix;
	}
	mat4& transform_t::matrix()
	{
		return _matrix;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_rotation()
	/// <summary>
	/// Retrieve the orientation quat for this transform.
	/// </summary>
	//-----------------------------------------------------------------------------
	quat transform_t::get_rotation() const
	{
		vec3 vScale, vShear, vTranslation;
		quat qRotation;
		if (decompose(vScale, vShear, qRotation, vTranslation))
			return qRotation;
		else
			return quat(0, 0, 0, 1);
	}

	//-----------------------------------------------------------------------------
	//  Name : zero()
	/// <summary>
	/// Reset the transform back to a completely empty state.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::zero()
	{
		memset(&_matrix, 0, sizeof(mat4));
		return *this;
	}


	//-----------------------------------------------------------------------------
	//  Name : decompose()
	/// <summary>
	/// Decompose a transform into its component parts: scale, rotation and 
	/// translation.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool transform_t::decompose(vec3 & vScale, quat & qRotation, vec3 & vTranslation) const
	{
		vec3 vShear;
		return decompose(vScale, vShear, qRotation, vTranslation);
	}

	//-----------------------------------------------------------------------------
	//  Name : decompose()
	/// <summary>
	/// Decompose a transform into its component parts: rotation and translation.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool transform_t::decompose(quat & qRotation, vec3 & vTranslation) const
	{
		vec3 vScale, vShear;
		return decompose(vScale, vShear, qRotation, vTranslation);
	}

	//-----------------------------------------------------------------------------
	//  Name : decompose() (Static)
	/// <summary>
	/// Decompose a transform into its component parts: scale, rotation and 
	/// translation.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool transform_t::decompose(vec3 & vScale, quat & qRotation, vec3 & vTranslation, const transform_t & t)
	{
		vec3 vShear;
		return t.decompose(vScale, vShear, qRotation, vTranslation);
	}

	//-----------------------------------------------------------------------------
	//  Name : decompose() (Static)
	/// <summary>
	/// Decompose a transform into its component parts: rotation and translation.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool transform_t::decompose(quat & qRotation, vec3 & vTranslation, const transform_t & t)
	{
		vec3 vScale, vShear;
		return t.decompose(vScale, vShear, qRotation, vTranslation);
	}

	//-----------------------------------------------------------------------------
	//  Name : decompose()
	/// <summary>
	/// Decompose a transform into its component parts: local scale, local shear, 
	/// rotation and translation.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool transform_t::decompose(vec3 & vScale, vec3 & vShear, quat & qRotation, vec3 & vTranslation) const
	{
		vec4 vPersp;
		glm::decompose(_matrix, vScale, qRotation, vTranslation, vShear, vPersp);

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
	bool transform_t::decompose(vec3 & vScale, vec3 & vShear, quat & qRotation, vec3 & vTranslation, const transform_t & t)
	{
		return t.decompose(vScale, vShear, qRotation, vTranslation);
	}

	//-----------------------------------------------------------------------------
	//  Name : compose()
	/// <summary>
	/// Compose a new transform from its component parts: local scale, local shear, 
	/// rotation and translation.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::compose(const vec3 & vScale, const vec3 & vShear, const quat & qRotation, const vec3 & vTranslation)
	{
		float a1, a2;
		mat4 m, mR;
		// Convert rotation quat to matrix form.
		mR = glm::mat4_cast(qRotation);

		/*// Shear matrix definition
		_11 = 1; _12 = 0; _13 = 0; _14 = 0;
		_21 = XYShear; _22 = 1; _23 = 0; _24 = 0;
		_31 = XZShear; _32 = YZShear; _33 = 1; _34 = 0;
		_41 = 0; _42 = 0; _43 = 0; _44 = 1;*/

		// (localScale * LocalShear) * rotation
		// 1st row * 1st column
		_matrix[0][0] = vScale.x * mR[0][0];
		_matrix[0][1] = vScale.x * mR[0][1];
		_matrix[0][2] = vScale.x * mR[0][2];

		// 2nd row * 2nd column
		a1 = vScale.y * vShear.x;
		_matrix[1][0] = a1 * mR[0][0] + vScale.y * mR[1][0];
		_matrix[1][1] = a1 * mR[0][1] + vScale.y * mR[1][1];
		_matrix[1][2] = a1 * mR[0][2] + vScale.y * mR[1][2];

		// 3rd row * 3rd column
		a1 = vScale.z * vShear.y; a2 = vScale.z * vShear.z;
		_matrix[2][0] = a1 * mR[0][0] + a2 * mR[1][0] + vScale.z * mR[2][0];
		_matrix[2][1] = a1 * mR[0][1] + a2 * mR[1][1] + vScale.z * mR[2][1];
		_matrix[2][2] = a1 * mR[0][2] + a2 * mR[1][2] + vScale.z * mR[2][2];

		// translation
		_matrix[3][0] = vTranslation.x; _matrix[3][1] = vTranslation.y; _matrix[3][2] = vTranslation.z;

		// identity fourth column.
		_matrix[0][3] = 0.0f; _matrix[1][3] = 0.0f; _matrix[2][3] = 0.0f; _matrix[3][3] = 1.0f;

		// Return reference to self in order to allow consecutive operations (i.e. a.rotate(...).scale(...))
		return *this;
	}


	transform_t& transform_t::compose(const vec3 & scale, const quat & rotation, const vec3 & translation)
	{
		return compose(scale, vec3{ 0.0f, 0.0f, 0.0f }, rotation, translation);
	}


	//-----------------------------------------------------------------------------
	//  Name : compose()
	/// <summary>
	/// Compose a new transform from its component parts: rotation and translation.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::compose(const quat & qRotation, const vec3 & vTranslation)
	{
		// Convert rotation quat to matrix form.
		_matrix = glm::mat4_cast(qRotation);

		// translation
		_matrix[3][0] = vTranslation.x; _matrix[3][1] = vTranslation.y; _matrix[3][2] = vTranslation.z;

		// Return reference to self in order to allow consecutive operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : invert()
	/// <summary>
	/// Invert this transform object.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::invert()
	{
		_matrix = glm::inverse(_matrix);

		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : transformCoord()
	/// <summary>
	/// transforms a 3D vector by the values in this transform object, projecting 
	/// the result back into w = 1.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 transform_t::transform_coord(const vec3 & v) const
	{
		vec4 vOut;
		vOut = _matrix * vec4{ v, 1.0f };
		vOut /= vOut.w;
		return vOut;
	}

	//-----------------------------------------------------------------------------
	//  Name : transformCoord() (Static)
	/// <summary>
	/// transforms a 3D vector by a given transform object, projecting the result 
	/// back into w = 1.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 transform_t::transform_coord(const vec3 & v, const transform_t & t)
	{
		return t.transform_coord(v);
	}

	//-----------------------------------------------------------------------------
	//  Name : inversetransformCoord()
	/// <summary>
	/// transforms a 3D vector by the inverse of the values in this transform 
	/// object, projecting the result back into w = 1.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 transform_t::inverse_transform_coord(const vec3 & v) const
	{
		mat4 im = glm::inverse(_matrix);
		vec3 vOut;
		vOut = im * vec4{ v, 1.0f };
		return vOut;
	}

	//-----------------------------------------------------------------------------
	//  Name : inversetransformCoord() (Static)
	/// <summary>
	/// transforms a 3D vector by the inverse of a given transform object,
	/// projecting the result back into w = 1.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 transform_t::inverse_transform_coord(const vec3 & v, const transform_t & t)
	{
		return t.inverse_transform_coord(v);
	}

	//-----------------------------------------------------------------------------
	//  Name : transformNormal()
	/// <summary>
	/// transforms the 3D vector normal by the values in this transform object.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 transform_t::transform_normal(const vec3 & v) const
	{
		mat4 im = _matrix;
		vec3 vOut;
		vOut = im * vec4{ v, 0.0f };
		return vOut;
	}

	//-----------------------------------------------------------------------------
	//  Name : transformNormal() (Static)
	/// <summary>
	/// transforms the 3D vector normal by the given transform object.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 transform_t::transform_normal(const vec3 & v, const transform_t & t)
	{
		return t.transform_normal(v);
	}

	//-----------------------------------------------------------------------------
	//  Name : inversetransformNormal()
	/// <summary>
	/// transforms the 3D vector normal by inverse of the values in this transform
	/// object.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 transform_t::inverse_transform_normal(const vec3 & v) const
	{
		mat4 im = glm::inverse(_matrix);
		vec3 vOut;
		vOut = im * vec4{ v, 0.0f };
		return vOut;
	}

	//-----------------------------------------------------------------------------
	//  Name : inversetransformNormal() (Static)
	/// <summary>
	/// transforms the 3D vector normal by the inverse of the given transform
	/// object.
	/// </summary>
	//-----------------------------------------------------------------------------
	vec3 transform_t::inverse_transform_normal(const vec3 & v, const transform_t & t)
	{
		return t.inverse_transform_normal(v);
	}

	//-----------------------------------------------------------------------------
	//  Name : rotate ()
	/// <summary>
	/// rotate the transform around its own local axes.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::rotate(float x, float y, float z)
	{
		// No - op?
		if (!x && !y && !z)
			return *this;

		quat qx = glm::angleAxis(x, vec3{ 1.0f, 0.0f, 0.0f });
		quat qy = glm::angleAxis(y, vec3{ 0.0f, 1.0f, 0.0f });
		quat qz = glm::angleAxis(z, vec3{ 0.0f, 0.0f, 1.0f });
		quat q = get_rotation();
		q = qz * qy * qx * q;
		set_rotation(q);

		// Return reference to self in order to allow consecutive operations (i.e. a.rotate(...).scale(...))
		return *this;
	}
	transform_t & transform_t::rotate_local(float x, float y, float z)
	{
		// No - op?
		if (!x && !y && !z)
			return *this;

		quat qx = glm::angleAxis(x, x_unit_axis());
		quat qy = glm::angleAxis(y, y_unit_axis());
		quat qz = glm::angleAxis(z, z_unit_axis());
		quat q = get_rotation();
		q = qz * qy * qx * q;
		set_rotation(q);

		// Return reference to self in order to allow consecutive operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	transform_t & transform_t::rotate_local(const vec3 & v)
	{
		return rotate_local(v.x, v.y, v.z);
	}
	//-----------------------------------------------------------------------------
	//  Name : rotateAxis ()
	/// <summary>
	/// Rotate the transform around the parent origin and the specified parent 
	/// relative axis.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::rotate_axis(float a, const vec3 & v)
	{
		// No - op?
		if (!a)
			return *this;

		quat q = glm::angleAxis(a, v) * get_rotation();
		set_rotation(q);

		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return *this;
	}
	transform_t & transform_t::scale(const vec3 & v)
	{
		return scale(v.x, v.y, v.z);
	}
	//-----------------------------------------------------------------------------
	//  Name : scaleLocal ()
	/// <summary>
	/// Scale the transform along its own local axes.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::scale(float x, float y, float z)
	{
		// No - op?
		if (x == 1.0f && y == 1.0f && z == 1.0f)
			return *this;

		// Retrieve the current local scale (length of axes).
	//     vec3 vCurrentLength = getScale();
	// 
	//     // Compute the final length of the axes after
	//     // scaling has been applied.
	//     vec3 vFinalLength( vCurrentLength.x * glm::abs<float>(x), vCurrentLength.y * glm::abs<float>(y), vCurrentLength.z * glm::abs<float>(z) );
	// 
	//     // If any fall below the minimum scale requirements, clamp the 
	//     // scaling operation.
	//     if ( (vFinalLength.x <= MinAxisLength) && vCurrentLength.x )
	//         x = (x < 0) ? MinAxisLength / vCurrentLength.x : MinAxisLength / -vCurrentLength.x;
	//     if ( vFinalLength.y <= MinAxisLength && vCurrentLength.y )
	//         y = (y < 0) ? MinAxisLength / vCurrentLength.y : MinAxisLength / -vCurrentLength.y;
	//     if ( (vFinalLength.z <= MinAxisLength) && vCurrentLength.z )
	//         z = (z < 0) ? MinAxisLength / vCurrentLength.z : MinAxisLength / -vCurrentLength.z;
	// 
		// Apply scale
		_matrix = glm::scale(_matrix, vec3{ x, y, z });


		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : translate ()
	/// <summary>
	/// Apply a translation to this transform in its parent space.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::translate(float x, float y, float z)
	{
		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return translate(vec3{ x, y, z });
	}

	//-----------------------------------------------------------------------------
	//  Name : translate ()
	/// <summary>
	/// Apply a translation to this transform in its parent space.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::translate(const vec3 & v)
	{
		position() += v;

		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : translateLocal ()
	/// <summary>
	/// Apply a translation to this transform along its own local axes.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::translate_local(float x, float y, float z)
	{
		position() += x_unit_axis() * x;
		position() += y_unit_axis() * y;
		position() += z_unit_axis() * z;

		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : translateLocal ()
	/// <summary>
	/// Apply a translation to this transform along its own local axes.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::translate_local(const vec3 & v)
	{
		position() += x_unit_axis() * v.x;
		position() += y_unit_axis() * v.y;
		position() += z_unit_axis() * v.z;

		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : setPosition ()
	/// <summary>
	/// Set the position of the origin of this transform with respect its parent.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::set_position(float x, float y, float z)
	{
		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return set_position(vec3{ x, y, z });;
	}

	//-----------------------------------------------------------------------------
	//  Name : setPosition ()
	/// <summary>
	/// Set the position of the origin of this transform with respect its parent.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::set_position(const vec3 & v)
	{
		position() = v;

		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : setLocalScale ()
	/// <summary>
	/// Set the scale of the local axes defined by this transform.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::set_scale(float x, float y, float z)
	{

		// Clamp the various scales to the minimum length.
		x = glm::max<float>(MinAxisLength, x);
		y = glm::max<float>(MinAxisLength, y);
		z = glm::max<float>(MinAxisLength, z);

		// Generate the new axis vectors;
		(vec3&)_matrix[0] = x_unit_axis() * x;
		(vec3&)_matrix[1] = y_unit_axis() * y;
		(vec3&)_matrix[2] = z_unit_axis() * z;

		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : setLocalScale ()
	/// <summary>
	/// Set the scale of the local axes defined by this transform.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::set_scale(const vec3 & v)
	{
		return set_scale(v.x, v.y, v.z);
	}

	//-----------------------------------------------------------------------------
	//  Name : setLocalShear ()
	/// <summary>
	/// Set the shear of the local axes defined by this transform.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::set_shear(float xy, float xz, float yz)
	{
		vec3 vScale, vShear, vTranslation;
		quat qRotation;

		// Decompose the matrix into its component parts
		decompose(vScale, vShear, qRotation, vTranslation);

		// Replace the shear
		vShear.x = xy;
		vShear.y = xz;
		vShear.z = yz;

		// Recompose a new matrix
		compose(vScale, vShear, qRotation, vTranslation);

		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : setOrientation ()
	/// <summary>
	/// Update the transform's orientation using the quat provided. This will
	/// also have the effect of removing any shear applied to the transform, but
	/// local axis scales will be maintained.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::set_rotation(const quat & q)
	{
		mat4 m = glm::mat4_cast(glm::normalize(q));

		// Maintain relative scale.
		vec3 vLength = get_scale();

		// Scale to original length
		(vec3&)m[0] *= vLength.x;
		(vec3&)m[1] *= vLength.y;
		(vec3&)m[2] *= vLength.z;

		// Duplicate position.
		(vec3&)m[3] = get_position();

		_matrix = m;
		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : setOrientation ()
	/// <summary>
	/// Update the transform's orientation using the axis vectors provided. This
	/// will also have the effect of replacing any shear applied to the transform
	/// with that of the axes supplied, but local axis scales will be maintained.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::set_rotation(const vec3 & vX, const vec3 & vY, const vec3 & vZ)
	{
		// Get current scale so that it can be preserved.
		vec3 vScale = get_scale();

		// Set the new axis vectors (normalized)
		(vec3&)_matrix[0] = glm::normalize(vX);
		(vec3&)_matrix[1] = glm::normalize(vY);
		(vec3&)_matrix[2] = glm::normalize(vZ);

		// Scale back to original length
		(vec3&)_matrix[0] *= vScale.x;
		(vec3&)_matrix[1] *= vScale.y;
		(vec3&)_matrix[2] *= vScale.z;

		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : lookAt()
	/// <summary>
	/// Generate a transform oriented toward the specified point.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::look_at(const vec3 & vEye, const vec3 & vAt)
	{
		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return look_at(vEye, vAt, vec3{ 0.0f, 1.0f, 0.0f });
	}

	//-----------------------------------------------------------------------------
	//  Name : lookAt()
	/// <summary>
	/// Generate a transform oriented toward the specified point.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::look_at(const vec3 & vEye, const vec3 & vAt, const vec3 & vUpAlign)
	{
		_matrix = glm::lookAt(vEye, vAt, vUpAlign);

		// Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : scaling()
	/// <summary>
	/// Generate a new scaling transform, replacing the existing transformation.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::scaling(float x, float y, float z)
	{
		_matrix = glm::scale(vec3{ x, y, z });
		return *this;
	}


	//-----------------------------------------------------------------------------
	//  Name : rotation()
	/// <summary>
	/// Generate a new rotation transform, replacing the existing transformation.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::rotation(float x, float y, float z)
	{
		quat qx, qy, qz;
		qx = glm::angleAxis(x, vec3{ 1.0f, 0.0f, 0.0f });
		qy = glm::angleAxis(y, vec3{ 0.0f, 1.0f, 0.0f });
		qz = glm::angleAxis(z, vec3{ 0.0f, 0.0f, 1.0f });
		quat q = qx * qy * qz;
		_matrix = glm::mat4_cast(q);
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : rotationAxis()
	/// <summary>
	/// Generate a new axis rotation transform, replacing the existing 
	/// transformation.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::rotation_axis(float a, const vec3 & v)
	{
		quat q = glm::angleAxis(a, v);
		_matrix = glm::mat4_cast(q);
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : translation()
	/// <summary>
	/// Generate a new translation transform, replacing the existing transformation.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::translation(float x, float y, float z)
	{
		_matrix = glm::translate(vec3{ x, y, z });
		return *this;
	}

	//-----------------------------------------------------------------------------
	//  Name : translation()
	/// <summary>
	/// Generate a new translation transform, replacing the existing transformation.
	/// </summary>
	//-----------------------------------------------------------------------------
	transform_t & transform_t::translation(const vec3 & v)
	{
		_matrix = glm::translate(v);
		return *this;
	}

	transform_t transform_t::lerp(transform_t & t1, transform_t & t2, float dt)
	{
		transform_t result;

		result._matrix = t1._matrix + (t2._matrix - t1._matrix) * dt;

		return result;
	}

	//-----------------------------------------------------------------------------
	//  Name : translation() (Static)
	/// <summary>
	/// Generate a new translation transform.
	/// </summary>
	//-----------------------------------------------------------------------------
	// transform & transform::translation( transform & tOut, const vec3 & v )
	// {
	//     return tOut.translation( v );
	// }

	//-----------------------------------------------------------------------------
	//  Name : compare()
	/// <summary>
	/// Compare this transform against the provided transform in order to determine
	/// if they are equivalent (within the specified tolerance).
	/// </summary>
	//-----------------------------------------------------------------------------
	int transform_t::compare(const transform_t & t, float fTolerance) const
	{
		float fDifference;
		for (int i = 0; i < 4; ++i)
		{
			for (int j = 0; j < 4; ++j)
			{
				fDifference = _matrix[i][j] - t._matrix[i][j];
				if (glm::abs<float>(fDifference) > fTolerance)
					return (fDifference < 0) ? -1 : 1;
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
	int transform_t::compare(const transform_t & t) const
	{
		return _matrix == t._matrix;
	}

	//-----------------------------------------------------------------------------
	//  Name : isIdentity()
	/// <summary>
	/// Determine if the transform is in an identity state.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool transform_t::is_identity() const
	{
		static const transform_t Identitytransform;
		return (!!(_matrix == Identitytransform._matrix));
	}

	math::transform_t inverse(transform_t const & t)
	{
		glm::mat4 m = t.matrix();
		glm::mat4 inv = glm::inverse(m);
		return inv;
	}

	math::transform_t transpose(transform_t const & t)
	{
		glm::mat4 m = t.matrix();
		glm::mat4 trans = glm::transpose(m);
		return trans;
	}

}