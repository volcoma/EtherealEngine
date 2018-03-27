#include "empty_path.hpp"

using namespace generator;

edge_t empty_path_t::edges_t::generate() const
{
	throw std::out_of_range("Called generate on an EmptyPath!");
}

bool empty_path_t::edges_t::done() const noexcept
{
	return true;
}

void empty_path_t::edges_t::next()
{
	throw std::out_of_range("Called next on an EmptyPath!");
}

empty_path_t::edges_t::edges_t()
{
}

path_vertex_t empty_path_t::vertices_t::generate() const
{
	throw std::out_of_range("Called generate on an EmptyPath!");
}

bool empty_path_t::vertices_t::done() const noexcept
{
	return true;
}

void empty_path_t::vertices_t::next()
{
	throw std::out_of_range("Called next on an EmptyPath!");
}

empty_path_t::vertices_t::vertices_t()
{
}

empty_path_t::empty_path_t()
{
}

empty_path_t::edges_t empty_path_t::edges() const noexcept
{
	return {};
}

empty_path_t::vertices_t empty_path_t::vertices() const noexcept
{
	return {};
}
