#ifndef GENERATOR_HELIXPATH_HPP
#define GENERATOR_HELIXPATH_HPP

#include "parametric_path.hpp"

namespace generator
{

/// A helix cented at origin aligned along the z-axis.
/// @image html HelixPath.svg
class helix_path_t
{
private:
	using impl_t = parametric_path_t;
	impl_t parametric_path_;

public:
	/// @param radius Radius from the z-axis
	/// @param size Half of the length along the z-axis.
	/// @param segments Number of subdivisions along the path.
	/// @param start Counterclockwise angle around the z-axis relative to the x-axis.
	/// @param sweep Counterclockwise angle around the z-axis.
	helix_path_t(double radius = 1.0, double size = 1.0, int segments = 32, double start = 0.0,
				 double sweep = gml::radians(720.0));

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
