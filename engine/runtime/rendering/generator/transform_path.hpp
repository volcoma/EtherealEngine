#ifndef GENERATOR_TRANSFORMPATH_HPP
#define GENERATOR_TRANSFORMPATH_HPP

#include <functional>

#include "path_vertex.hpp"
#include "utils.hpp"

namespace generator
{

/// Apply a mutator function to each vertex.
template <typename path_t>
class transform_path_t
{
private:
	using impl_t = path_t;
	impl_t path_;

public:
	class vertices_t
	{
	public:
		path_vertex_t generate() const
		{
			auto vertex = vertices_.generate();
			path_->mutate_(vertex);
			return vertex;
		}

		bool done() const noexcept
		{
			return vertices_.done();
		}

		void next()
		{
			vertices_.next();
		}

	private:
		vertices_t(const transform_path_t& path)
			: path_{&path}
			, vertices_{path.path_.vertices()}
		{
		}

		const transform_path_t* path_;

		typename vertex_generator_type<path_t>::type vertices_;

		friend class transform_path_t;
	};

	/// @param path Source data path.
	/// @param mutate Callback function that gets called once per vertex.
	transform_path_t(path_t path, std::function<void(path_vertex_t&)> mutate)
		: path_{std::move(path)}
		, mutate_{mutate}
	{
	}

	vertices_t vertices() const noexcept
	{
		return *this;
	}

	using edges_t = typename impl_t::edges_t;

	edges_t edges() const noexcept
	{
		return path_.edges();
	}

private:
	std::function<void(path_vertex_t&)> mutate_;
};

template <typename path_t>
transform_path_t<path_t> transform_path(path_t path, std::function<void(path_vertex_t&)> mutate)
{
	return transform_path_t<path_t>{std::move(path), std::move(mutate)};
}
}

#endif
