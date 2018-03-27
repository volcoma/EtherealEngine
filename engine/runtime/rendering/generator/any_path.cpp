#include "any_path.hpp"

using namespace generator;

any_path::base::~base() = default;

any_path::any_path(const any_path& that)
	: base_{that.base_->clone()}
{
}

any_path& any_path::operator=(const any_path& that)
{
	base_ = that.base_->clone();
	return *this;
}

any_generator<edge_t> any_path::edges() const noexcept
{
	return base_->edges();
}

any_generator<path_vertex_t> any_path::vertices() const noexcept
{
	return base_->vertices();
}
