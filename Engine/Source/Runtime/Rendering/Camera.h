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
	
	//-----------------------------------------------------------------------------
	//  Name : Camera ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	Camera();

	//-----------------------------------------------------------------------------
	//  Name : ~Camera ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	~Camera();

	//-------------------------------------------------------------------------
	// Public Methods
	//-------------------------------------------------------------------------
	//-----------------------------------------------------------------------------
	//  Name : setProjectionMode ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setProjectionMode(ProjectionMode mode);

	//-----------------------------------------------------------------------------
	//  Name : setFOV ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setFOV(float degrees);

	//-----------------------------------------------------------------------------
	//  Name : setProjectionWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setProjectionWindow(const fRect& rect);

	//-----------------------------------------------------------------------------
	//  Name : setNearClip ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setNearClip(float distance);

	//-----------------------------------------------------------------------------
	//  Name : setFarClip ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setFarClip(float distance);

	//-----------------------------------------------------------------------------
	//  Name : setZoomFactor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setZoomFactor(float zoom);

	//-----------------------------------------------------------------------------
	//  Name : getProjectionMode ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	ProjectionMode getProjectionMode() const;

	//-----------------------------------------------------------------------------
	//  Name : getFOV ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float getFOV() const;

	//-----------------------------------------------------------------------------
	//  Name : getProjectionWindow ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const fRect& getProjectionWindow() const;

	//-----------------------------------------------------------------------------
	//  Name : getNearClip ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float getNearClip() const;

	//-----------------------------------------------------------------------------
	//  Name : getFarClip ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float getFarClip() const;

	//-----------------------------------------------------------------------------
	//  Name : getZoomFactor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float getZoomFactor() const;

	//-----------------------------------------------------------------------------
	//  Name : setViewportSize ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setViewportSize(const uSize& viewportSize);

	//-----------------------------------------------------------------------------
	//  Name : setViewportPos ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void setViewportPos(const uPoint& viewportPos) { mViewportPos = viewportPos; }

	//-----------------------------------------------------------------------------
	//  Name : getViewportSize ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const uSize& getViewportSize() const { return mViewportSize; }

	//-----------------------------------------------------------------------------
	//  Name : getViewportPos ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const uPoint& getViewportPos() const { return mViewportPos; }

	//-----------------------------------------------------------------------------
	//  Name : setAspectRatio ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setAspectRatio(float aspect, bool locked = false);

	//-----------------------------------------------------------------------------
	//  Name : getAspectRatio ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float getAspectRatio() const;

	//-----------------------------------------------------------------------------
	//  Name : isAspectLocked ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isAspectLocked() const;

	//-----------------------------------------------------------------------------
	//  Name : isFrustumLocked ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isFrustumLocked() const;

	//-----------------------------------------------------------------------------
	//  Name : lockFrustum ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void lockFrustum(bool locked);

	//-----------------------------------------------------------------------------
	//  Name : getFrustum ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::frustum& getFrustum();

	//-----------------------------------------------------------------------------
	//  Name : getClippingVolume ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::frustum& getClippingVolume();

	//-----------------------------------------------------------------------------
	//  Name : getProj ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform& getProj();

	//-----------------------------------------------------------------------------
	//  Name : getView ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform& getView();

	//-----------------------------------------------------------------------------
	//  Name : getPreviousView ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform& getPreviousView() const;

	//-----------------------------------------------------------------------------
	//  Name : getPreviousProj ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform& getPreviousProj() const;

	//-----------------------------------------------------------------------------
	//  Name : recordCurrentMatrices ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void recordCurrentMatrices();

	//-----------------------------------------------------------------------------
	//  Name : setAAData ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void setAAData(const uSize& viewportSize, std::uint32_t currentSubpixelIndex, std::uint32_t temporalAASamples);

	//-----------------------------------------------------------------------------
	//  Name : getAAData ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::vec4& getAAData() const;

	//-----------------------------------------------------------------------------
	//  Name : boundsInFrustum ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::VolumeQuery::E boundsInFrustum(const math::bbox & bounds);

	//-----------------------------------------------------------------------------
	//  Name : boundsInFrustum ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::VolumeQuery::E boundsInFrustum(const math::bbox & bounds, const math::transform & t);

	//-----------------------------------------------------------------------------
	//  Name : viewportToRay ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewportToRay(const uSize & viewportSize, const math::vec2 & point, math::vec3 & rayOriginOut, math::vec3 & rayDirectionOut);

	//-----------------------------------------------------------------------------
	//  Name : viewportToWorld ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewportToWorld(const uSize & viewportSize, const math::vec2 & point, const math::plane & plane, math::vec3 & positionOut);

	//-----------------------------------------------------------------------------
	//  Name : viewportToMajorAxis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewportToMajorAxis(const uSize & viewportSize, const math::vec2 & point, const math::vec3 & axisOrigin, math::vec3 & positionOut, math::vec3 & majorAxisOut);

	//-----------------------------------------------------------------------------
	//  Name : viewportToMajorAxis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewportToMajorAxis(const uSize & viewportSize, const math::vec2 & point, const math::vec3 & axisOrigin, const math::vec3 & alignNormal, math::vec3 & positionOut, math::vec3 & majorAxisOut);

	//-----------------------------------------------------------------------------
	//  Name : viewportToCamera ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewportToCamera(const uSize & viewportSize, const math::vec3 & point, math::vec3 & positionOut);

	//-----------------------------------------------------------------------------
	//  Name : worldToViewport ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	bool worldToViewport(const uSize & viewportSize, const math::vec3 & position, math::vec3 & pointOut, bool clipX = true, bool clipY = true, bool clipZ = true);

	//-----------------------------------------------------------------------------
	//  Name : estimateZoomFactor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimateZoomFactor(const uSize & viewportSize, const math::plane & plane);

	//-----------------------------------------------------------------------------
	//  Name : estimateZoomFactor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimateZoomFactor(const uSize & viewportSize, const math::vec3 & position);

	//-----------------------------------------------------------------------------
	//  Name : estimateZoomFactor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimateZoomFactor(const uSize & viewportSize, const math::plane & plane, float maximumValue);

	//-----------------------------------------------------------------------------
	//  Name : estimateZoomFactor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimateZoomFactor(const uSize & viewportSize, const math::vec3 & position, float maximumValue);

	//-----------------------------------------------------------------------------
	//  Name : estimatePickTolerance ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 estimatePickTolerance(const uSize & viewportSize, float pixelTolerance, const math::vec3 & referencePosition, const math::transform & objectTransform);

	//-----------------------------------------------------------------------------
	//  Name : lookAt ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void lookAt(const math::vec3 & vEye, const math::vec3 & vAt);

	//-----------------------------------------------------------------------------
	//  Name : lookAt ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void lookAt(const math::vec3 & vEye, const math::vec3 & vAt, const math::vec3 & vUp);

	//-----------------------------------------------------------------------------
	//  Name : getPosition ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 getPosition() const;

	//-----------------------------------------------------------------------------
	//  Name : zUnitAxis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 zUnitAxis() const;

	//-----------------------------------------------------------------------------
	//  Name : getLocalBoundingBox (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual math::bbox getLocalBoundingBox();

	//-----------------------------------------------------------------------------
	//  Name : update (virtual )
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void update();

	virtual void onModified();

