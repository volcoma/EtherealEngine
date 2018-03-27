#ifndef GENERATOR_FLIPMESH_HPP
#define GENERATOR_FLIPMESH_HPP

#include "transform_mesh.hpp"
#include "triangle.hpp"

namespace generator
{

/// Flips mesh inside out. Reverses triangles and normals.
template <typename mesh_t>
class flip_mesh_t
{
private:
	using impl_t = transform_mesh_t<mesh_t>;
	impl_t transform_mesh_;

public:
	class triangles_t
	{
	public:
		triangle_t generate() const
		{
			triangle_t triangle = triangles_.generate();
			std::swap(triangle.vertices[0], triangle.vertices[2]);
			return triangle;
		}

		bool done() const noexcept
		{
			return triangles_.done();
		}

		void next()
		{
			triangles_.next();
		}

	private:
		typename triangle_generator_type<transform_mesh_t<mesh_t>>::type triangles_;

		triangles_t(const transform_mesh_t<mesh_t>& mesh)
			: triangles_{mesh.triangles()}
		{
		}

		friend class flip_mesh_t;
	};

	/// @param mesh Source data mesh.
	flip_mesh_t(mesh_t mesh)
		: transform_mesh_{std::move(mesh), [](mesh_vertex_t& vertex) { vertex.normal *= -1.0; }}
	{
	}

	triangles_t triangles() const noexcept
	{
		return this->transform_mesh_;
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return transform_mesh_.vertices();
	}
};

template <typename mesh_t>
flip_mesh_t<mesh_t> flip_mesh(mesh_t mesh)
{
	return flip_mesh_t<mesh_t>{std::move(mesh)};
}
}

#endif
