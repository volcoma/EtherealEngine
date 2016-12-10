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
const transform_t transform_t::Identity; // Defaults to identity

///////////////////////////////////////////////////////////////////////////////
// transform Member Functions
///////////////////////////////////////////////////////////////////////////////
//-----------------------------------------------------------------------------
//  Name : transform () (Default Constructor)
/// <summary>
/// transform Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
transform_t::transform_t( ) :
    mMatrix( 1,0,0,0,0,1,0,0,0,0,1,0,0,0,0,1 )
{
}

//-----------------------------------------------------------------------------
//  Name : transform () (Constructor)
/// <summary>
/// transform Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
transform_t::transform_t( const transform_t & t ) :
    mMatrix( t.mMatrix )
{
}

//-----------------------------------------------------------------------------
//  Name : transform () (Constructor)
/// <summary>
/// transform Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
transform_t::transform_t( const mat4 & m ) :
    mMatrix( m )
{
}

//-----------------------------------------------------------------------------
//  Name : transform () (Constructor)
/// <summary>
/// transform Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
transform_t::transform_t( const vec3 & vTranslation )
{
    position() = vTranslation;
}

//-----------------------------------------------------------------------------
//  Name : transform () (Constructor)
/// <summary>
/// transform Class Constructor
/// </summary>
//-----------------------------------------------------------------------------
transform_t::transform_t( const quat & qOrientation, const vec3 & vTranslation )
{
	mMatrix = glm::mat4_cast(qOrientation);
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
transform_t & transform_t::operator=( const mat4 & m )
{
    mMatrix = m;
    
    // Return reference to self in order to allow multiple assignments (i.e. a=b=c)
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : operator== () (const transform&)
/// <summary>
/// Equality comparison operator
/// </summary>
//-----------------------------------------------------------------------------
bool transform_t::operator==( const transform_t & t ) const
{
    return ((mMatrix == t.mMatrix) != 0);
}

//-----------------------------------------------------------------------------
//  Name : operator!= () (const transform&)
/// <summary>
/// Inequality comparison operator
/// </summary>
//-----------------------------------------------------------------------------
bool transform_t::operator!=( const transform_t & t ) const
{
    return ((mMatrix != t.mMatrix) != 0);
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
    return mMatrix;
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
    return &mMatrix;
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
    return &mMatrix;
}

transform_t::operator float*()
{
	return &mMatrix[0][0];
}

transform_t::operator const float*() const
{
	return &mMatrix[0][0];
}

mat4::col_type & transform_t::operator[](mat4::length_type i)
{
	return mMatrix[i];
}

mat4::col_type const & transform_t::operator[](mat4::length_type i) const
{
	return mMatrix[i];
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
	tOut.mMatrix = mMatrix * t.mMatrix;

    return tOut;
}

//-----------------------------------------------------------------------------
//  Name : operator * (transform)
/// <summary>
/// transformation scale operator.
/// </summary>
//-----------------------------------------------------------------------------
transform_t transform_t::operator* ( float f ) const
{
    transform_t tOut( mMatrix * f );
    
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
	tOut.mMatrix = mMatrix + t.mMatrix;
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
	tOut.mMatrix = mMatrix - t.mMatrix;
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
	mMatrix *= t.mMatrix;
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
    mMatrix *= f;

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
	mMatrix += t.mMatrix;
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
	mMatrix -= t.mMatrix;
	return *this;
}

//-----------------------------------------------------------------------------
//  Name : localScale()
/// <summary>
/// Retrieve the scale of the transformation along its local axes.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform_t::getScale( ) const
{
    return vec3( glm::length( xAxis() ), glm::length( yAxis() ), glm::length( zAxis() ) );
}

//-----------------------------------------------------------------------------
//  Name : position()
/// <summary>
/// Retrieve the parent relative position of this transform.
/// </summary>
//-----------------------------------------------------------------------------
vec3 & transform_t::position( )
{
    return (vec3&)mMatrix[3];
}

//-----------------------------------------------------------------------------
//  Name : position()
/// <summary>
/// Retrieve the parent relative position of this transform.
/// </summary>
//-----------------------------------------------------------------------------
const vec3 & transform_t::getPosition( ) const
{
    return (vec3&)mMatrix[3];
}

//-----------------------------------------------------------------------------
//  Name : xAxis()
/// <summary>
/// Retrieve the X axis orientation vector for this transform.
/// This axis vector is not necessarily unit length and may be scaled.
/// </summary>
//-----------------------------------------------------------------------------
const vec3 & transform_t::xAxis( ) const
{
    return (vec3&)mMatrix[0];
}

//-----------------------------------------------------------------------------
//  Name : YXAxis()
/// <summary>
/// Retrieve the Y axis orientation vector for this transform.
/// This axis vector is not necessarily unit length and may be scaled.
/// </summary>
//-----------------------------------------------------------------------------
const vec3 & transform_t::yAxis( ) const
{
    return (vec3&)mMatrix[1];
}

//-----------------------------------------------------------------------------
//  Name : ZXAxis()
/// <summary>
/// Retrieve the Z axis orientation vector for this transform.
/// This axis vector is not necessarily unit length and may be scaled.
/// </summary>
//-----------------------------------------------------------------------------
const vec3 & transform_t::zAxis( ) const
{
    return (vec3&)mMatrix[2];
}

//-----------------------------------------------------------------------------
//  Name : xUnitAxis()
/// <summary>
/// Retrieve the unit length X axis orientation vector for this transform.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform_t::xUnitAxis( ) const
{
    vec3 v = glm::normalize( xAxis() );
    return v;
}

//-----------------------------------------------------------------------------
//  Name : yUnitAxis()
/// <summary>
/// Retrieve the unit length Y axis orientation vector for this transform.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform_t::yUnitAxis( ) const
{
	vec3 v = glm::normalize(yAxis());
    return v;
}

//-----------------------------------------------------------------------------
//  Name : zUnitAxis()
/// <summary>
/// Retrieve the unit length Z axis orientation vector for this transform.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform_t::zUnitAxis( ) const
{
    vec3 v = glm::normalize( zAxis() );
    return v;
}


//-----------------------------------------------------------------------------
//  Name : matrix()
/// <summary>
/// Retrieve matrix representation of the transform
/// </summary>
//-----------------------------------------------------------------------------
const mat4& transform_t::matrix( ) const
{
	return mMatrix;
}
mat4& transform_t::matrix()
{
	return mMatrix;
}

//-----------------------------------------------------------------------------
//  Name : orientation()
/// <summary>
/// Retrieve the orientation quat for this transform.
/// </summary>
//-----------------------------------------------------------------------------
quat transform_t::getRotation( ) const
{
    vec3 vScale, vShear, vTranslation;
    quat qRotation;
    if ( decompose( vScale, vShear, qRotation, vTranslation ) )
        return qRotation;
    else
        return quat(0,0,0,1);
}

//-----------------------------------------------------------------------------
//  Name : zero()
/// <summary>
/// Reset the transform back to a completely empty state.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::zero( )
{
    memset( &mMatrix, 0, sizeof(mat4) );
    return *this;
}


//-----------------------------------------------------------------------------
//  Name : decompose()
/// <summary>
/// Decompose a transform into its component parts: scale, rotation and 
/// translation.
/// </summary>
//-----------------------------------------------------------------------------
bool transform_t::decompose( vec3 & vScale, quat & qRotation, vec3 & vTranslation ) const
{
    vec3 vShear;
    return decompose( vScale, vShear, qRotation, vTranslation );
}

//-----------------------------------------------------------------------------
//  Name : decompose()
/// <summary>
/// Decompose a transform into its component parts: rotation and translation.
/// </summary>
//-----------------------------------------------------------------------------
bool transform_t::decompose( quat & qRotation, vec3 & vTranslation ) const
{
    vec3 vScale, vShear;
    return decompose( vScale, vShear, qRotation, vTranslation );
}

//-----------------------------------------------------------------------------
//  Name : decompose() (Static)
/// <summary>
/// Decompose a transform into its component parts: scale, rotation and 
/// translation.
/// </summary>
//-----------------------------------------------------------------------------
bool transform_t::decompose( vec3 & vScale, quat & qRotation, vec3 & vTranslation, const transform_t & t )
{
    vec3 vShear;
    return t.decompose( vScale, vShear, qRotation, vTranslation );
}

//-----------------------------------------------------------------------------
//  Name : decompose() (Static)
/// <summary>
/// Decompose a transform into its component parts: rotation and translation.
/// </summary>
//-----------------------------------------------------------------------------
bool transform_t::decompose( quat & qRotation, vec3 & vTranslation, const transform_t & t )
{
    vec3 vScale, vShear;
    return t.decompose( vScale, vShear, qRotation, vTranslation );
}

//-----------------------------------------------------------------------------
//  Name : decompose()
/// <summary>
/// Decompose a transform into its component parts: local scale, local shear, 
/// rotation and translation.
/// </summary>
//-----------------------------------------------------------------------------
bool transform_t::decompose( vec3 & vScale, vec3 & vShear, quat & qRotation, vec3 & vTranslation ) const
{
	vec4 vPersp;
	glm::decompose(mMatrix, vScale, qRotation, vTranslation, vShear, vPersp);

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
bool transform_t::decompose( vec3 & vScale, vec3 & vShear, quat & qRotation, vec3 & vTranslation, const transform_t & t )
{
    return t.decompose( vScale, vShear, qRotation, vTranslation );
}

//-----------------------------------------------------------------------------
//  Name : compose()
/// <summary>
/// Compose a new transform from its component parts: local scale, local shear, 
/// rotation and translation.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::compose( const vec3 & vScale, const vec3 & vShear, const quat & qRotation, const vec3 & vTranslation )
{
    float a1, a2;
    mat4 m, mR;
    // Convert rotation quat to matrix form.
    mR = glm::mat4_cast( qRotation );

    /*// Shear matrix definition
    _11 = 1; _12 = 0; _13 = 0; _14 = 0;
    _21 = XYShear; _22 = 1; _23 = 0; _24 = 0;
    _31 = XZShear; _32 = YZShear; _33 = 1; _34 = 0;
    _41 = 0; _42 = 0; _43 = 0; _44 = 1;*/

    // (localScale * LocalShear) * rotation
    // 1st row * 1st column
	mMatrix[0][0] = vScale.x * mR[0][0];
	mMatrix[0][1] = vScale.x * mR[0][1];
    mMatrix[0][2] = vScale.x * mR[0][2];

    // 2nd row * 2nd column
    a1 = vScale.y * vShear.x;
	mMatrix[1][0] = a1 * mR[0][0] + vScale.y * mR[1][0];
	mMatrix[1][1] = a1 * mR[0][1] + vScale.y * mR[1][1];
	mMatrix[1][2] = a1 * mR[0][2] + vScale.y * mR[1][2];

    // 3rd row * 3rd column
    a1 = vScale.z * vShear.y; a2 = vScale.z * vShear.z;
	mMatrix[2][0] = a1 * mR[0][0] + a2 * mR[1][0] + vScale.z * mR[2][0];
	mMatrix[2][1] = a1 * mR[0][1] + a2 * mR[1][1] + vScale.z * mR[2][1];
	mMatrix[2][2] = a1 * mR[0][2] + a2 * mR[1][2] + vScale.z * mR[2][2];

    // translation
    mMatrix[3][0] = vTranslation.x; mMatrix[3][1] = vTranslation.y; mMatrix[3][2] = vTranslation.z;

    // identity fourth column.
    mMatrix[0][3] = 0.0f; mMatrix[1][3] = 0.0f; mMatrix[2][3] = 0.0f; mMatrix[3][3] = 1.0f;

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
transform_t & transform_t::compose( const quat & qRotation, const vec3 & vTranslation )
{
    // Convert rotation quat to matrix form.
    mMatrix = glm::mat4_cast( qRotation );
    
    // translation
	mMatrix[3][0] = vTranslation.x; mMatrix[3][1] = vTranslation.y; mMatrix[3][2] = vTranslation.z;

    // Return reference to self in order to allow consecutive operations (i.e. a.rotate(...).scale(...))
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : invert()
/// <summary>
/// Invert this transform object.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::invert( )
{
	mMatrix = glm::inverse(mMatrix);

    return *this;
}

//-----------------------------------------------------------------------------
//  Name : transformCoord()
/// <summary>
/// transforms a 3D vector by the values in this transform object, projecting 
/// the result back into w = 1.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform_t::transformCoord( const vec3 & v ) const
{
	vec4 vOut;
	vOut = mMatrix * vec4{ v, 1.0f };
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
vec3 transform_t::transformCoord( const vec3 & v, const transform_t & t )
{
    return t.transformCoord( v );
}

//-----------------------------------------------------------------------------
//  Name : inversetransformCoord()
/// <summary>
/// transforms a 3D vector by the inverse of the values in this transform 
/// object, projecting the result back into w = 1.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform_t::inverseTransformCoord( const vec3 & v ) const
{
    mat4 im = glm::inverse( mMatrix );
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
vec3 transform_t::inverseTransformCoord( const vec3 & v, const transform_t & t )
{
    return t.inverseTransformCoord( v );
}
    
//-----------------------------------------------------------------------------
//  Name : transformNormal()
/// <summary>
/// transforms the 3D vector normal by the values in this transform object.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform_t::transformNormal( const vec3 & v ) const
{
	mat4 im = mMatrix;
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
vec3 transform_t::transformNormal( const vec3 & v, const transform_t & t )
{
    return t.transformNormal( v );
}

//-----------------------------------------------------------------------------
//  Name : inversetransformNormal()
/// <summary>
/// transforms the 3D vector normal by inverse of the values in this transform
/// object.
/// </summary>
//-----------------------------------------------------------------------------
vec3 transform_t::inverseTransformNormal( const vec3 & v ) const
{
    mat4 im = glm::inverse( mMatrix );
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
vec3 transform_t::inverseTransformNormal( const vec3 & v, const transform_t & t )
{
    return t.inverseTransformNormal( v );
}

//-----------------------------------------------------------------------------
//  Name : rotate ()
/// <summary>
/// rotate the transform around its own local axes.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::rotate( float x, float y, float z )
{
    // No - op?
    if ( !x && !y && !z )
        return *this;
	
	quat qx = glm::angleAxis(x, vec3{ 1.0f, 0.0f, 0.0f });
	quat qy = glm::angleAxis(y, vec3{ 0.0f, 1.0f, 0.0f });
	quat qz = glm::angleAxis(z, vec3{ 0.0f, 0.0f, 1.0f });
	quat q = getRotation();
	q = qz * qy * qx * q;
	setRotation(q);

    // Return reference to self in order to allow consecutive operations (i.e. a.rotate(...).scale(...))
    return *this;
}
transform_t & transform_t::rotateLocal(float x, float y, float z)
{
	// No - op?
	if (!x && !y && !z)
		return *this;

	quat qx = glm::angleAxis(x, xUnitAxis());
	quat qy = glm::angleAxis(y, yUnitAxis());
	quat qz = glm::angleAxis(z, zUnitAxis());
	quat q = getRotation();
	q =  qz * qy * qx * q;
	setRotation(q);

	// Return reference to self in order to allow consecutive operations (i.e. a.rotate(...).scale(...))
	return *this;
}

transform_t & transform_t::rotateLocal(const vec3 & v)
{
	return rotateLocal(v.x, v.y, v.z);
}
//-----------------------------------------------------------------------------
//  Name : rotateAxis ()
/// <summary>
/// Rotate the transform around the parent origin and the specified parent 
/// relative axis.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::rotateAxis( float a, const vec3 & v )
{
    // No - op?
    if ( !a )
        return *this;
    
	quat q = glm::angleAxis(a, v) * getRotation();
	setRotation(q);

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
transform_t & transform_t::scale( float x, float y, float z )
{
    // No - op?
    if ( x == 1.0f && y == 1.0f && z == 1.0f )
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
	mMatrix = glm::scale(mMatrix, vec3{ x, y, z });


    // Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : translate ()
/// <summary>
/// Apply a translation to this transform in its parent space.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::translate( float x, float y, float z )
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
transform_t & transform_t::translate( const vec3 & v )
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
transform_t & transform_t::translateLocal( float x, float y, float z )
{
    position() += xUnitAxis() * x;
    position() += yUnitAxis() * y;
    position() += zUnitAxis() * z;

    // Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : translateLocal ()
/// <summary>
/// Apply a translation to this transform along its own local axes.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::translateLocal( const vec3 & v )
{
    position() += xUnitAxis() * v.x;
    position() += yUnitAxis() * v.y;
    position() += zUnitAxis() * v.z;

    // Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : setPosition ()
/// <summary>
/// Set the position of the origin of this transform with respect its parent.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::setPosition( float x, float y, float z )
{
    // Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
    return setPosition(vec3{ x, y, z });;
}

//-----------------------------------------------------------------------------
//  Name : setPosition ()
/// <summary>
/// Set the position of the origin of this transform with respect its parent.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::setPosition( const vec3 & v )
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
transform_t & transform_t::setScale( float x, float y, float z )
{

    // Clamp the various scales to the minimum length.
	x = glm::max<float>(MinAxisLength, x);
	y = glm::max<float>(MinAxisLength, y);
	z = glm::max<float>(MinAxisLength, z);

    // Generate the new axis vectors;
    (vec3&)mMatrix[0] = xUnitAxis() * x;
    (vec3&)mMatrix[1] = yUnitAxis() * y;
    (vec3&)mMatrix[2] = zUnitAxis() * z;

    // Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : setLocalScale ()
/// <summary>
/// Set the scale of the local axes defined by this transform.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::setScale( const vec3 & v )
{
    return setScale( v.x, v.y, v.z );
}

//-----------------------------------------------------------------------------
//  Name : setLocalShear ()
/// <summary>
/// Set the shear of the local axes defined by this transform.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::setShear( float xy, float xz, float yz )
{
    vec3 vScale, vShear, vTranslation;
    quat qRotation;

    // Decompose the matrix into its component parts
    decompose( vScale, vShear, qRotation, vTranslation );

    // Replace the shear
    vShear.x = xy;
    vShear.y = xz;
    vShear.z = yz;

    // Recompose a new matrix
    compose( vScale, vShear, qRotation, vTranslation );
    
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
transform_t & transform_t::setRotation( const quat & q )
{
    mat4 m = glm::mat4_cast(glm::normalize(q));
    
    // Maintain relative scale.
    vec3 vLength = getScale();
    
    // Scale to original length
	(vec3&)m[0] *= vLength.x;
	(vec3&)m[1] *= vLength.y;
	(vec3&)m[2] *= vLength.z;
    
    // Duplicate position.
    (vec3&)m[3] = getPosition();
	
	mMatrix = m;
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
transform_t & transform_t::setRotation( const vec3 & vX, const vec3 & vY, const vec3 & vZ )
{
    // Get current scale so that it can be preserved.
    vec3 vScale = getScale();

    // Set the new axis vectors (normalized)
	(vec3&)mMatrix[0] = glm::normalize(vX);
	(vec3&)mMatrix[1] = glm::normalize(vY);
	(vec3&)mMatrix[2] = glm::normalize(vZ);

    // Scale back to original length
	(vec3&)mMatrix[0] *= vScale.x;
	(vec3&)mMatrix[1] *= vScale.y;
	(vec3&)mMatrix[2] *= vScale.z;

    // Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : lookAt()
/// <summary>
/// Generate a transform oriented toward the specified point.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::lookAt( const vec3 & vEye, const vec3 & vAt )
{
    // Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
    return lookAt(vEye, vAt, vec3{ 0.0f, 1.0f, 0.0f });
}

//-----------------------------------------------------------------------------
//  Name : lookAt()
/// <summary>
/// Generate a transform oriented toward the specified point.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::lookAt( const vec3 & vEye, const vec3 & vAt, const vec3 & vUpAlign )
{
	mMatrix = glm::lookAt(vEye, vAt, vUpAlign);

    // Return reference to self in order to allow multiple operations (i.e. a.rotate(...).scale(...))
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : scaling()
/// <summary>
/// Generate a new scaling transform, replacing the existing transformation.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::scaling( float x, float y, float z )
{
	mMatrix = glm::scale(vec3{ x, y, z });
    return *this;
}


//-----------------------------------------------------------------------------
//  Name : rotation()
/// <summary>
/// Generate a new rotation transform, replacing the existing transformation.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::rotation( float x, float y, float z )
{
	quat qx, qy, qz;
	qx = glm::angleAxis(x, vec3{ 1.0f, 0.0f, 0.0f });
	qy = glm::angleAxis(y, vec3{ 0.0f, 1.0f, 0.0f });
	qz = glm::angleAxis(z, vec3{ 0.0f, 0.0f, 1.0f });
	quat q = qx * qy * qz;
	mMatrix = glm::mat4_cast(q);
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : rotationAxis()
/// <summary>
/// Generate a new axis rotation transform, replacing the existing 
/// transformation.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::rotationAxis( float a, const vec3 & v )
{
	quat q = glm::angleAxis(a, v);
	mMatrix = glm::mat4_cast(q);
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : translation()
/// <summary>
/// Generate a new translation transform, replacing the existing transformation.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::translation( float x, float y, float z )
{
	mMatrix = glm::translate(vec3{ x, y, z });
    return *this;
}

//-----------------------------------------------------------------------------
//  Name : translation()
/// <summary>
/// Generate a new translation transform, replacing the existing transformation.
/// </summary>
//-----------------------------------------------------------------------------
transform_t & transform_t::translation( const vec3 & v )
{
    mMatrix = glm::translate(v);
    return *this;
}

transform_t transform_t::lerp(transform_t & t1, transform_t & t2, float dt)
{
	transform_t result;

	result.mMatrix = t1.mMatrix + (t2.mMatrix - t1.mMatrix) * dt;

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
int transform_t::compare( const transform_t & t, float fTolerance ) const
{
    float fDifference;
    for ( int i = 0; i < 4; ++i )
    {
        for ( int j = 0; j < 4; ++j )
        {
            fDifference = mMatrix[i][j] - t.mMatrix[i][j];
            if ( glm::abs<float>( fDifference ) > fTolerance )
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
int transform_t::compare( const transform_t & t ) const
{
	return mMatrix == t.mMatrix;
}

//-----------------------------------------------------------------------------
//  Name : isIdentity()
/// <summary>
/// Determine if the transform is in an identity state.
/// </summary>
//-----------------------------------------------------------------------------
bool transform_t::isIdentity( ) const
{
    static const transform_t Identitytransform;
    return ( !!(mMatrix == Identitytransform.mMatrix) );
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