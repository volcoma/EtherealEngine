#include "grid_shape.hpp"
#include <algorithm>
using namespace generator;

grid_shape_t::grid_shape_t(const gml::dvec2& size, const gml::ivec2& segments,
						   const gml::ivec2& subSegments) noexcept
	: mImpl{
		  // Horizontal lines
		  repeat_shape_t<line_shape_t>{line_shape_t{gml::dvec2{-size[0], -size[1]},
													gml::dvec2{size[0], -size[1]},
													segments[0] * subSegments[0]},
									   segments[1] < 1 ? 0 : segments[1] + 1,
									   gml::dvec2{0.0, 2.0 * size[1] / std::max(segments[1], 1)}},
		  // Vertical lines
		  repeat_shape_t<line_shape_t>{line_shape_t{gml::dvec2{-size[0], -size[1]},
													gml::dvec2{-size[0], size[1]},
													segments[1] * subSegments[1]},
									   segments[0] < 1 ? 0 : segments[0] + 1,
									   gml::dvec2{2.0 * size[0] / std::max(segments[0], 1), 0.0}},
	  }
{
	//
}

grid_shape_t::edges_t grid_shape_t::edges() const noexcept
{
	return mImpl.edges();
}

grid_shape_t::vertices_t grid_shape_t::vertices() const noexcept
{
	return mImpl.vertices();
}
