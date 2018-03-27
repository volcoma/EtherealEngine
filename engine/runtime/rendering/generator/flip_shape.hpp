#ifndef GENERATOR_FLIPSHAPE_HPP
#define GENERATOR_FLIPSHAPE_HPP

#include "edge.hpp"
#include "transform_shape.hpp"

namespace generator
{

/// Flips shape direction. Reverses edges and tangents.
template <typename shape_t>
class flip_shape_t
{
private:
	using impl_t = transform_shape_t<shape_t>;
	impl_t transform_shape_;

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
		typename edge_generator_type<transform_shape_t<shape_t>>::type edges_;

		edges_t(const transform_shape_t<shape_t>& shape)
			: edges_{shape.edges()}
		{
		}

		friend class flip_shape_t;
	};

	/// @param shape Source data shape.
	flip_shape_t(shape_t shape)
		: transform_shape_{std::move(shape), [](shape_vertex_t& vertex) { vertex.tangent *= -1.0; }}
	{
	}

	edges_t edges() const noexcept
	{
		return {*this};
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return transform_shape_.vertices();
	}
};

template <typename shape_t>
flip_shape_t<shape_t> flip_shape(shape_t shape)
{
	return flip_shape_t<shape_t>{std::move(shape)};
}
}

#endif
