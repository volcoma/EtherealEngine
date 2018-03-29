#ifndef GENERATOR_PARAMETRICSHAPE_HPP
#define GENERATOR_PARAMETRICSHAPE_HPP

#include <functional>

#include "edge.hpp"
#include "shape_vertex.hpp"

namespace generator
{

/// A shape with values evaluated using a callback function.
class parametric_shape_t
{
public:
	class edges_t
	{
	public:
		edge_t generate() const;
		bool done() const noexcept;
		void next();

	private:
		explicit edges_t(const parametric_shape_t& shape);

		const parametric_shape_t* shape_;

		int i_;

		friend class parametric_shape_t;
	};

	class vertices_t
	{
	public:
		shape_vertex_t generate() const;
		bool done() const noexcept;
		void next();

	private:
		explicit vertices_t(const parametric_shape_t& shape);

		const parametric_shape_t* shape_;

		int i_;

		friend class parametric_shape_t;
	};

	/// @param eval A callback that returns a ShapeVertex for a given value.
	/// @param segments The number of segments along the shape.
	/// Should be >= 1. Zero yields an empty shape.
	explicit parametric_shape_t(const std::function<shape_vertex_t(double)>& eval, int segments = 16) noexcept;

	edges_t edges() const noexcept;

	vertices_t vertices() const noexcept;

private:
	std::function<shape_vertex_t(double)> eval_;

	int segments_;

	double delta_;
};
}

#endif
