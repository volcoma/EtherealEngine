#include "triangle_mesh.hpp"

#include <stdexcept>

using namespace generator;

triangle_mesh_t::triangles_t::triangles_t(const triangle_mesh_t& mesh)
	: mesh_{&mesh}
	, row_{0}
	, col_{0}
	, i_{0}
{
}

bool triangle_mesh_t::triangles_t::done() const noexcept
{
	return row_ == mesh_->segments_;
}

triangle_t triangle_mesh_t::triangles_t::generate() const
{
	if(done())
		throw std::out_of_range("Done!");

	triangle_t triangle;

	if(col_ % 2 == 0)
	{
		triangle.vertices[0] = i_;
		triangle.vertices[1] = i_ + 1;
		triangle.vertices[2] = i_ + 1 + mesh_->segments_ - row_;
	}
	else
	{
		triangle.vertices[0] = i_;
		triangle.vertices[1] = i_ + 1 + mesh_->segments_ - row_;
		triangle.vertices[2] = i_ + mesh_->segments_ - row_;
	}

	return triangle;
}

void triangle_mesh_t::triangles_t::next()
{
	if(done())
		throw std::out_of_range("Done!");

	if(col_ % 2 == 0)
		++i_;

	++col_;
	if(col_ == 2 * (mesh_->segments_ - row_) - 1)
	{
		++i_;
		col_ = 0;
		++row_;
	}
}

triangle_mesh_t::vertices_t::vertices_t(const triangle_mesh_t& mesh)
	: mesh_{&mesh}
	, row_{0}
	, col_{0}
{
}

bool triangle_mesh_t::vertices_t::done() const noexcept
{
	return row_ > mesh_->segments_;
}

mesh_vertex_t triangle_mesh_t::vertices_t::generate() const
{
	if(done())
		throw std::out_of_range("Done!");

	mesh_vertex_t vertex;

	if(row_ == mesh_->segments_)
	{
		vertex.position = mesh_->v2_;
		vertex.tex_coord = gml::dvec2{0.5, 1.0};
	}
	else
	{
		const double t = 1.0 / mesh_->segments_ * row_;
		const double t2 = 1.0 / (mesh_->segments_ - row_) * col_;

		const auto e1 = gml::mix(mesh_->v0_, mesh_->v2_, t);
		const auto e2 = gml::mix(mesh_->v1_, mesh_->v2_, t);

		vertex.position = gml::mix(e1, e2, t2);

		vertex.tex_coord[0] = t2;
		vertex.tex_coord[1] = t;
	}

	vertex.normal = mesh_->normal_;
	return vertex;
}

void triangle_mesh_t::vertices_t::next()
{
	if(done())
		throw std::out_of_range("Done!");

	++col_;
	if(col_ > mesh_->segments_ - row_)
	{
		col_ = 0;
		++row_;
	}
}

triangle_mesh_t::triangle_mesh_t(double radius, int segments)
	: triangle_mesh_t{radius * gml::normalize(gml::dvec3{-1.0, -1.0, 0.0}),
					  radius * gml::normalize(gml::dvec3{1.0, -1.0, 0.0}), gml::dvec3{0.0, radius, 0.0},
					  segments}
{
}

triangle_mesh_t::triangle_mesh_t(const gml::dvec3& v0, const gml::dvec3& v1, const gml::dvec3& v2,
								 int segments)
	: v0_{v0}
	, v1_{v1}
	, v2_{v2}
	, normal_{gml::normal(v0, v1, v2)}
	, segments_{segments}
{
}

triangle_mesh_t::triangles_t triangle_mesh_t::triangles() const noexcept
{
	return triangles_t{*this};
}

triangle_mesh_t::vertices_t triangle_mesh_t::vertices() const noexcept
{
    return vertices_t{*this};
}
