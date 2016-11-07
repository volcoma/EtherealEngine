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
	// Public Methods
	//-------------------------------------------------------------------------

	//-----------------------------------------------------------------------------
	//  Name : setProjectionMode ()
	/// <summary>
	/// Sets the current projection mode for this camera (i.e. orthographic
	/// or perspective).
	/// </summary>
	//-----------------------------------------------------------------------------
	void setProjectionMode(ProjectionMode mode);

	//-----------------------------------------------------------------------------
	//  Name : setFOV ()
	/// <summary>
	/// Sets the field of view angle of this camera (perspective only).
	/// </summary>
	//-----------------------------------------------------------------------------
	void setFOV(float degrees);

	//-----------------------------------------------------------------------------
	//  Name : setProjectionWindow ()
	/// <summary>
	/// Sets offsets for the projection window (orthographic only).
	/// </summary>
	//-----------------------------------------------------------------------------
	void setProjectionWindow(const fRect& rect);

	//-----------------------------------------------------------------------------
	//  Name : setNearClip ()
	/// <summary>
	/// Set the near plane distance
	/// </summary>
	//-----------------------------------------------------------------------------
	void setNearClip(float distance);

	//-----------------------------------------------------------------------------
	//  Name : setFarClip()
	/// <summary>
	/// Set the far plane distance
	/// </summary>
	//-----------------------------------------------------------------------------
	void setFarClip(float distance);

	//-----------------------------------------------------------------------------
	// Name : setZoomFactor( )
	/// <summary>
	/// Set the zoom factor (scale) currently applied to any orthographic view.
	/// </summary>
	//-----------------------------------------------------------------------------
	void setZoomFactor(float zoom);

	//-----------------------------------------------------------------------------
	//  Name : getProjectionMode ()
	/// <summary>
	/// Retrieve the current projection mode for this camera.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline ProjectionMode getProjectionMode() const { return mProjectionMode; }

	//-----------------------------------------------------------------------------
	//  Name : getFOV()
	/// <summary>
	/// Retrieve the current field of view angle in degrees.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getFOV() const { return mFOV; }

	//-----------------------------------------------------------------------------
	//  Name : getProjectionWindow ()
	/// <summary>
	/// Retrieve offsets for the projection window (orthographic only).
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const fRect& getProjectionWindow() const { return mProjectionWindow; }

	//-----------------------------------------------------------------------------
	//  Name : getNearClip()
	/// <summary>
	/// Retrieve the distance from the camera to the near clip plane.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getNearClip() const { return mNearClip; }

	//-----------------------------------------------------------------------------
	//  Name : getFarClip()
	/// <summary>
	/// Retrieve the distance from the camera to the far clip plane.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getFarClip() const { return mFarClip; }

	//-----------------------------------------------------------------------------
	// Name : getZoomFactor( )
	/// <summary>
	/// Get the zoom factor (scale) currently applied to any orthographic view.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getZoomFactor() const { return mZoomFactor; }

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
	inline const uPoint& getViewportPos() const { return mViewportPos; }

	//-----------------------------------------------------------------------------
	//  Name : setAspectRatio ()
	/// <summary>
	/// Set the aspect ratio that should be used to generate the horizontal
	/// FOV angle (perspective only).
	/// </summary>
	//-----------------------------------------------------------------------------
	void setAspectRatio(float aspect, bool locked = false);

	//-----------------------------------------------------------------------------
	//  Name : getAspectRatio()
	/// <summary>
	/// Retrieve the aspect ratio used to generate the horizontal FOV angle.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float getAspectRatio() const { return mAspectRatio; }

	//-----------------------------------------------------------------------------
	//  Name : isAspectLocked()
	/// <summary>
	/// Determine if the aspect ratio is currently being updated by the
	/// render driver.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool isAspectLocked() const;

	//-----------------------------------------------------------------------------
	//  Name : isFrustumLocked ()
	/// <summary>
	/// Inform the caller whether or not the frustum is currently locked
	/// This is useful as a debugging tool.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool isFrustumLocked() const { return mFrustumLocked; }

	//-----------------------------------------------------------------------------
	//  Name : lockFrustum ()
	/// <summary>
	/// Prevent the frustum from updating.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void lockFrustum(bool locked) { mFrustumLocked = locked; }

	//-----------------------------------------------------------------------------
	//  Name : getFrustum()
	/// <summary>
	/// Retrieve the current camera object frustum.
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::frustum& getFrustum();

	//-----------------------------------------------------------------------------
	//  Name : getClippingVolume()
	/// <summary>
	/// Retrieve the frustum / volume that represents the space between the camera 
	/// position and its near plane. This frustum represents the 'volume' that can 
	/// end up clipping geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::frustum& getClippingVolume();

	//-----------------------------------------------------------------------------
	//  Name : getProj ()
	/// <summary>
	/// Return the current projection matrix.
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform& getProj();

	//-----------------------------------------------------------------------------
	//  Name : getView ()
	/// <summary>
	/// Return the current view matrix.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::transform& getView() const { return mView; }

	//-----------------------------------------------------------------------------
	//  Name : getPreviousView ()
	/// <summary>
	/// Retrieve a copy of the view matrix recorded with the most recent call
	/// to recordCurrentMatrices().
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::transform& getPreviousView() const { return mPreviousView; }

	//-----------------------------------------------------------------------------
	//  Name : getPreviousProj ()
	/// <summary>
	/// Retrieve a copy of the projection matrix recorded with the most
	/// recent call to recordCurrentMatrices().
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::transform& getPreviousProj() const { return mPreviousProj; }

	//-----------------------------------------------------------------------------
	//  Name : recordCurrentMatrices ()
	/// <summary>
	/// Make a copy of the current view / projection matrices before they
	/// are changed. Useful for performing effects such as motion blur.
	/// </summary>
	//-----------------------------------------------------------------------------
	void recordCurrentMatrices();

	//-----------------------------------------------------------------------------
	//  Name : setAAData ()
	/// <summary>
	/// Sets the current jitter value for temporal anti-aliasing
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
	inline const math::vec4& getAAData() const { return mAAData; }

	//-----------------------------------------------------------------------------
	//  Name : boundsInFrustum ()
	/// <summary>
	/// Determine whether or not the AABB specified falls within the frustum.
	/// </summary>
	//-----------------------------------------------------------------------------
	math::VolumeQuery::E boundsInFrustum(const math::bbox & bounds);

	//-----------------------------------------------------------------------------
	//  Name : boundsInFrustum ()
	/// <summary>
	/// Determine whether or not the OOBB specified is within the frustum.
	/// </summary>
	//-----------------------------------------------------------------------------
	math::VolumeQuery::E boundsInFrustum(const math::bbox & bounds, const math::transform & t);

	//-----------------------------------------------------------------------------
	//  Name : viewportToRay()
	/// <summary>
	/// Convert the specified screen position into a ray origin and direction
	/// vector, suitable for use during picking.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewportToRay(const uSize & viewportSize, const math::vec2 & point, math::vec3 & rayOriginOut, math::vec3 & rayDirectionOut);

	//-----------------------------------------------------------------------------
	//  Name : viewportToWorld ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will cast that 
	/// ray and return the world space position on the specified plane. The value
	/// is returned via the world parameter passed.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewportToWorld(const uSize & viewportSize, const math::vec2 & point, const math::plane & plane, math::vec3 & positionOut);

	//-----------------------------------------------------------------------------
	//  Name : viewportToMajorAxis ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will cast that 
	/// ray and return the world space intersection point on one of the major axis
	/// planes selected based on the camera look vector.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewportToMajorAxis(const uSize & viewportSize, const math::vec2 & point, const math::vec3 & axisOrigin, math::vec3 & positionOut, math::vec3 & majorAxisOut);

	//-----------------------------------------------------------------------------
	//  Name : viewportToMajorAxis ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will cast that 
	/// ray and return the world space intersection point on one of the major axis
	/// planes selected based on the specified normal.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewportToMajorAxis(const uSize & viewportSize, const math::vec2 & point, const math::vec3 & axisOrigin, const math::vec3 & alignNormal, math::vec3 & positionOut, math::vec3 & majorAxisOut);

	//-----------------------------------------------------------------------------
	//  Name : viewportToCamera ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will convert
	/// the point into a camera space position at the near plane.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewportToCamera(const uSize & viewportSize, const math::vec3 & point, math::vec3 & positionOut);

	//-----------------------------------------------------------------------------
	//  Name : worldToViewport()
	/// <summary>
	/// Transform a point from world space, into screen space. Returns false 
	/// if the point was clipped off the screen.
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
	/// Given the current viewport type and projection mode, estimate the "zoom"
	/// factor that can be used for scaling various operations relative to their
	/// "scale" as it appears in the viewport.
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimateZoomFactor(const uSize & viewportSize, const math::vec3 & position);

	//-----------------------------------------------------------------------------
	//  Name : estimateZoomFactor ()
	/// <summary>
	/// Given the current viewport type and projection mode, estimate the "zoom"
	/// factor that can be used for scaling various operations relative to their
	/// "scale" as it appears in the viewport.
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimateZoomFactor(const uSize & viewportSize, const math::plane & plane, float maximumValue);

	//-----------------------------------------------------------------------------
	// Name : estimateZoomFactor ()
	/// <summary>
	/// Given the current viewport type and projection mode, estimate the "zoom"
	/// factor that can be used for scaling various operations relative to the
	/// "scale" of an object as it appears in the viewport at the specified position.
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimateZoomFactor(const uSize & viewportSize, const math::vec3 & position, float maximumValue);

	//-----------------------------------------------------------------------------
	// Name : estimatePickTolerance ()
	/// <summary>
	/// Estimate the distance (along each axis) from the specified object space 
	/// point to use as a tolerance for picking.
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
	//  Name : getLocalBoundingBox ()
	/// <summary>
	/// Retrieve the bounding box of this object.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual math::bbox getLocalBoundingBox();

	//-----------------------------------------------------------------------------
	//  Name : () touch
	/// <summary>
	/// When the camera is modified.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual void touch();

protected:
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------
	/// The type of projection currently selected for this camera.
	ProjectionMode mProjectionMode = ProjectionMode::Perspective;
	/// Vertical degrees angle (perspective only).
	float mFOV = 60.0f;
	/// Near clip plane Distance
	float mNearClip = 0.1f;
	/// Far clip plane Distance
	float mFarClip = 1000.0f;
	/// Projection window (orthographic only)
	fRect mProjectionWindow = { 0.0f, 0.0f, 0.0f, 0.0f };
	/// The zoom factor (scale) currently applied to any orthographic view.
	float mZoomFactor = 1.0f;
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
	float mAspectRatio = 1.0f;
	/// Anti-aliasing data.
	math::vec4 mAAData = { 0.0f, 0.0f, 0.0f, 0.0f };
	/// View matrix dirty ?
	bool mViewDirty = true;
	/// Projection matrix dirty ?
	bool mProjectionDirty = true;
	/// Has the aspect ratio changed?
	bool mAspectDirty = true;
	/// Are the frustum planes dirty ?
	bool mFrustumDirty = true;
	/// Should the aspect ratio be automatically updated by the render driver?
	bool mAspectLocked = false;
	/// Is the frustum locked?
	bool mFrustumLocked = false;
};