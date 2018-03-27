#ifndef GENERATOR_REPEATPATH_HPP
#define GENERATOR_REPEATPATH_HPP

#include "edge.hpp"
#include "path_vertex.hpp"
#include "utils.hpp"

namespace generator
{

/**
 * Repeats the same path a given number of time at given intervals.
 */
template <typename path_t>
class repeat_path_t
{
public:
	class edges_t
	{
	public:
		bool done() const noexcept
		{
			return index_ >= repeat_path_->instances_;
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
				delta_ += repeat_path_->vertex_count_;
				edges_ = repeat_path_->path_.edges();
			}
		}

	private:
		const repeat_path_t* repeat_path_;

		typename edge_generator_type<path_t>::type edges_;

		int index_;

		int delta_;

		explicit edges_t(const repeat_path_t* repeatPath) noexcept
			: repeat_path_{repeatPath}
			, edges_{repeatPath->path_.edges()}
			, index_{repeatPath->vertex_count_ > 0 ? 0 : repeatPath->instances_}
			, delta_{0}
		{
		}

		friend class repeat_path_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept
		{
			return index_ >= repeat_path_->instances_;
		}

		path_vertex_t generate() const
		{
			path_vertex_t temp = vertices_.generate();
			temp.position += delta_;
			return temp;
		}

		void next()
		{
			vertices_.next();

			if(vertices_.done())
			{
				++index_;
				delta_ += repeat_path_->delta_;
				vertices_ = repeat_path_->path_.vertices();
			}
		}

	private:
		explicit vertices_t(const repeat_path_t* repeatPath)
			: repeat_path_{repeatPath}
			, vertices_{repeatPath->path_.vertices()}
			, index_{repeatPath->vertex_count_ > 0 ? 0 : repeatPath->instances_}
			, delta_{}
		{
		}

		const repeat_path_t* repeat_path_;

		typename vertex_generator_type<path_t>::type vertices_;

		int index_;

		gml::dvec3 delta_;

		int countvertices_t() const noexcept
		{
			if(repeat_path_->instances_ < 1)
				return 0;

			return repeat_path_->vertex_count_ * (repeat_path_->instances_ - index_ - 1) + count(vertices_);
		}

		friend int count(const vertices_t& generator) noexcept
		{
			return generator.countvertices_t();
		}

		friend class repeat_path_t;
	};

	/// @param path The path to repeat.
	/// @param instances Number of times to repeat. If <1 an empty path results.
	/// @param delta An offset aplied to each copy.
	explicit repeat_path_t(path_t path, int instances, const gml::dvec3& delta) noexcept
		: path_{std::move(path)}
		, instances_{instances}
		, delta_{delta}
		, vertex_count_{count(path_.vertices())}
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
	path_t path_;

	int instances_;

	gml::dvec3 delta_;

	int vertex_count_;
};

template <typename path_t>
repeat_path_t<path_t> repeat_path(path_t path, int instances, const gml::dvec3& delta) noexcept
{
	return repeat_path_t<path_t>{std::move(path), instances, delta};
}
}

#endif
