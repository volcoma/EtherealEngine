#ifndef GENERATOR_UVSWAPMESH_HPP
#define GENERATOR_UVSWAPMESH_HPP

#include "transform_mesh.hpp"

namespace generator
{

/// Swaps the texture coordinates axis u and v.
template <typename mesh_t>
class uv_swap_mesh_t
{
private:
	using impl_t = transform_mesh_t<mesh_t>;
	impl_t transform_mesh_;

public:
	/// @param mesh Source data mesh
	uv_swap_mesh_t(mesh_t mesh)
		: transform_mesh_{std::move(mesh),
						  [](mesh_vertex_t& vertex) { std::swap(vertex.tex_coord[0], vertex.tex_coord[1]); }}
	{
	}

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return transform_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return transform_mesh_.vertices();
	}
};

template <typename mesh_t>
uv_swap_mesh_t<mesh_t> uv_swap_mesh(mesh_t mesh)
{
	return uv_swap_mesh_t<mesh_t>{std::move(mesh)};
}
}

#endif
