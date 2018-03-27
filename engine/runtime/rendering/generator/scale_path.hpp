#ifndef GENERATOR_SCALEPATH_HPP
#define GENERATOR_SCALEPATH_HPP

#include "transform_path.hpp"

namespace generator
{

/// Scales a path. Keeps tangents and normals unit length.
template <typename path_t>
class scale_path_t
{
private:
	using impl_t = transform_path_t<path_t>;
	impl_t transform_path_;

public:
	/// @param path Source data path.
	/// @param scale Scale Factor. Must not have zero components!
	scale_path_t(path_t path, const gml::dvec3& scale)
		: transform_path_{std::move(path), [scale](path_vertex_t& value) {
							  value.position *= scale;
							  value.tangent = gml::normalize(scale * value.tangent);
							  value.normal = gml::normalize(scale * value.normal);
						  }}
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
scale_path_t<path_t> scale_path(path_t path, const gml::dvec3& scale)
{
	return scale_path_t<path_t>{std::move(path), scale};
}
}

#endif
