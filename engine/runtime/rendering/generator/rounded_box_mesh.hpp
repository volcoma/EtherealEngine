#ifndef GENERATOR_ROUNDEDBOXMESH_HPP
#define GENERATOR_ROUNDEDBOXMESH_HPP

#include "axis_flip_mesh.hpp"
#include "box_mesh.hpp"
#include "cylinder_mesh.hpp"
#include "merge_mesh.hpp"
#include "mirror_mesh.hpp"
#include "plane_mesh.hpp"
#include "rotate_mesh.hpp"
#include "spherical_triangle_mesh.hpp"
#include "translate_mesh.hpp"

namespace generator
{

namespace detail
{

class box_edge_t
{
private:
	using impl_t = translate_mesh_t<cylinder_mesh_t>;
	impl_t translate_mesh_;

public:
	box_edge_t(const gml::dvec2& position, double radius, double size, int slices, int segments);

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

class box_edges_t
{
private:
	using impl_t = mirror_mesh_t<mirror_mesh_t<box_edge_t>>;
	impl_t mirrorMesh_;

public:
	box_edges_t(const gml::dvec3& size, double radius, int slices, int segments);

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return mirrorMesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return mirrorMesh_.vertices();
	}
};

class box_corner_t
{
private:
	using impl_t = translate_mesh_t<spherical_triangle_mesh_t>;
	impl_t translate_mesh_;

public:
	box_corner_t(const gml::dvec3& position, double radius, int slices);

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

class box_corners_t
{
private:
	using impl_t = mirror_mesh_t<mirror_mesh_t<mirror_mesh_t<box_corner_t>>>;
	impl_t mirrorMesh_;

public:
	box_corners_t(const gml::dvec3& size, double radius, int slices);

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return mirrorMesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return mirrorMesh_.vertices();
	}
};
}

/// Rectangular box with rounded edges centered at origin aligned along the x, y
/// and z axis.
/// @image html RoundedBoxMesh.svg
class rounded_box_mesh_t
{
private:
	using impl_t = merge_mesh_t<axis_swap_mesh_t<detail::box_faces_t>,
								uv_flip_mesh_t<axis_swap_mesh_t<detail::box_faces_t>>, detail::box_faces_t,
								axis_swap_mesh_t<detail::box_edges_t>, axis_swap_mesh_t<detail::box_edges_t>,
								detail::box_edges_t, detail::box_corners_t>;
	impl_t merge_mesh_;

public:
	/// @param radius Radius of the rounded edges.
	/// @param size Half of the side length in x (0), y (1) and z (2) direction.
	/// @param slices Number subdivions around in the rounded edges.
	/// @param segments Number of subdivisons in x (0), y (1) and z (2)
	/// direction for the flat faces.
	rounded_box_mesh_t(double radius = 0.25, const gml::dvec3& size = {0.75, 0.75, 0.75}, int slices = 4,
					   const gml::ivec3& segments = {8, 8, 8});

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
