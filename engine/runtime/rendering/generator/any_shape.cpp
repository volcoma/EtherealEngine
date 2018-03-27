#include "any_shape.hpp"

using namespace generator;

any_shape::base::~base()
{
}

any_shape::any_shape(const any_shape& that)
	: base_{that.base_->clone()}
{
}

any_shape& any_shape::operator=(const any_shape& that)
{
	base_ = that.base_->clone();
	return *this;
}

any_generator<edge_t> any_shape::edges() const noexcept
{
	return base_->edges();
}

any_generator<shape_vertex_t> any_shape::vertices() const noexcept
{
	return base_->vertices();
}
