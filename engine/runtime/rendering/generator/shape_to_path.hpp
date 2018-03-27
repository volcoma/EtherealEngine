#ifndef GENERATOR_SHAPETOPATH_HPP
#define GENERATOR_SHAPETOPATH_HPP

#include "path_vertex.hpp"
#include "shape_vertex.hpp"
#include "utils.hpp"

namespace generator
{

/// Converts a shape_t to a Path.
/// The shape position coordinates are used as the x and y and 0 is used as z.
/// shape_t tangent becomes the path tangent and z-axis becomes normal vector and
/// thus the shape normal becomes the path binormal.
template <typename shape_t>
class shape_to_path_t
{
private:
	using impl_t = shape_t;
	impl_t shape_;

public:
	class vertices_t
	{
	public:
		path_vertex_t generate() const
		{

			shape_vertex_t shapeVertex = vertices_.generate();

			path_vertex_t vertex;

			vertex.position = gml::dvec3(shapeVertex.position, 0.0);

			vertex.tangent = gml::dvec3(shapeVertex.tangent, 0.0);
			vertex.normal = gml::dvec3{0.0, 0.0, 1.0};

			vertex.tex_coord = shapeVertex.tex_coord;

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
		typename vertex_generator_type<shape_t>::type vertices_;

		vertices_t(const shape_t& shape)
			: vertices_{shape.vertices()}
		{
		}

		friend class shape_to_path_t;
	};

	shape_to_path_t(shape_t shape)
		: shape_{std::move(shape)}
	{
	}

	using edges_t = typename impl_t::edges_t;

	edges_t edges() const noexcept
	{
		return shape_.edges();
	}

	vertices_t vertices() const
	{
		return *this;
	}
};

template <typename shape_t>
shape_to_path_t<shape_t> shape_to_path(shape_t shape)
{
	return shape_to_path_t<shape_t>{std::move(shape)};
}
}

#endif
