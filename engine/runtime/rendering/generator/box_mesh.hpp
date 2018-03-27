#ifndef GENERATOR_BOXMESH_HPP
#define GENERATOR_BOXMESH_HPP

#include "axis_swap_mesh.hpp"
#include "flip_mesh.hpp"
#include "merge_mesh.hpp"
#include "plane_mesh.hpp"
#include "translate_mesh.hpp"
#include "uv_flip_mesh.hpp"
#include "uv_swap_mesh.hpp"

namespace generator
{

namespace detail
{

class box_face_t
{
private:
	using impl_t = translate_mesh_t<plane_mesh_t>;
	impl_t translate_mesh_;

public:
	box_face_t(const gml::dvec2& size, const gml::ivec2& segments, double delta);

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return translate_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return translate_mesh_.vertices();
	}
};

class box_faces_t
{
private:
	using impl_t = merge_mesh_t<box_face_t, uv_flip_mesh_t<flip_mesh_t<box_face_t>>>;
	impl_t merge_mesh_;

public:
	box_faces_t(const gml::dvec2& size, const gml::ivec2& segments, double delta);

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return merge_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return merge_mesh_.vertices();
	}
};
}

/// Rectangular box centered at origin aligned along the x, y and z axis.
/// @image html BoxMesh.svg
class box_mesh_t
{
private:
	using impl_t = merge_mesh_t<axis_swap_mesh_t<detail::box_faces_t>,
								uv_flip_mesh_t<axis_swap_mesh_t<detail::box_faces_t>>, detail::box_faces_t>;
	impl_t merge_mesh_;

public:
	/// @param size Half of the side length in x (0), y (1) and z (2) direction.
	/// @param segments The number of segments in x (0), y (1) and z (2)
	/// directions. All should be >= 1. If any one is zero faces in that
	/// direction are not genereted. If more than one is zero the mesh is empty.
	explicit box_mesh_t(const gml::dvec3& size = {1.0, 1.0, 1.0},
						const gml::ivec3& segments = {8, 8, 8}) noexcept;

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return merge_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return merge_mesh_.vertices();
	}
};
}

#endif
