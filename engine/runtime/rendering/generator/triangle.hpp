#ifndef GENERATOR_TRIANGLE_HPP
#define GENERATOR_TRIANGLE_HPP

#include "math.hpp"

namespace generator
{

class triangle_t
{
public:
	/// Zero based indices of the triangle vertices in counterclockwise order.
	gml::ivec3 vertices;

	triangle_t() noexcept
		: vertices{}
	{
	}

	explicit triangle_t(const gml::ivec3& vertices) noexcept
		: vertices{vertices}
	{
	}
};
}

#endif
