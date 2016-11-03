#pragma once
#include "Core/math/math_includes.h"
#include "Core/common/basetypes.hpp"
#include "Core/reflection/rttr/rttr_enable.h"
#include "Core/serialization/serialization.h"

enum class ProjectionMode : std::uint32_t
{
	Perspective = 0,
	Orthographic = 1
};

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : Camera (Class)
/// <summary>
/// Class representing a camera. Contains functionality for manipulating and
/// updating a camera. It should not be used as a standalone class - see
/// CameraComponent and the Entity system.
/// </summary>
//-----------------------------------------------------------------------------
class Camera
{
public:
	REFLECTABLE(Camera)
	SERIALIZABLE(Camera)
	//-------------------------------------------------------------------------
	// Constructors & Destructors
	//-------------------------------------------------------------------------
	Camera															( );
	~Camera															( );

	//-------------------------------------------------------------------------
	// Public Virtual Methods (Override)
	//-------------------------------------------------------------------------


	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	void								setProjectionMode			( ProjectionMode mode );
    void								setFOV						( float degrees );
	void								setProjectionWindow			( const fRect& rect );
    void								setNearClip					( float distance );
    void								setFarClip					( float distance );
    void								setZoomFactor				( float zoom );

    ProjectionMode						getProjectionMode			( ) const;
    float								getFOV						( ) const;
    const fRect&						getProjectionWindow			( ) const;
    float								getNearClip					( ) const;
    float								getFarClip					( ) const;
    float								getZoomFactor				( ) const;

	void								setViewportSize				( const uSize& viewportSize);
	void								setViewportPos				( const uPoint& viewportPos) { mViewportPos = viewportPos; }
	const uSize&						getViewportSize				( ) const { return mViewportSize;}
	const uPoint&						getViewportPos				( ) const { return mViewportPos;}
	void								setAspectRatio              ( float aspect, bool locked = false );
    float								getAspectRatio              ( ) const;
    bool								isAspectLocked              ( ) const;
    bool								isFrustumLocked             ( ) const;
    void								lockFrustum                 ( bool locked );

	const math::frustum&				getFrustum                  ( );
	const math::frustum&				getClippingVolume           ( );
    const math::transform&				getProj						( );
    const math::transform&				getView						( );
    const math::transform&				getPreviousView				( ) const;
    const math::transform&				getPreviousProj				( ) const;
    void								recordCurrentMatrices       ( );
	
	void								setAAData                   ( const uSize& viewportSize, std::uint32_t currentSubpixelIndex, std::uint32_t temporalAASamples );
	const math::vec4&					getAAData                   ( ) const;

	math::VolumeQuery::E				boundsInFrustum				( const math::bbox & bounds );
	math::VolumeQuery::E				boundsInFrustum				( const math::bbox & bounds, const math::transform & t );
    bool								viewportToRay               ( const uSize & viewportSize, const math::vec2 & point, math::vec3 & rayOriginOut, math::vec3 & rayDirectionOut );
    bool								viewportToWorld             ( const uSize & viewportSize, const math::vec2 & point, const math::plane & plane, math::vec3 & positionOut );
    bool								viewportToMajorAxis         ( const uSize & viewportSize, const math::vec2 & point, const math::vec3 & axisOrigin, math::vec3 & positionOut, math::vec3 & majorAxisOut );
    bool								viewportToMajorAxis         ( const uSize & viewportSize, const math::vec2 & point, const math::vec3 & axisOrigin, const math::vec3 & alignNormal, math::vec3 & positionOut, math::vec3 & majorAxisOut );
    bool								viewportToCamera            ( const uSize & viewportSize, const math::vec3 & point, math::vec3 & positionOut );
    bool								worldToViewport             ( const uSize & viewportSize, const math::vec3 & position, math::vec3 & pointOut, bool clipX = true, bool clipY = true, bool clipZ = true );
    float								estimateZoomFactor          ( const uSize & viewportSize, const math::plane & plane );
    float								estimateZoomFactor          ( const uSize & viewportSize, const math::vec3 & position );
    float								estimateZoomFactor          ( const uSize & viewportSize, const math::plane & plane, float maximumValue );
    float								estimateZoomFactor          ( const uSize & viewportSize, const math::vec3 & position, float maximumValue );
    math::vec3							estimatePickTolerance       ( const uSize & viewportSize, float pixelTolerance, const math::vec3 & referencePosition, const math::transform & objectTransform );

	void								lookAt						( const math::vec3 & vEye, const math::vec3 & vAt );
	void								lookAt						( const math::vec3 & vEye, const math::vec3 & vAt, const math::vec3 & vUp );
    math::vec3							getPosition					( ) const;
	math::vec3							zUnitAxis					( ) const;
	//-------------------------------------------------------------------------
    // Public Virtual Methods
    //-------------------------------------------------------------------------
	virtual math::bbox					getLocalBoundingBox			( );
    virtual void						update                      ( );
	virtual void						onModified					( );

protected:
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------
	ProjectionMode						mProjectionMode;			/// The type of projection currently selected for this camera.
	float								mFOV;						/// Vertical degrees angle (perspective only).
	float								mNearClip;					/// Near clip plane Distance
	float								mFarClip;					/// Far clip plane Distance
	fRect								mProjectionWindow;			/// Projection window (orthographic only)
	float								mZoomFactor;				/// The zoom factor (scale) currently applied to any orthographic view.
	uPoint								mViewportPos = { 0, 0 };
	uSize								mViewportSize = { 0, 0 };
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------
	math::transform						mView;						/// Cached view matrix
	math::transform						mProj;						/// Cached projection matrix.
	math::transform						mPreviousView;				/// Cached "previous" view matrix.
	math::transform						mPreviousProj;				/// Cached "previous" projection matrix.
	math::frustum						mFrustum;                   /// Details regarding the camera frustum.
	math::frustum						mClippingVolume;            /// The near clipping volume (area of space between the camera position and the near plane).
	float								mAspectRatio;               /// The aspect ratio used to generate the correct horizontal degrees (perspective only)
	math::vec4							mAAData;					/// 

	// Update States
	bool								mViewDirty;                 /// View matrix dirty ?
	bool								mProjectionDirty;           /// Projection matrix dirty ?
	bool								mAspectDirty;               /// Has the aspect ratio changed?
	bool								mAspectLocked;              /// Should the aspect ratio be automatically updated by the render driver?
	bool								mFrustumDirty;              /// Are the frustum planes dirty ?
	bool								mFrustumLocked;             /// Is the frustum locked?

};