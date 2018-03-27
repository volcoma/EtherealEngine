#ifndef GENERATOR_SCALEMESH_HPP
#define GENERATOR_SCALEMESH_HPP

#include "transform_mesh.hpp"

namespace generator
{

/// Scales a mesh (keeps normals unit length)
template <typename mesh_t>
class scale_mesh_t
{
private:
	using impl_t = transform_mesh_t<mesh_t>;
	impl_t transform_mesh_;

public:
	/// @param mesh Source data mesh.
	/// @param scale Scale factors. Must not be zero!
	scale_mesh_t(mesh_t mesh, const gml::dvec3& scale)
		: transform_mesh_{std::move(mesh), [scale](mesh_vertex_t& value) {
							  value.position *= scale;
							  value.normal = normalize(scale * value.normal);
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
scale_mesh_t<mesh_t> scale_mesh(mesh_t mesh, const gml::dvec3& delta)
{
	return scale_mesh_t<mesh_t>{std::move(mesh), delta};
}
}

#endif
