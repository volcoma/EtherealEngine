#include "rounded_rectangle_shape.hpp"

using namespace generator;

rounded_rectangle_shape_t::rounded_rectangle_shape_t(double radius, const gml::dvec2& size, int slices,
													 const gml::ivec2& segments)
	: merge_shape_{{{size[0] + radius, -size[1]}, {size[0] + radius, size[1]}, segments[1]},
				   {{radius, slices, 0.0, gml::radians(90.0)}, size},
				   {{size[0], size[1] + radius}, {-size[0], size[1] + radius}, segments[0]},
				   {{radius, slices, gml::radians(90.0), gml::radians(90.0)}, {-size[0], size[1]}},
				   {{-size[0] - radius, size[1]}, {-size[0] - radius, -size[1]}, segments[1]},
				   {{radius, slices, gml::radians(180.0), gml::radians(90.0)}, -size},
				   {{-size[0], -size[1] - radius}, {size[0], -size[1] - radius}, segments[0]},
				   {{radius, slices, gml::radians(270.0), gml::radians(90.0)}, {size[0], -size[1]}}}
{
}
