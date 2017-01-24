#pragma once
#include "core/math/math_includes.h"
#include "core/common/basetypes.hpp"
#include "core/reflection/rttr/rttr_enable.h"
#include "core/serialization/serialization.h"

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
	//  Name : set_projection_mode ()
	/// <summary>
	/// Sets the current projection mode for this camera (i.e. orthographic
	/// or perspective).
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_projection_mode(ProjectionMode mode);

	//-----------------------------------------------------------------------------
	//  Name : set_fov ()
	/// <summary>
	/// Sets the field of view angle of this camera (perspective only).
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_fov(float degrees);

	//-----------------------------------------------------------------------------
	//  Name : set_near_clip ()
	/// <summary>
	/// Set the near plane distance
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_near_clip(float distance);

	//-----------------------------------------------------------------------------
	//  Name : set_far_clip()
	/// <summary>
	/// Set the far plane distance
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_far_clip(float distance);

	//-----------------------------------------------------------------------------
	// Name : set_orthographic_size( )
	/// <summary>
	/// Sets the half of the vertical size of the viewing volume in world units.
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_orthographic_size(float size);

	//-----------------------------------------------------------------------------
	//  Name : get_projection_mode ()
	/// <summary>
	/// Retrieve the current projection mode for this camera.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline ProjectionMode get_projection_mode() const { return _projection_mode; }

	//-----------------------------------------------------------------------------
	//  Name : get_fov()
	/// <summary>
	/// Retrieve the current field of view angle in degrees.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_fov() const { return _fov; }

	//-----------------------------------------------------------------------------
	//  Name : get_near_clip()
	/// <summary>
	/// Retrieve the distance from the camera to the near clip plane.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_near_clip() const { return _near_clip; }

	//-----------------------------------------------------------------------------
	//  Name : get_far_clip()
	/// <summary>
	/// Retrieve the distance from the camera to the far clip plane.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_far_clip() const { return _far_clip; }

	//-----------------------------------------------------------------------------
	// Name : get_ortho_size( )
	/// <summary>
	/// Get the orthographic size.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_ortho_size() const { return _ortho_size; }

	//-----------------------------------------------------------------------------
	//  Name : get_zoom_factor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float get_zoom_factor() const;

	//-----------------------------------------------------------------------------
	//  Name : get_ppu ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float get_ppu() const;

	//-----------------------------------------------------------------------------
	//  Name : set_viewport_size ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_viewport_size(const uSize& viewportSize);

	//-----------------------------------------------------------------------------
	//  Name : set_viewport_pos ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_viewport_pos(const uPoint& viewportPos) { _viewport_pos = viewportPos; }

	//-----------------------------------------------------------------------------
	//  Name : get_viewport_size ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	const uSize& get_viewport_size() const { return _viewport_size; }

	//-----------------------------------------------------------------------------
	//  Name : get_viewport_pos ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const uPoint& get_viewport_pos() const { return _viewport_pos; }

	//-----------------------------------------------------------------------------
	//  Name : set_aspect_ratio ()
	/// <summary>
	/// Set the aspect ratio that should be used to generate the horizontal
	/// FOV angle (perspective only).
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_aspect_ratio(float aspect, bool locked = false);

	//-----------------------------------------------------------------------------
	//  Name : get_aspect_ratio()
	/// <summary>
	/// Retrieve the aspect ratio used to generate the horizontal FOV angle.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_aspect_ratio() const { return _aspect_ratio; }

	//-----------------------------------------------------------------------------
	//  Name : is_aspect_locked()
	/// <summary>
	/// Determine if the aspect ratio is currently being updated by the
	/// render driver.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool is_aspect_locked() const;

	//-----------------------------------------------------------------------------
	//  Name : is_frustum_locked ()
	/// <summary>
	/// Inform the caller whether or not the frustum is currently locked
	/// This is useful as a debugging tool.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline bool is_frustum_locked() const { return _frustum_locked; }

	//-----------------------------------------------------------------------------
	//  Name : lock_frustum ()
	/// <summary>
	/// Prevent the frustum from updating.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void lock_frustum(bool locked) { _frustum_locked = locked; }

	//-----------------------------------------------------------------------------
	//  Name : get_frustum()
	/// <summary>
	/// Retrieve the current camera object frustum.
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::frustum& get_frustum();

	//-----------------------------------------------------------------------------
	//  Name : get_clipping_volume()
	/// <summary>
	/// Retrieve the frustum / volume that represents the space between the camera 
	/// position and its near plane. This frustum represents the 'volume' that can 
	/// end up clipping geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::frustum& get_clipping_volume();

	//-----------------------------------------------------------------------------
	//  Name : get_projection ()
	/// <summary>
	/// Return the current projection matrix.
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform_t& get_projection();

	//-----------------------------------------------------------------------------
	//  Name : get_view ()
	/// <summary>
	/// Return the current view matrix.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::transform_t& get_view() const { return _view; }

	//-----------------------------------------------------------------------------
	//  Name : get_view_proj ()
	/// <summary>
	/// Return the current view-projection matrix.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline math::transform_t get_view_proj() { return get_projection() * get_view(); }

	//-----------------------------------------------------------------------------
	//  Name : get_last_view ()
	/// <summary>
	/// Retrieve a copy of the view matrix recorded with the most recent call
	/// to recordCurrentMatrices().
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::transform_t& get_last_view() const { return _last_view; }

	//-----------------------------------------------------------------------------
	//  Name : get_last_projection ()
	/// <summary>
	/// Retrieve a copy of the projection matrix recorded with the most
	/// recent call to recordCurrentMatrices().
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::transform_t& get_last_projection() const { return _last_projection; }

	//-----------------------------------------------------------------------------
	//  Name : record_current_matrices ()
	/// <summary>
	/// Make a copy of the current view / projection matrices before they
	/// are changed. Useful for performing effects such as motion blur.
	/// </summary>
	//-----------------------------------------------------------------------------
	void record_current_matrices();

	//-----------------------------------------------------------------------------
	//  Name : set_aa_data ()
	/// <summary>
	/// Sets the current jitter value for temporal anti-aliasing
	/// </summary>
	//-----------------------------------------------------------------------------
	void set_aa_data(const uSize& viewportSize, std::uint32_t currentSubpixelIndex, std::uint32_t temporalAASamples);

	//-----------------------------------------------------------------------------
	//  Name : get_aa_data ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::vec4& get_aa_data() const { return _aa_data; }

	//-----------------------------------------------------------------------------
	//  Name : bounds_in_frustum ()
	/// <summary>
	/// Determine whether or not the AABB specified falls within the frustum.
	/// </summary>
	//-----------------------------------------------------------------------------
	math::VolumeQuery::E bounds_in_frustum(const math::bbox & bounds);

	//-----------------------------------------------------------------------------
	//  Name : bounds_in_frustum ()
	/// <summary>
	/// Determine whether or not the OOBB specified is within the frustum.
	/// </summary>
	//-----------------------------------------------------------------------------
	math::VolumeQuery::E bounds_in_frustum(const math::bbox & bounds, const math::transform_t & t);

	//-----------------------------------------------------------------------------
	//  Name : viewport_to_ray()
	/// <summary>
	/// Convert the specified screen position into a ray origin and direction
	/// vector, suitable for use during picking.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewport_to_ray(const math::vec2 & point, math::vec3 & rayOriginOut, math::vec3 & rayDirectionOut);

	//-----------------------------------------------------------------------------
	//  Name : viewport_to_world ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will cast that 
	/// ray and return the world space position on the specified plane. The value
	/// is returned via the world parameter passed.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewport_to_world(const math::vec2 & point, const math::plane & plane, math::vec3 & positionOut, bool clip);

	//-----------------------------------------------------------------------------
	//  Name : viewport_to_major_axis ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will cast that 
	/// ray and return the world space intersection point on one of the major axis
	/// planes selected based on the camera look vector.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewport_to_major_axis(const math::vec2 & point, const math::vec3 & axisOrigin, math::vec3 & positionOut, math::vec3 & majorAxisOut);

	//-----------------------------------------------------------------------------
	//  Name : viewportToMajorAxis ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will cast that 
	/// ray and return the world space intersection point on one of the major axis
	/// planes selected based on the specified normal.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewport_to_major_axis(const math::vec2 & point, const math::vec3 & axisOrigin, const math::vec3 & alignNormal, math::vec3 & positionOut, math::vec3 & majorAxisOut);

	//-----------------------------------------------------------------------------
	//  Name : viewport_to_camera ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will convert
	/// the point into a camera space position at the near plane.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewport_to_camera(const math::vec3 & point, math::vec3 & positionOut);

	//-----------------------------------------------------------------------------
	//  Name : world_to_viewport()
	/// <summary>
	/// Transform a point from world space, into screen space. Returns false 
	/// if the point was clipped off the screen.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool world_to_viewport(const uPoint& viewport_pos, const uSize& viewport_size, const math::vec3& pos, math::vec3 & point, bool clipX = true, bool clipY = true, bool clipZ = true);

	//-----------------------------------------------------------------------------
	//  Name : estimate_zoom_factor ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimate_zoom_factor(const math::plane & plane);

	//-----------------------------------------------------------------------------
	//  Name : estimate_zoom_factor ()
	/// <summary>
	/// Given the current viewport type and projection mode, estimate the "zoom"
	/// factor that can be used for scaling various operations relative to their
	/// "scale" as it appears in the viewport.
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimate_zoom_factor(const math::vec3 & position);

	//-----------------------------------------------------------------------------
	//  Name : estimate_zoom_factor ()
	/// <summary>
	/// Given the current viewport type and projection mode, estimate the "zoom"
	/// factor that can be used for scaling various operations relative to their
	/// "scale" as it appears in the viewport.
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimate_zoom_factor(const math::plane & plane, float maximumValue);

	//-----------------------------------------------------------------------------
	// Name : estimate_zoom_factor ()
	/// <summary>
	/// Given the current viewport type and projection mode, estimate the "zoom"
	/// factor that can be used for scaling various operations relative to the
	/// "scale" of an object as it appears in the viewport at the specified position.
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimate_zoom_factor(const math::vec3 & position, float maximumValue);

	//-----------------------------------------------------------------------------
	// Name : estimate_pick_tolerance ()
	/// <summary>
	/// Estimate the distance (along each axis) from the specified object space 
	/// point to use as a tolerance for picking.
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 estimate_pick_tolerance(float pixelTolerance, const math::vec3 & referencePosition, const math::transform_t & objectTransform);

	//-----------------------------------------------------------------------------
	//  Name : look_at ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void look_at(const math::vec3 & vEye, const math::vec3 & vAt);

	//-----------------------------------------------------------------------------
	//  Name : look_at ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	void look_at(const math::vec3 & vEye, const math::vec3 & vAt, const math::vec3 & vUp);

	//-----------------------------------------------------------------------------
	//  Name : get_position ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 get_position() const;

	//-----------------------------------------------------------------------------
	//  Name : x_unit_axis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 x_unit_axis() const;

	//-----------------------------------------------------------------------------
	//  Name : y_unit_axis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 y_unit_axis() const;

	//-----------------------------------------------------------------------------
	//  Name : z_unit_axis ()
	/// <summary>
	/// 
	/// 
	/// 
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 z_unit_axis() const;

	//-----------------------------------------------------------------------------
	//  Name : get_local_bounding_box ()
	/// <summary>
	/// Retrieve the bounding box of this object.
	/// </summary>
	//-----------------------------------------------------------------------------
	virtual math::bbox get_local_bounding_box();

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
	ProjectionMode _projection_mode = ProjectionMode::Perspective;
	/// Vertical degrees angle (perspective only).
	float _fov = 60.0f;
	/// Near clip plane Distance
	float _near_clip = 0.1f;
	/// Far clip plane Distance
	float _far_clip = 1000.0f;
	/// Camera's half-size when in orthographic mode.
	float _ortho_size = 5;
	/// Viewport position
	uPoint _viewport_pos = { 0, 0 };
	/// Viewport size
	uSize _viewport_size = { 0, 0 };
	/// Cached view matrix
	math::transform_t _view;
	/// Cached projection matrix.
	math::transform_t _projection;
	/// Cached "previous" view matrix.
	math::transform_t _last_view;
	/// Cached "previous" projection matrix.
	math::transform_t _last_projection;
	/// Details regarding the camera frustum.
	math::frustum _frustum;
	/// The near clipping volume (area of space between the camera position and the near plane).
	math::frustum _clipping_volume;
	/// The aspect ratio used to generate the correct horizontal degrees (perspective only)
	float _aspect_ratio = 1.0f;
	/// Anti-aliasing data.
	math::vec4 _aa_data = { 0.0f, 0.0f, 0.0f, 0.0f };
	/// View matrix dirty ?
	bool _view_dirty = true;
	/// Projection matrix dirty ?
	bool _projection_dirty = true;
	/// Has the aspect ratio changed?
	bool _aspect_dirty = true;
	/// Are the frustum planes dirty ?
	bool _frustum_dirty = true;
	/// Should the aspect ratio be automatically updated by the render driver?
	bool _aspect_locked = false;
	/// Is the frustum locked?
	bool _frustum_locked = false;
};