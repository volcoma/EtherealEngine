#ifndef GENERATOR_EDGE_HPP
#define GENERATOR_EDGE_HPP

#include "math.hpp"

namespace generator
{

class edge_t
{
public:
	gml::ivec2 vertices;

	edge_t() noexcept
		: vertices{}
	{
	}

	explicit edge_t(const gml::ivec2& vertices) noexcept
		: vertices{vertices}
	{
	}
};

}

#endif
