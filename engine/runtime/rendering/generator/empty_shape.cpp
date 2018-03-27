#include "empty_shape.hpp"

#include <stdexcept>

using namespace generator;

empty_shape_t::edges_t::edges_t() = default;

edge_t empty_shape_t::edges_t::generate() const
{
	throw std::out_of_range("Called generate on an EmptyShape!");
}

bool empty_shape_t::edges_t::done() const noexcept
{
	return true;
}

void empty_shape_t::edges_t::next()
{
	throw std::out_of_range("Called next on an EmptyShape!");
}

empty_shape_t::vertices_t::vertices_t()
{
}

shape_vertex_t empty_shape_t::vertices_t::generate() const
{
	throw std::out_of_range("Called generate on an EmptyShape!");
}

bool empty_shape_t::vertices_t::done() const noexcept
{
	return true;
}

void empty_shape_t::vertices_t::next()
{
	throw std::out_of_range("Called next on an EmptyShape!");
}

empty_shape_t::edges_t empty_shape_t::edges() const noexcept
{
	return {};
}

empty_shape_t::vertices_t empty_shape_t::vertices() const noexcept
{
	return {};
}
