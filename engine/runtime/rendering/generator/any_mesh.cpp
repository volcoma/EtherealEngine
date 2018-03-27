#include "any_mesh.hpp"

using namespace generator;

any_mesh::base::~base() = default;

any_mesh::any_mesh(const any_mesh& that)
	: base_{that.base_->clone()}
{
}

any_mesh& any_mesh::operator=(const any_mesh& that)
{
	base_ = that.base_->clone();
	return *this;
}

any_generator<triangle_t> any_mesh::triangles() const noexcept
{
	return base_->triangles();
}

any_generator<mesh_vertex_t> any_mesh::vertices() const noexcept
{
	return base_->vertices();
}
