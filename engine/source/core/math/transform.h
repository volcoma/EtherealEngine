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
		vec3 transformCoord(const vec3 & v) const;
		vec3 inverseTransformCoord(const vec3 & v) const;
		vec3 transformNormal(const vec3 & v) const;
		vec3 inverseTransformNormal(const vec3 & v) const;
		bool decompose(vec3 & scale, vec3 & shear, quat & rotation, vec3 & translation) const;
		bool decompose(vec3 & scale, quat & rotation, vec3 & translation) const;
		bool decompose(quat & rotation, vec3 & translation) const;
		transform_t& invert();

		const vec3& getPosition() const;
		vec3 getScale() const;
		quat getRotation() const;
		const vec3& xAxis() const;
		const vec3& yAxis() const;
		const vec3& zAxis() const;
		vec3 xUnitAxis() const;
		vec3 yUnitAxis() const;
		vec3 zUnitAxis() const;
		const mat4&	 matrix() const;
		mat4& matrix();

		// transformation Operations
		transform_t& rotateAxis(float a, const vec3 & v);
		transform_t& rotate(float x, float y, float z);
		transform_t& rotateLocal(float x, float y, float z);
		transform_t& rotateLocal(const vec3 & v);
		transform_t& scale(const vec3 & v);
		transform_t& scale(float x, float y, float z);
		transform_t& translate(float x, float y, float z);
		transform_t& translate(const vec3 & v);
		transform_t& translateLocal(float x, float y, float z);
		transform_t& translateLocal(const vec3 & v);
		transform_t& setPosition(float x, float y, float z);
		transform_t& setPosition(const vec3 & v);
		transform_t& setScale(float x, float y, float z);
		transform_t& setScale(const vec3 & v);
		transform_t& setShear(float xy, float xz, float yz);
		transform_t& setRotation(const vec3 & x, const vec3 & y, const vec3 & z);
		transform_t& setRotation(const quat & q);

		// Full Re-Populate
		transform_t& zero();
		transform_t& compose(const vec3 & scale, const vec3 & shear, const quat & rotation, const vec3 & translation);
		transform_t& compose(const vec3 & scale, const quat & rotation, const vec3 & translation);
		transform_t& compose(const quat & rotation, const vec3 & translation);
		transform_t& scaling(float x, float y, float z);
		transform_t& rotation(float x, float y, float z);
		transform_t& rotationAxis(float angle, const vec3 & axis);
		transform_t& translation(float x, float y, float z);
		transform_t& translation(const vec3 & v);
		transform_t& lookAt(const vec3 & eye, const vec3 & at);
		transform_t& lookAt(const vec3 & eye, const vec3 & at, const vec3 & upAlign);

		// Comparisons
		int compare(const transform_t & t) const;
		int compare(const transform_t & t, float tolerance) const;
		bool isIdentity() const;

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
		transform_t operator *(float f) const;
		transform_t operator +(const transform_t & t) const;
		transform_t operator -(const transform_t & t) const;

		//-------------------------------------------------------------------------
		// Public Static Functions
		//-------------------------------------------------------------------------
		static vec3 transformCoord(const vec3 & v, const transform_t & t);
		static vec3 inverseTransformCoord(const vec3 & v, const transform_t & t);
		static vec3 transformNormal(const vec3 & v, const transform_t & t);
		static vec3 inverseTransformNormal(const vec3 & v, const transform_t & t);
		static bool decompose(vec3 & scale, vec3 & shear, quat & rotation, vec3 & translation, const transform_t & t);
		static bool decompose(vec3 & scale, quat & rotation, vec3 & translation, const transform_t & t);
		static bool decompose(quat & rotation, vec3 & translation, const transform_t & t);
		static transform_t lerp(transform_t & t1, transform_t & t2, float dt);
		//-------------------------------------------------------------------------
		// Public Static Variables
		//-------------------------------------------------------------------------
		static const transform_t Identity;

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