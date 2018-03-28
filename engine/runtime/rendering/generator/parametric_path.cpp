#include "parametric_path.hpp"

using namespace generator;

parametric_path_t::edges_t::edges_t(const parametric_path_t& path)
	: path_{&path}
	, i_{0}
{
}

edge_t parametric_path_t::edges_t::generate() const
{
	if(done())
		throw std::out_of_range("Done!");
	return edge_t{{i_, i_ + 1}};
}

bool parametric_path_t::edges_t::done() const noexcept
{
	return i_ == path_->segments_;
}

void parametric_path_t::edges_t::next()
{
	if(done())
		throw std::out_of_range("Done!");
	++i_;
}

parametric_path_t::vertices_t::vertices_t(const parametric_path_t& path)
	: path_{&path}
	, i_{0}
{
}

path_vertex_t parametric_path_t::vertices_t::generate() const
{
	if(done())
		throw std::out_of_range("Done!");

	return path_->eval_(i_ * path_->delta_);
}

bool parametric_path_t::vertices_t::done() const noexcept
{
	if(path_->segments_ == 0)
		return true;
	return i_ == path_->segments_ + 1;
}

void parametric_path_t::vertices_t::next()
{
	if(done())
		throw std::out_of_range("Done!");
	++i_;
}

parametric_path_t::parametric_path_t(std::function<path_vertex_t(double)> eval, int segments) noexcept
	: eval_{std::move(eval)}
	, segments_{segments}
	, delta_{1.0 / segments}
{
}

parametric_path_t::edges_t parametric_path_t::edges() const noexcept
{
    return edges_t{*this};
}

parametric_path_t::vertices_t parametric_path_t::vertices() const noexcept
{
    return vertices_t{*this};
}
