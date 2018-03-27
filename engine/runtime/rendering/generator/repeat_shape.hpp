#ifndef GENERATOR_REPEATSHAPE_HPP
#define GENERATOR_REPEATSHAPE_HPP

#include "edge.hpp"
#include "shape_vertex.hpp"
#include "utils.hpp"

namespace generator
{

/**
 * Repeats the same shape a given number of time at given intervals.
 */
template <typename shape_t>
class repeat_shape_t
{
public:
	class edges_t
	{
	public:
		bool done() const noexcept
		{
			return index_ >= repeat_shape_->instances_;
		}

		edge_t generate() const
		{
			edge_t temp = edges_.generate();
			temp.vertices += delta_;
			return temp;
		}

		void next() noexcept
		{
			edges_.next();

			if(edges_.done())
			{
				++index_;
				delta_ += repeat_shape_->vertex_count_;
				edges_ = repeat_shape_->shape_.edges();
			}
		}

	private:
		const repeat_shape_t* repeat_shape_;

		typename edge_generator_type<shape_t>::type edges_;

		int index_;

		int delta_;

		explicit edges_t(const repeat_shape_t* repeatShape) noexcept
			: repeat_shape_{repeatShape}
			, edges_{repeatShape->shape_.edges()}
			, index_{repeatShape->vertex_count_ > 0 ? 0 : repeatShape->instances_}
			, delta_{0}
		{
		}

		int countEdges() const noexcept
		{
			if(repeat_shape_->instances_ < 1)
				return 0;

			return count(repeat_shape_->shape_.edges()) * (repeat_shape_->instances_ - index_ - 1) +
				   count(edges_);
		}

		friend int count(const edges_t& generator) noexcept
		{
			return generator.countEdges();
		}

		friend class repeat_shape_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept
		{
			return index_ >= repeat_shape_->instances_;
		}

		shape_vertex_t generate() const
		{
			shape_vertex_t temp = vertices_.generate();
			temp.position += delta_;
			return temp;
		}

		void next()
		{
			vertices_.next();

			if(vertices_.done())
			{
				++index_;
				delta_ += repeat_shape_->delta_;
				vertices_ = repeat_shape_->shape_.vertices();
			}
		}

	private:
		explicit vertices_t(const repeat_shape_t* repeatShape)
			: repeat_shape_{repeatShape}
			, vertices_{repeatShape->shape_.vertices()}
			, index_{repeatShape->vertex_count_ > 0 ? 0 : repeatShape->instances_}
			, delta_{}
		{
		}

		const repeat_shape_t* repeat_shape_;

		typename vertex_generator_type<shape_t>::type vertices_;

		int index_;

		gml::dvec2 delta_;

		int countvertices_t() const noexcept
		{
			if(repeat_shape_->instances_ < 1)
				return 0;

			return repeat_shape_->vertex_count_ * (repeat_shape_->instances_ - index_ - 1) + count(vertices_);
		}

		friend int count(const vertices_t& generator) noexcept
		{
			return generator.countvertices_t();
		}

		friend class repeat_shape_t;
	};

	/// @param shape The shape to repeat.
	/// @param instances Number of times to repeat. If <1 an empty shape results.
	/// @param delta An offset aplied to each copy.
	explicit repeat_shape_t(shape_t shape, int instances, const gml::dvec2& delta) noexcept
		: shape_{std::move(shape)}
		, instances_{instances}
		, delta_{delta}
		, vertex_count_{count(shape_.vertices())}
	{
	}

	edges_t edges() const noexcept
	{
		return edges_t{this};
	}

	vertices_t vertices() const noexcept
	{
		return vertices_t{this};
	}

private:
	shape_t shape_;

	int instances_;

	gml::dvec2 delta_;

	int vertex_count_;
};

template <typename shape_t>
repeat_shape_t<shape_t> repeat_shape(shape_t shape, int instances, const gml::dvec2& delta) noexcept
{
	return repeat_shape_t<shape_t>{std::move(shape), instances, delta};
}
}

#endif
