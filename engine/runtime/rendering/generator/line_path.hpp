#ifndef GENERATOR_LINEPATH_HPP
#define GENERATOR_LINEPATH_HPP

#include "parametric_path.hpp"

namespace generator
{

/// A path from point to point.
/// @image html LinePath.svg
class line_path_t
{
private:
	using impl_t = parametric_path_t;
	impl_t parametric_path_;

public:
	/// @param start Start point of the line.
	/// @param end End point of the line.
	/// @param normal Line normal. Should be parallel to the line.
	/// @param segments Number of subdivisions along the line.
	line_path_t(const gml::dvec3& start = {0.0, 0.0, -1.0}, const gml::dvec3& end = {0.0, 0.0, 1.0},
				const gml::dvec3& normal = {1.0, 0.0, 0.0}, int segments = 8);

	using edges_t = typename impl_t::edges_t;

	edges_t edges() const noexcept
	{
		return parametric_path_.edges();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return parametric_path_.vertices();
	}
};
}

#endif
