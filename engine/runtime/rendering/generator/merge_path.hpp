#ifndef GENERATOR_MERGEPath_HPP
#define GENERATOR_MERGEPath_HPP

#include "empty_path.hpp"
#include "path_vertex.hpp"
#include "utils.hpp"

namespace generator
{

/// Merges (concatenates) multiple paths together.
template <typename... Path>
class merge_path_t; // undefined

template <>
class merge_path_t<> : public empty_path_t
{
};

template <typename Head, typename... Tail>
class merge_path_t<Head, Tail...>
{
public:
	class edges_t
	{
	public:
		edge_t generate() const
		{
			if(!head_.done())
				return head_.generate();
			return tail_.generate() + head_vertex_count_;
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
		typename edge_generator_type<merge_path_t<Tail...>>::type tail_;

		int head_vertex_count_;

		bool all_done_;

		explicit edges_t(const merge_path_t& path)
			: head_{path.head_.triangles()}
			, tail_(path.tail_.triangles())
			, head_vertex_count_{count(path.head_.vertices())}
			, all_done_{tail_.done() && head_.done()}
		{
		}

		friend class merge_path_t<Head, Tail...>;
	};

	class vertices_t
	{
	public:
		path_vertex_t generate() const
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
		typename vertex_generator_type<merge_path_t<Tail...>>::type tail_;
		bool all_done_;

		explicit vertices_t(const merge_path_t& path)
			: head_{path.head_.vertices()}
			, tail_(path.tail_.vertices())
			, all_done_{tail_.done() && head_.done()}
		{
		}

		friend class merge_path_t<Head, Tail...>;
	};

	merge_path_t(Head head, Tail... tail)
		: head_{head}
		, tail_{tail...}
	{
	}

	edges_t edges() const noexcept
	{
        return edges_t{*this};
	}

	vertices_t vertices() const noexcept
	{
        return vertices_t{*this};
	}

private:
	Head head_;
	merge_path_t<Tail...> tail_;
};

template <typename... Path>
merge_path_t<Path...> merge_path(Path... paths)
{
	return merge_path_t<Path...>{std::move(paths)...};
}
}

#endif
