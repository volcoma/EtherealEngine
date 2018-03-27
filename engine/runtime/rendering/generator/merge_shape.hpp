#ifndef GENERATOR_MERGESHAPE_HPP
#define GENERATOR_MERGESHAPE_HPP

#include "empty_shape.hpp"
#include "shape_vertex.hpp"
#include "utils.hpp"

namespace generator
{

/// Merges (concatenates) multiple shapes together.
template <typename... shape_t>
class merge_shape_t; // undefined

template <>
class merge_shape_t<> : public empty_shape_t
{
};

template <typename Head, typename... Tail>
class merge_shape_t<Head, Tail...>
{
public:
	class edges_t
	{
	public:
		edge_t generate() const
		{
			if(!head_.done())
				return head_.generate();

			edge_t edge = tail_.generate();
			edge.vertices += head_vertex_count_;
			return edge;
		}

		bool done() const noexcept
		{
			return all_done_;
		}

		void next()
		{
			if(!head_.done())
				head_.next();
			else
				tail_.next();

			all_done_ = tail_.done() && head_.done();
		}

	private:
		typename edge_generator_type<Head>::type head_;
		typename edge_generator_type<merge_shape_t<Tail...>>::type tail_;

		int head_vertex_count_;

		bool all_done_;

		edges_t(const merge_shape_t& shape)
			: head_{shape.head_.edges()}
			, tail_(shape.tail_.edges())
			, head_vertex_count_{count(shape.head_.vertices())}
			, all_done_{tail_.done() && head_.done()}
		{
		}

		friend class merge_shape_t<Head, Tail...>;
	};

	class vertices_t
	{
	public:
		shape_vertex_t generate() const
		{
			if(!head_.done())
				return head_.generate();
			return tail_.generate();
		}

		bool done() const noexcept
		{
			return all_done_;
		}

		void next()
		{
			if(!head_.done())
				head_.next();
			else
				tail_.next();

			all_done_ = tail_.done() && head_.done();
		}

	private:
		typename vertex_generator_type<Head>::type head_;
		typename vertex_generator_type<merge_shape_t<Tail...>>::type tail_;
		bool all_done_;

		vertices_t(const merge_shape_t& shape)
			: head_{shape.head_.vertices()}
			, tail_(shape.tail_.vertices())
			, all_done_{tail_.done() && head_.done()}
		{
		}

		friend class merge_shape_t<Head, Tail...>;
	};

	merge_shape_t(Head head, Tail... tail)
		: head_{head}
		, tail_{tail...}
	{
	}

	edges_t edges() const noexcept
	{
		return *this;
	}

	vertices_t vertices() const noexcept
	{
		return *this;
	}

private:
	Head head_;
	merge_shape_t<Tail...> tail_;
};

template <typename... shape_t>
merge_shape_t<shape_t...> merge_shape(shape_t... shapes)
{
	return merge_shape_t<shape_t...>{std::move(shapes)...};
}
}

#endif
