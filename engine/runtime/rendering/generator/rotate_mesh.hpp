#ifndef GENERATOR_ROTATEMESH_HPP
#define GENERATOR_ROTATEMESH_HPP

#include "axis.hpp"
#include "transform_mesh.hpp"

namespace generator
{

/// Rotates vertices and normals.
template <typename mesh_t>
class rotate_mesh_t
{
private:
	using impl_t = transform_mesh_t<mesh_t>;
	impl_t transform_mesh_;

public:
	/// @param mesh Source mesh data.
	/// @param rotation Quaternion presenting the rotation.
	rotate_mesh_t(mesh_t mesh, const gml::dquat& rotation)
		: transform_mesh_{std::move(mesh), [rotation](mesh_vertex_t& value) {
							  value.position = gml::transform(rotation, value.position);
							  value.normal = gml::transform(rotation, value.normal);
						  }}
	{
	}

	/// @param mesh Source mesh data.
	/// @param angle Counterclockwise angle around the given axis.
	/// @param axis Unit length axis to rotate around.
	rotate_mesh_t(mesh_t mesh, double angle, const gml::dvec3& axis)
		: rotate_mesh_t{std::move(mesh), gml::qrotate(angle, axis)}
	{
	}

	rotate_mesh_t(mesh_t mesh, double angle, axis_t axis)
		: rotate_mesh_t{std::move(mesh),
						gml::qrotate(angle, axis == axis_t::X
												? gml::dvec3{1.0, 0.0, 0.0}
												: (axis == axis_t::Y ? gml::dvec3{0.0, 1.0, 0.0}
																	 : gml::dvec3{0.0, 0.0, 1.0}))}
	{
	}

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return transform_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return transform_mesh_.vertices();
	}
};

template <typename mesh_t>
rotate_mesh_t<mesh_t> rotate_mesh(mesh_t mesh, const gml::dquat& rotation)
{
	return rotate_mesh_t<mesh_t>{std::move(mesh), rotation};
}

template <typename mesh_t>
rotate_mesh_t<mesh_t> rotate_mesh(mesh_t mesh, double angle, const gml::dvec3& axis)
{
	return rotate_mesh_t<mesh_t>{std::move(mesh), angle, axis};
}

template <typename mesh_t>
rotate_mesh_t<mesh_t> rotate_mesh(mesh_t mesh, double angle, axis_t axis)
{
	return rotate_mesh_t<mesh_t>{std::move(mesh), angle, axis};
}
}

#endif
