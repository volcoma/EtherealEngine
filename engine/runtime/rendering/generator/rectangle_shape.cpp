#include "rectangle_shape.hpp"

using namespace generator;

rectangle_shape_t::rectangle_shape_t(const gml::dvec2& size, const gml::ivec2& segments)
	: merge_shape_{{{size[0], -size[1]}, size, segments[1]},
				   {size, {-size[0], size[1]}, segments[0]},
				   {{-size[0], size[1]}, -size, segments[1]},
				   {-size, {size[0], -size[1]}, segments[0]}}
{
}
