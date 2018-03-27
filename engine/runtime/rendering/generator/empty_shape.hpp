#ifndef GENERATOR_EMPTYSHAPE_HPP
#define GENERATOR_EMPTYSHAPE_HPP

#include "edge.hpp"
#include "shape_vertex.hpp"

namespace generator
{

/// Empty shape with zero vertices and edges.
class empty_shape_t
{
public:
	class edges_t
	{
	public:
		edge_t generate() const;
		bool done() const noexcept;
		void next();

	private:
		edges_t();
		friend class empty_shape_t;
	};

	class vertices_t
	{
	public:
		shape_vertex_t generate() const;
		bool done() const noexcept;
		void next();

	private:
		vertices_t();
		friend class empty_shape_t;
	};

	edges_t edges() const noexcept;

	vertices_t vertices() const noexcept;
};
}

#endif
