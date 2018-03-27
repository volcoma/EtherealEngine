#ifndef GENERATOR_TORUSKNOTPATH_HPP
#define GENERATOR_TORUSKNOTPATH_HPP

#include "circle_shape.hpp"
#include "extrude_mesh.hpp"
#include "parametric_path.hpp"

namespace generator
{

/// A circle winding multiple times around.
/// @image html KnotPath.svg
class knot_path_t
{
private:
	using impl_t = parametric_path_t;
	impl_t parametric_path_;

public:
	/// @param q Times around a circle
	/// @param p Times around z axis
	/// @param segments Number of subdivisions along the path.
	knot_path_t(int p = 2, int q = 3, int segments = 96);

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
