#ifndef GENERATOR_TRANSLATOR_HPP
#define GENERATOR_TRANSLATOR_HPP

#include <functional>

#include "mesh_vertex.hpp"
#include "utils.hpp"

namespace generator
{

/// Apply a mutator function to each vertex.
template <typename mesh_t>
class transform_mesh_t
{
private:
	using impl_t = mesh_t;
	impl_t mesh_;

public:
	class vertices_t
	{
	public:
		mesh_vertex_t generate() const
		{
			auto vertex = vertices_.generate();
			mesh_->mutate_(vertex);
			return vertex;
		}

		bool done() const noexcept
		{
			return vertices_.done();
		}

		void next()
		{
			vertices_.next();
		}

	private:
		const transform_mesh_t* mesh_;

		typename vertex_generator_type<mesh_t>::type vertices_;

		explicit vertices_t(const transform_mesh_t& mesh)
			: mesh_{&mesh}
			, vertices_{mesh.mesh_.vertices()}
		{
		}

		friend class transform_mesh_t;
	};

	/// @param mesh Source data mesh.
	/// @param mutate Callback function that gets called once per vertex.
	explicit transform_mesh_t(mesh_t mesh, std::function<void(mesh_vertex_t&)> mutate)
		: mesh_{std::move(mesh)}
		, mutate_{std::move(mutate)}
	{
	}

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return mesh_.triangles();
	}

	vertices_t vertices() const noexcept
	{
		return vertices_t{*this};
	}

private:
	std::function<void(mesh_vertex_t&)> mutate_;
};

template <typename mesh_t>
transform_mesh_t<mesh_t> transform_mesh(mesh_t mesh, std::function<void(mesh_vertex_t&)> mutate)
{
	return transform_mesh_t<mesh_t>{std::move(mesh), std::move(mutate)};
}
}

#endif
