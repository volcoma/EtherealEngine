#ifndef GENERATOR_AXISSWAPMESH_HPP
#define GENERATOR_AXISSWAPMESH_HPP

#include "axis.hpp"
#include "transform_mesh.hpp"
#include "triangle.hpp"

namespace generator
{

/// Swaps any number of axis in the mesh.
template <typename mesh_t>
class axis_swap_mesh_t
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
			if(mesh_->flip_)
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
		const axis_swap_mesh_t* mesh_;

		typename triangle_generator_type<transform_mesh_t<mesh_t>>::type triangles_;

		explicit triangles_t(const axis_swap_mesh_t& mesh)
			: mesh_{&mesh}
			, triangles_{mesh.transform_mesh_.triangles()}
		{
		}

		friend class axis_swap_mesh_t;
	};

	///@param mesh Source data mesh.
	///@param x Axis to be used as the x-axis
	///@param y Axis to be used as the y-axis
	///@param z Axis to be used as the z-axis
	axis_swap_mesh_t(mesh_t mesh, axis_t x, axis_t y, axis_t z)
		: transform_mesh_{std::move(mesh),
						  [x, y, z](mesh_vertex_t& vertex) {
							  vertex.position = gml::dvec3{vertex.position[static_cast<int>(x)],
														   vertex.position[static_cast<int>(y)],
														   vertex.position[static_cast<int>(z)]};
							  vertex.normal = gml::dvec3{vertex.normal[static_cast<int>(x)],
														 vertex.normal[static_cast<int>(y)],
														 vertex.normal[static_cast<int>(z)]};
						  }}
		, flip_{true}
	{
		if(x != axis_t::X)
			flip_ = !flip_;
		if(y != axis_t::Y)
			flip_ = !flip_;
		if(z != axis_t::Z)
			flip_ = !flip_;
	}

	triangles_t triangles() const noexcept
	{
        return triangles_t{*this};
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return transform_mesh_.vertices();
	}

private:
	bool flip_;
};

template <typename mesh_t>
axis_swap_mesh_t<mesh_t> axis_swap_mesh(mesh_t mesh, axis_t x, axis_t y, axis_t z)
{
	return axis_swap_mesh_t<mesh_t>{std::move(mesh), x, y, z};
}
}

#endif
