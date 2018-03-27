#ifndef GENERATOR_ROTATEPATH_HPP
#define GENERATOR_ROTATEPATH_HPP

#include "axis.hpp"
#include "transform_path.hpp"

namespace generator
{

/// Rotates vertices, tangents and normals.
template <typename path_t>
class rotate_path_t
{
private:
	using impl_t = transform_path_t<path_t>;
	impl_t transform_path_;

public:
	rotate_path_t(path_t path, const gml::dquat& rotation)
		: transform_path_{std::move(path), [rotation](path_vertex_t& value) {
							  value.position = gml::transform(rotation, value.position);
							  value.normal = gml::transform(rotation, value.normal);
						  }}
	{
	}

	rotate_path_t(path_t path, double angle, const gml::dvec3& axis)
		: rotate_path_t{std::move(path), gml::qrotate(angle, axis)}
	{
	}

	rotate_path_t(path_t path, double angle, axis_t axis)
		: rotate_path_t{std::move(path),
						gml::qrotate(angle, axis == axis_t::X
												? gml::dvec3{1.0, 0.0, 0.0}
												: (axis == axis_t::Y ? gml::dvec3{0.0, 1.0, 0.0}
																	 : gml::dvec3{0.0, 0.0, 1.0}))}
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
rotate_path_t<path_t> rotate_path(path_t path, const gml::dquat& rotation)
{
	return rotate_path_t<path_t>{std::move(path), rotation};
}

template <typename path_t>
rotate_path_t<path_t> rotate_path(path_t path, double angle, const gml::dvec3& axis)
{
	return rotate_path_t<path_t>{std::move(path), angle, axis};
}

template <typename path_t>
rotate_path_t<path_t> rotate_path(path_t path, double angle, axis_t axis)
{
	return rotate_path_t<path_t>{std::move(path), angle, axis};
}
}

#endif
