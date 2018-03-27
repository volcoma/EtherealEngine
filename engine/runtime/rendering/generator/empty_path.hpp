#ifndef GENERATOR_EMPTYPATH_HPP
#define GENERATOR_EMPTYPATH_HPP

#include "edge.hpp"
#include "iterator.hpp"
#include "path_vertex.hpp"

namespace generator
{

/// Empty path with zero vertices and edges.
class empty_path_t
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

		friend class empty_path_t;
	};

	class vertices_t
	{
	public:
		path_vertex_t generate() const;
		bool done() const noexcept;
		void next();

	private:
		vertices_t();

		friend class empty_path_t;
	};

	empty_path_t();

	edges_t edges() const noexcept;

	vertices_t vertices() const noexcept;
};
}

#endif
