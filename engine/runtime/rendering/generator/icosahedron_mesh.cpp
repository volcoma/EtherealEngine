#include "icosahedron_mesh.hpp"
#include "iterator.hpp"

#include <array>

using namespace generator;

static const double a = (1.0 + std::sqrt(5.0)) / 2.0;
static const double d = std::sqrt(1.0 + a * a);
static const double x = 1.0 / d;
static const double y = a / d;

static const std::array<gml::dvec3, 12> vertices{{{-x, y, 0.0},
												  {x, y, 0.0},
												  {-x, -y, 0.0},
												  {x, -y, 0.0},
												  {0.0, -x, y},
												  {0.0, x, y},
												  {0.0, -x, -y},
												  {0.0, x, -y},
												  {y, 0.0, -x},
												  {y, 0.0, x},
												  {-y, 0.0, -x},
												  {-y, 0.0, x}}};

static const std::array<gml::ivec3, 20> triangles{
	{{0, 11, 5},  {0, 5, 1},  {0, 1, 7},  {0, 7, 10}, {0, 10, 11}, {1, 5, 9}, {5, 11, 4},
	 {11, 10, 2}, {10, 7, 6}, {7, 1, 8},  {3, 9, 4},  {3, 4, 2},   {3, 2, 6}, {3, 6, 8},
	 {3, 8, 9},   {4, 9, 5},  {2, 4, 11}, {6, 2, 10}, {8, 6, 7},   {9, 8, 1}}};

icosahedron_mesh_t::triangles_t::triangles_t(const icosahedron_mesh_t& mesh)
	: mesh_{&mesh}
	, i_{0}
	, triangle_mesh_{std::make_shared<triangle_mesh_t>(::vertices[::triangles[0][0]],
													   ::vertices[::triangles[0][1]],
													   ::vertices[::triangles[0][2]], mesh.segments_)}
	, triangles_{triangle_mesh_->triangles()}
{
}

bool icosahedron_mesh_t::triangles_t::done() const noexcept
{
	return i_ == static_cast<int>(::triangles.size());
}

triangle_t icosahedron_mesh_t::triangles_t::generate() const
{
	if(done())
		throw std::out_of_range("Done!");

	triangle_t triangle = triangles_.generate();

	const int base = i_ * mesh_->face_vertex_count_;

	triangle.vertices[0] += base;
	triangle.vertices[1] += base;
	triangle.vertices[2] += base;

	return triangle;
}

void icosahedron_mesh_t::triangles_t::next()
{
	if(done())
		throw std::out_of_range("Done!");

	triangles_.next();

	if(triangles_.done())
	{
		++i_;

		if(!done())
		{
			triangle_mesh_ = std::make_shared<triangle_mesh_t>(
				::vertices[::triangles[i_][0]], ::vertices[::triangles[i_][1]],
				::vertices[::triangles[i_][2]], mesh_->segments_);

			triangles_ = triangle_mesh_->triangles();
		}
	}
}

icosahedron_mesh_t::vertices_t::vertices_t(const icosahedron_mesh_t& mesh)
	: mesh_{&mesh}
	, i_{0}
	, triangle_mesh_{std::make_shared<triangle_mesh_t>(::vertices[::triangles[0][0]],
													   ::vertices[::triangles[0][1]],
													   ::vertices[::triangles[0][2]], mesh_->segments_)}
	, vertices_{triangle_mesh_->vertices()}
{
}

bool icosahedron_mesh_t::vertices_t::done() const noexcept
{
	return i_ == static_cast<int>(::triangles.size());
}

mesh_vertex_t icosahedron_mesh_t::vertices_t::generate() const
{
	if(done())
		throw std::out_of_range("Done!");

	mesh_vertex_t vertex = vertices_.generate();

	vertex.position *= mesh_->radius_;

	return vertex;
}

void icosahedron_mesh_t::vertices_t::next()
{
	if(done())
		throw std::out_of_range("Done!");

	vertices_.next();

	if(vertices_.done())
	{
		++i_;

		if(!done())
		{
			triangle_mesh_ = std::make_shared<triangle_mesh_t>(
				::vertices[::triangles[i_][0]], ::vertices[::triangles[i_][1]],
				::vertices[::triangles[i_][2]], mesh_->segments_);

			vertices_ = triangle_mesh_->vertices();
		}
	}
}

icosahedron_mesh_t::icosahedron_mesh_t(double radius, int segments)
	: radius_{radius}
	, segments_{segments}
	, face_vertex_count_{count(triangle_mesh_t{1.0, segments}.vertices())}
{
}

icosahedron_mesh_t::triangles_t icosahedron_mesh_t::triangles() const noexcept
{
	return {*this};
}

icosahedron_mesh_t::vertices_t icosahedron_mesh_t::vertices() const noexcept
{
	return {*this};
}
