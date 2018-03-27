#ifndef GENERATOR_MIRRORMESH_HPP
#define GENERATOR_MIRRORMESH_HPP

#include "axis.hpp"
#include "axis_flip_mesh.hpp"
#include "merge_mesh.hpp"

namespace generator
{

/// Duplicates the mesh by mirrorring it along an axis.
template <typename Mesh>
class mirror_mesh_t
{
private:
	using impl_t = merge_mesh_t<Mesh, axis_flip_mesh_t<Mesh>>;
	impl_t merge_mesh_;

public:
	/// @param mesh Source data mesh.
	/// @param axis The axis to mirror along.
	mirror_mesh_t(Mesh mesh, axis_t axis)
		: merge_mesh_{mesh, {mesh, axis == axis_t::X, axis == axis_t::Y, axis == axis_t::Z}}
	{
	}

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

template <typename Mesh>
mirror_mesh_t<Mesh> mirror_mesh(Mesh mesh)
{
	return mirror_mesh_t<Mesh>{std::move(mesh)};
}
}

#endif
