#ifndef GENERATOR_TRANSLATEPATH_HPP
#define GENERATOR_TRANSLATEPATH_HPP

#include "math.hpp"

#include "transform_path.hpp"

namespace generator
{

/// Translates the position of each vertex by given amount.
template <typename path_t>
class translate_path_t
{
private:
	using impl_t = transform_path_t<path_t>;
	impl_t transform_path_;

public:
	/// @param path Source data path.
	/// @param delta Amount to increment each vertex position.
	translate_path_t(path_t path, const gml::dvec3& delta)
		: transform_path_{std::move(path), [delta](path_vertex_t& value) { value.position += delta; }}
	{
	}

	using edges_t = typename impl_t::edges_t;

	edges_t edges() const noexcept
	{
		return transform_path_.edges();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return transform_path_.vertices();
	}
};

template <typename path_t>
translate_path_t<path_t> translate_path(path_t path, const gml::dvec3& delta)
{
	return translate_path_t<path_t>{std::move(path), delta};
}
}

#endif