protected:
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------
	/// The type of projection currently selected for this camera.
	ProjectionMode mProjectionMode;
	/// Vertical degrees angle (perspective only).
	float mFOV;
	/// Near clip plane Distance
	float mNearClip;
	/// Far clip plane Distance
	float mFarClip;
	/// Projection window (orthographic only)
	fRect mProjectionWindow;
	/// The zoom factor (scale) currently applied to any orthographic view.
	float mZoomFactor;
	/// Viewport position
	uPoint mViewportPos = { 0, 0 };
	/// Viewport size
	uSize mViewportSize = { 0, 0 };
	/// Cached view matrix
	math::transform mView;
	/// Cached projection matrix.
	math::transform mProj;
	/// Cached "previous" view matrix.
	math::transform mPreviousView;
	/// Cached "previous" projection matrix.
	math::transform mPreviousProj;
	/// Details regarding the camera frustum.
	math::frustum mFrustum;
	/// The near clipping volume (area of space between the camera position and the near plane).
	math::frustum mClippingVolume;
	/// The aspect ratio used to generate the correct horizontal degrees (perspective only)
	float mAspectRatio;
	/// Anti-aliasing data.
	math::vec4 mAAData;
	/// View matrix dirty ?
	bool mViewDirty;
	/// Projection matrix dirty ?
	bool mProjectionDirty;
	/// Has the aspect ratio changed?
	bool mAspectDirty;
	/// Should the aspect ratio be automatically updated by the render driver?
	bool mAspectLocked;
	/// Are the frustum planes dirty ?
	bool mFrustumDirty;
	/// Is the frustum locked?
	bool mFrustumLocked;
};