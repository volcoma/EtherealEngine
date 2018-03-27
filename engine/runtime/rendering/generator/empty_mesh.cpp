#include "empty_mesh.hpp"

using namespace generator;

empty_mesh::triangles_t::triangles_t() = default;

triangle_t empty_mesh::triangles_t::generate() const
{
	throw std::out_of_range("Called generate on an EmptyMesh!");
}

bool empty_mesh::triangles_t::done() const noexcept
{
	return true;
}

void empty_mesh::triangles_t::next()
{
	throw std::out_of_range("Called next on an EmptyMesh!");
}

empty_mesh::vertices_t::vertices_t() = default;

mesh_vertex_t empty_mesh::vertices_t::generate() const
{
	throw std::out_of_range("Called generate on an EmptyMesh!");
}

bool empty_mesh::vertices_t::done() const noexcept
{
	return true;
}

void empty_mesh::vertices_t::next()
{
	throw std::out_of_range("Called next on an EmptyMesh!");
}

empty_mesh::empty_mesh() = default;

empty_mesh::triangles_t empty_mesh::triangles() const noexcept
{
	return {};
}

empty_mesh::vertices_t empty_mesh::vertices() const noexcept
{
	return {};
}
