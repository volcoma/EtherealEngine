#pragma once
#include "core/common/basetypes.hpp"
#include "core/math/math_includes.h"
#include "core/reflection/registration.h"
#include "core/serialization/serialization.h"

enum class projection_mode : std::uint32_t
{
	perspective = 0,
	orthographic = 1
};

//-----------------------------------------------------------------------------
// Main Class Declarations
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
//  Name : camera (Class)
/// <summary>
/// Class representing a camera. Contains functionality for manipulating and
/// updating a camera. It should not be used as a standalone class - see
/// camera_component and the entity system.
/// </summary>
//-----------------------------------------------------------------------------
class camera
{
public:
	REFLECTABLE(camera)
	SERIALIZABLE(camera)

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
	void set_projection_mode(projection_mode mode);

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
	inline projection_mode get_projection_mode() const
	{
		return projection_mode_;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_fov()
	/// <summary>
	/// Retrieve the current field of view angle in degrees.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_fov() const
	{
		return fov_;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_near_clip()
	/// <summary>
	/// Retrieve the distance from the camera to the near clip plane.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_near_clip() const
	{
		return near_clip_;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_far_clip()
	/// <summary>
	/// Retrieve the distance from the camera to the far clip plane.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_far_clip() const
	{
		return far_clip_;
	}

	//-----------------------------------------------------------------------------
	// Name : get_ortho_size( )
	/// <summary>
	/// Get the orthographic size.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline float get_ortho_size() const
	{
		return ortho_size_;
	}

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
	void set_viewport_size(const usize32_t& viewportSize);

	//-----------------------------------------------------------------------------
	//  Name : set_viewport_pos ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void set_viewport_pos(const upoint32_t& viewportPos)
	{
		viewport_pos_ = viewportPos;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_viewport_size ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	const usize32_t& get_viewport_size() const
	{
		return viewport_size_;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_viewport_pos ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const upoint32_t& get_viewport_pos() const
	{
		return viewport_pos_;
	}

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
	inline float get_aspect_ratio() const
	{
		return aspect_ratio_;
	}

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
	inline bool is_frustum_locked() const
	{
		return frustum_locked_;
	}

	//-----------------------------------------------------------------------------
	//  Name : lock_frustum ()
	/// <summary>
	/// Prevent the frustum from updating.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline void lock_frustum(bool locked)
	{
		frustum_locked_ = locked;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_frustum()
	/// <summary>
	/// Retrieve the current camera object frustum.
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::frustum& get_frustum() const;

	//-----------------------------------------------------------------------------
	//  Name : get_clipping_volume()
	/// <summary>
	/// Retrieve the frustum / volume that represents the space between the camera
	/// position and its near plane. This frustum represents the 'volume' that can
	/// end up clipping geometry.
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::frustum& get_clipping_volume() const;

	//-----------------------------------------------------------------------------
	//  Name : get_projection ()
	/// <summary>
	/// Return the current projection matrix.
	/// </summary>
	//-----------------------------------------------------------------------------
	const math::transform& get_projection() const;

	//-----------------------------------------------------------------------------
	//  Name : get_view ()
	/// <summary>
	/// Return the current view matrix.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::transform& get_view() const
	{
		return view_;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_view_projection ()
	/// <summary>
	/// Return the current view-projection matrix.
	/// </summary>
	//-----------------------------------------------------------------------------
	inline math::transform get_view_projection() const
	{
		return get_projection() * get_view();
	}

	//-----------------------------------------------------------------------------
	//  Name : get_last_view ()
	/// <summary>
	/// Retrieve a copy of the view matrix recorded with the most recent call
	/// to recordCurrentMatrices().
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::transform& get_last_view() const
	{
		return last_view_;
	}

	//-----------------------------------------------------------------------------
	//  Name : get_last_projection ()
	/// <summary>
	/// Retrieve a copy of the projection matrix recorded with the most
	/// recent call to recordCurrentMatrices().
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::transform& get_last_projection() const
	{
		return last_projection_;
	}

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
	void set_aa_data(const usize32_t& viewportSize, std::uint32_t currentSubpixelIndex,
					 std::uint32_t temporalAASamples);

	//-----------------------------------------------------------------------------
	//  Name : get_aa_data ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	inline const math::vec4& get_aa_data() const
	{
		return aa_data_;
	}

	//-----------------------------------------------------------------------------
	//  Name : bounds_in_frustum ()
	/// <summary>
	/// Determine whether or not the AABB specified falls within the frustum.
	/// </summary>
	//-----------------------------------------------------------------------------
	math::volume_query bounds_in_frustum(const math::bbox& bounds) const;

	//-----------------------------------------------------------------------------
	//  Name : bounds_in_frustum ()
	/// <summary>
	/// Determine whether or not the OOBB specified is within the frustum.
	/// </summary>
	//-----------------------------------------------------------------------------
	math::volume_query bounds_in_frustum(const math::bbox& bounds, const math::transform& t) const;

	//-----------------------------------------------------------------------------
	//  Name : viewport_to_ray()
	/// <summary>
	/// Convert the specified screen position into a ray origin and direction
	/// vector, suitable for use during picking.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewport_to_ray(const math::vec2& point, math::vec3& rayOriginOut,
						 math::vec3& rayDirectionOut) const;

	//-----------------------------------------------------------------------------
	//  Name : viewport_to_world ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will cast
	/// that
	/// ray and return the world space position on the specified plane. The value
	/// is returned via the world parameter passed.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewport_to_world(const math::vec2& point, const math::plane& plane, math::vec3& positionOut,
						   bool clip) const;

	//-----------------------------------------------------------------------------
	//  Name : viewport_to_major_axis ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will cast
	/// that
	/// ray and return the world space intersection point on one of the major axis
	/// planes selected based on the camera look vector.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewport_to_major_axis(const math::vec2& point, const math::vec3& axisOrigin,
								math::vec3& positionOut, math::vec3& majorAxisOut) const;

	//-----------------------------------------------------------------------------
	//  Name : viewportToMajorAxis ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will cast
	/// that
	/// ray and return the world space intersection point on one of the major axis
	/// planes selected based on the specified normal.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewport_to_major_axis(const math::vec2& point, const math::vec3& axisOrigin,
								const math::vec3& alignNormal, math::vec3& positionOut,
								math::vec3& majorAxisOut) const;

	//-----------------------------------------------------------------------------
	//  Name : viewport_to_camera ()
	/// <summary>
	/// Given a view screen position (in screen space) this function will convert
	/// the point into a camera space position at the near plane.
	/// </summary>
	//-----------------------------------------------------------------------------
	bool viewport_to_camera(const math::vec3& point, math::vec3& positionOut) const;

	//-----------------------------------------------------------------------------
	//  Name : world_to_viewport()
	/// <summary>
	/// Transform a point from world space, into screen space. Returns false
	/// if the point was clipped off the screen.
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 world_to_viewport(const math::vec3& pos) const;

	//-----------------------------------------------------------------------------
	//  Name : estimate_zoom_factor ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimate_zoom_factor(const math::plane& plane) const;

	//-----------------------------------------------------------------------------
	//  Name : estimate_zoom_factor ()
	/// <summary>
	/// Given the current viewport type and projection mode, estimate the "zoom"
	/// factor that can be used for scaling various operations relative to their
	/// "scale" as it appears in the viewport.
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimate_zoom_factor(const math::vec3& position) const;

	//-----------------------------------------------------------------------------
	//  Name : estimate_zoom_factor ()
	/// <summary>
	/// Given the current viewport type and projection mode, estimate the "zoom"
	/// factor that can be used for scaling various operations relative to their
	/// "scale" as it appears in the viewport.
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimate_zoom_factor(const math::plane& plane, float maximumValue) const;

	//-----------------------------------------------------------------------------
	// Name : estimate_zoom_factor ()
	/// <summary>
	/// Given the current viewport type and projection mode, estimate the "zoom"
	/// factor that can be used for scaling various operations relative to the
	/// "scale" of an object as it appears in the viewport at the specified
	/// position.
	/// </summary>
	//-----------------------------------------------------------------------------
	float estimate_zoom_factor(const math::vec3& position, float maximumValue) const;

	//-----------------------------------------------------------------------------
	// Name : estimate_pick_tolerance ()
	/// <summary>
	/// Estimate the distance (along each axis) from the specified object space
	/// point to use as a tolerance for picking.
	/// </summary>
	//-----------------------------------------------------------------------------
	math::vec3 estimate_pick_tolerance(float pixelTolerance, const math::vec3& referencePosition,
									   const math::transform& objectTransform) const;

	//-----------------------------------------------------------------------------
	//  Name : look_at ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void look_at(const math::vec3& vEye, const math::vec3& vAt);

	//-----------------------------------------------------------------------------
	//  Name : look_at ()
	/// <summary>
	///
	///
	///
	/// </summary>
	//-----------------------------------------------------------------------------
	void look_at(const math::vec3& vEye, const math::vec3& vAt, const math::vec3& vUp);

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
	math::bbox get_local_bounding_box();

	//-----------------------------------------------------------------------------
	//  Name : () touch
	/// <summary>
	/// When the camera is modified.
	/// </summary>
	//-----------------------------------------------------------------------------
	void touch();

	//-----------------------------------------------------------------------------
	//  Name : () touch
	/// <summary>
	/// Get camera for one of six cube faces
	/// </summary>
	//-----------------------------------------------------------------------------
	static camera get_face_camera(std::uint32_t face, const math::transform& transform);

protected:
	//-------------------------------------------------------------------------
	// Protected Variables
	//-------------------------------------------------------------------------
	/// The type of projection currently selected for this camera.
	projection_mode projection_mode_ = projection_mode::perspective;
	/// Vertical degrees angle (perspective only).
	float fov_ = 60.0f;
	/// Near clip plane Distance
	float near_clip_ = 0.1f;
	/// Far clip plane Distance
	float far_clip_ = 1000.0f;
	/// camera's half-size when in orthographic mode.
	float ortho_size_ = 5;
	/// Viewport position
	upoint32_t viewport_pos_ = {0, 0};
	/// Viewport size
	usize32_t viewport_size_ = {0, 0};
	/// Cached view matrix
	math::transform view_;
	/// Cached projection matrix.
	mutable math::transform projection_;
	/// Cached "previous" view matrix.
	math::transform last_view_;
	/// Cached "previous" projection matrix.
	math::transform last_projection_;
	/// Details regarding the camera frustum.
	mutable math::frustum frustum_;
	/// The near clipping volume (area of space between the camera position and
	/// the near plane).
	mutable math::frustum clipping_volume_;
	/// The aspect ratio used to generate the correct horizontal degrees
	/// (perspective only)
	float aspect_ratio_ = 1.0f;
	/// Anti-aliasing data.
	math::vec4 aa_data_ = {0.0f, 0.0f, 0.0f, 0.0f};
	/// View matrix dirty ?
	bool view_dirty_ = true;
	/// Projection matrix dirty ?
	mutable bool projection_dirty_ = true;
	/// Has the aspect ratio changed?
	mutable bool aspect_dirty_ = true;
	/// Are the frustum planes dirty ?
	mutable bool frustum_dirty_ = true;
	/// Should the aspect ratio be automatically updated by the render driver?
	bool aspect_locked_ = false;
	/// Is the frustum locked?
	bool frustum_locked_ = false;
};
