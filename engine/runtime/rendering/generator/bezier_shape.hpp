#ifndef GENERATOR_BEZIER_SHAPE_HPP
#define GENERATOR_BEZIER_SHAPE_HPP

#include <algorithm>
#include <limits>

#include "parametric_shape.hpp"

namespace generator
{

/// A bezier curve with D control points.
/// @tparam D Number of control points. 4 = cubic curve. Must be > 1.
/// @image html BezierShape.svg
template <int D>
class bezier_shape_t
{
private:
	static_assert(D > 1, "D must be > 1.");

	using impl_t = parametric_shape_t;
	impl_t parametric_shape_;

	struct array_wrapper
	{
		gml::dvec2 data[D];

		array_wrapper(const gml::dvec2 (&p)[D])
		{
			std::copy(&p[0], &p[0] + D, &data[0]);
		}
	};

	explicit bezier_shape_t(const array_wrapper& p, int segments)
		: parametric_shape_{[p](double t) {
								shape_vertex_t vertex;

								vertex.position = gml::bezier(p.data, t);

								vertex.tangent = gml::bezierDerivative<1>(p.data, t);

								// If tangent is zero try again near by.
								const double e = std::numeric_limits<double>::epsilon();
								if(gml::dot(vertex.tangent, vertex.tangent) < e)
								{
									vertex.tangent = gml::bezierDerivative<1>(p.data, t + 10.0 * e);
								}

								vertex.tangent = gml::normalize(vertex.tangent);

								vertex.tex_coord = t;

								return vertex;
							},
							segments}
	{
	}

public:
	/// @param p Control points
	/// @param segments Number of subdivisions
	explicit bezier_shape_t(const gml::dvec2 (&p)[D], int segments = 16)
		: // Work around a msvc lambda capture bug by wrapping the array.
		bezier_shape_t{array_wrapper{p}, segments}
	{
	}

	using edges_t = typename impl_t::edges_t;

	edges_t edges() const noexcept
	{
		return parametric_shape_.edges();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return parametric_shape_.vertices();
	}
};
}

#endif
