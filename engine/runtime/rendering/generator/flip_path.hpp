#ifndef GENERATOR_FLIPPATH_HPP
#define GENERATOR_FLIPPATH_HPP

#include "edge.hpp"
#include "transform_path.hpp"

namespace generator
{

/// Flips mesh inside out. Reverses triangles and normals.
template <typename path_t>
class flip_path_t
{
private:
	using impl_t = transform_path_t<path_t>;
	impl_t transform_path_;

public:
	class edges_t
	{
	public:
		edge_t generate() const
		{
			edge_t edge = edges_.generate();
			std::swap(edge.vertices[0], edge.vertices[1]);
			return edge;
		}

		bool done() const noexcept
		{
			return edges_.done();
		}

		void next()
		{
			edges_.next();
		}

	private:
		typename edge_generator_type<transform_path_t<path_t>>::type edges_;

		edges_t(const transform_path_t<path_t>& path)
			: edges_{path.edges()}
		{
		}

		friend class flip_path_t;
	};

	/// @param path Source data path.
	flip_path_t(path_t path)
		: transform_path_{std::move(path), [](path_vertex_t& vertex) {
							  vertex.tangent *= -1.0;
							  vertex.normal *= -1.0;
						  }}
	{
	}

	edges_t edges() const noexcept
	{
		return {*this};
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return transform_path_.vertices();
	}
};

template <typename path_t>
flip_path_t<path_t> flip_path(path_t path)
{
	return flip_path_t<path_t>{std::move(path)};
}
}

#endif
