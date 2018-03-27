#include "parametric_shape.hpp"

using namespace generator;

parametric_shape_t::edges_t::edges_t(const parametric_shape_t& shape)
	: shape_{&shape}
	, i_{0}
{
}

edge_t parametric_shape_t::edges_t::generate() const
{
	if(done())
		throw std::out_of_range("Done!");

	return edge_t{{i_, i_ + 1}};
}

bool parametric_shape_t::edges_t::done() const noexcept
{
	return i_ == shape_->segments_;
}

void parametric_shape_t::edges_t::next()
{
	if(done())
		throw std::out_of_range("Done!");
	++i_;
}

shape_vertex_t parametric_shape_t::vertices_t::generate() const
{
	if(done())
		throw std::out_of_range("Done!");

	return shape_->eval_(i_ * shape_->delta_);
}

parametric_shape_t::vertices_t::vertices_t(const parametric_shape_t& shape)
	: shape_{&shape}
	, i_{0}
{
}

bool parametric_shape_t::vertices_t::done() const noexcept
{
	if(shape_->segments_ == 0)
		return true;
	return i_ == shape_->segments_ + 1;
}

void parametric_shape_t::vertices_t::next()
{
	if(done())
		throw std::out_of_range("Done!");
	++i_;
}

parametric_shape_t::parametric_shape_t(std::function<shape_vertex_t(double)> eval, int segments) noexcept
	: eval_{std::move(eval)}
	, segments_{segments}
	, delta_{1.0 / segments}
{
}

parametric_shape_t::edges_t parametric_shape_t::edges() const noexcept
{
	return *this;
}

parametric_shape_t::vertices_t parametric_shape_t::vertices() const noexcept
{
	return *this;
}
