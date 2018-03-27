#include "parametric_mesh.hpp"

using namespace generator;

parametric_mesh_t::triangles_t::triangles_t(const parametric_mesh_t& mesh)
	: mesh_{&mesh}
	, i_{0}
	, even_{false}
{
}

triangle_t parametric_mesh_t::triangles_t::generate() const
{
	if(done())
		throw std::out_of_range("Done!");

	triangle_t triangle;

	const int base = i_[1] * (mesh_->segments_[0] + 1) + i_[0];

	if(!even_)
	{
		triangle.vertices = {base, base + 1, base + mesh_->segments_[0] + 1};
	}
	else
	{
		triangle.vertices = {base + 1, base + mesh_->segments_[0] + 2, base + mesh_->segments_[0] + 1};
	}

	return triangle;
}

bool parametric_mesh_t::triangles_t::done() const noexcept
{
	if(mesh_->segments_[0] == 0 || mesh_->segments_[1] == 0)
		return true;
	return i_[1] == mesh_->segments_[1];
}

void parametric_mesh_t::triangles_t::next()
{
	if(done())
		throw std::out_of_range("Done!");

	even_ = !even_;

	if(!even_)
	{
		++i_[0];
		if(i_[0] == mesh_->segments_[0])
		{
			i_[0] = 0;
			++i_[1];
		}
	}
}

parametric_mesh_t::vertices_t::vertices_t(const parametric_mesh_t& mesh)
	: mesh_{&mesh}
	, i_{0}
{
}

mesh_vertex_t parametric_mesh_t::vertices_t::generate() const
{
	if(done())
		throw std::out_of_range("Done!");

	return mesh_->eval_({i_[0] * mesh_->delta_[0], i_[1] * mesh_->delta_[1]});
}

bool parametric_mesh_t::vertices_t::done() const noexcept
{
	if(mesh_->segments_[0] == 0 || mesh_->segments_[1] == 0)
		return true;
	return i_[1] > mesh_->segments_[1];
}

void parametric_mesh_t::vertices_t::next()
{
	if(done())
		throw std::out_of_range("Done!");

	++i_[0];
	if(i_[0] > mesh_->segments_[0])
	{
		i_[0] = 0;
		++i_[1];
	}
}

parametric_mesh_t::parametric_mesh_t(std::function<mesh_vertex_t(const gml::dvec2& t)> eval,
									 const gml::ivec2& segments) noexcept
	: eval_{std::move(eval)}
	, segments_{segments}
	, delta_{1.0 / segments[0], 1.0 / segments[1]}
{
}

parametric_mesh_t::triangles_t parametric_mesh_t::triangles() const noexcept
{
	return *this;
}

parametric_mesh_t::vertices_t parametric_mesh_t::vertices() const noexcept
{
	return *this;
}
