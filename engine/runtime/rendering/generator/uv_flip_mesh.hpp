#ifndef GENERATOR_UVFLIPMESH_HPP
#define GENERATOR_UVFLIPMESH_HPP

#include "transform_mesh.hpp"

namespace generator
{

/// Flips texture coordinate axis directions.
template <typename mesh_t>
class uv_flip_mesh_t
{
private:
	using impl_t = transform_mesh_t<mesh_t>;
	impl_t transform_mesh_;

public:
	/// @param mesh Source data mesh.
	/// @param u Flip u
	/// @param v Flip v
	uv_flip_mesh_t(mesh_t mesh, bool u, bool v)
		: transform_mesh_{std::move(mesh), [u, v](mesh_vertex_t& vertex) {
							  if(u)
								  vertex.tex_coord[0] = 1.0 - vertex.tex_coord[0];
							  if(v)
								  vertex.tex_coord[1] = 1.0 - vertex.tex_coord[1];
						  }}
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
uv_flip_mesh_t<mesh_t> uv_flip_mesh(mesh_t mesh)
{
	return uv_flip_mesh_t<mesh_t>{std::move(mesh)};
}
}

#endif
