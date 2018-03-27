#include "dodecahedron_mesh.hpp"
#include "iterator.hpp"

#include <array>
#include <memory>

using namespace generator;

static const double a = (1.0 + std::sqrt(5.0)) / 2.0;

static const std::array<gml::dvec3, 20> vertices{
	{{1.0, 1.0, -1.0},   {-1.0, 1.0, -1.0},   {1.0, -1.0, -1.0},  {-1.0, -1.0, -1.0},
	 {1.0, 1.0, 1.0},	{-1.0, 1.0, 1.0},	{1.0, -1.0, 1.0},   {-1.0, -1.0, 1.0},

	 {0.0, 1.0 / a, -a}, {0.0, -1.0 / a, -a}, {0.0, 1.0 / a, a},  {0.0, -1.0 / a, a},

	 {1.0 / a, a, 0.0},  {-1.0 / a, a, 0.0},  {1.0 / a, -a, 0.0}, {-1.0 / a, -a, 0.0},

	 {a, 0.0, -1.0 / a}, {-a, 0.0, -1.0 / a}, {a, 0.0, 1.0 / a},  {-a, 0.0, 1.0 / a}}};

static const std::array<std::array<int, 5>, 12> polygons{{
	{{0, 12, 4, 18, 16}}, // 0
	{{18, 6, 14, 2, 16}}, // 1
	{{4, 10, 11, 6, 18}}, // 2
	{{11, 7, 15, 14, 6}}, // 3
	{{7, 11, 10, 5, 19}}, // 4
	{{17, 3, 15, 7, 19}}, // 5
	{{9, 2, 14, 15, 3}},  // 6
	{{1, 8, 9, 3, 17}},   // 7
	{{10, 4, 12, 13, 5}}, // 8
	{{5, 13, 1, 17, 19}}, // 9
	{{0, 8, 1, 13, 12}},  // 10
	{{0, 16, 2, 9, 8}}	// 11
}};

static std::vector<gml::dvec3> makevertices_t(int faceIndex) noexcept
{
	std::vector<gml::dvec3> result(5);
	for(int i = 0; i < 5; ++i)
	{
		result.at(i) = gml::normalize(vertices.at(polygons.at(faceIndex)[i]));
	}
	return result;
}

dodecahedron_mesh_t::triangles_t::triangles_t(const dodecahedron_mesh_t& mesh) noexcept
	: mesh_{&mesh}
	, face_index_{0}
	, face_mesh_{std::make_shared<convex_polygon_mesh_t>(makevertices_t(0), mesh_->segments_, mesh_->rings_)}
	, triangles_{face_mesh_->triangles()}
{
}

bool dodecahedron_mesh_t::triangles_t::done() const noexcept
{
	return face_index_ == ::polygons.size();
}

triangle_t dodecahedron_mesh_t::triangles_t::generate() const
{
	if(done())
		throw std::out_of_range("Done!");

	triangle_t triangle = triangles_.generate();

	const int base = face_index_ * mesh_->face_vertex_count_;

	triangle.vertices[0] += base;
	triangle.vertices[1] += base;
	triangle.vertices[2] += base;

	return triangle;
}

void dodecahedron_mesh_t::triangles_t::next()
{
	if(done())
		throw std::out_of_range("Done!");

	triangles_.next();

	if(triangles_.done())
	{
		++face_index_;

		if(!done())
		{
			face_mesh_ =
				std::make_shared<convex_polygon_mesh_t>(makevertices_t(face_index_), mesh_->segments_);

			triangles_ = face_mesh_->triangles();
		}
	}
}

dodecahedron_mesh_t::vertices_t::vertices_t(const dodecahedron_mesh_t& mesh) noexcept
	: mesh_{&mesh}
	, face_index_{0}
	, face_mesh_{std::make_shared<convex_polygon_mesh_t>(makevertices_t(0), mesh_->segments_, mesh_->rings_)}
	, vertices_{face_mesh_->vertices()}
{
}

bool dodecahedron_mesh_t::vertices_t::done() const noexcept
{
	return face_index_ == ::polygons.size();
}

mesh_vertex_t dodecahedron_mesh_t::vertices_t::generate() const
{

	mesh_vertex_t vertex = vertices_.generate();

	vertex.position *= mesh_->radius_;

	return vertex;
}

void dodecahedron_mesh_t::vertices_t::next()
{
	vertices_.next();

	if(vertices_.done())
	{
		++face_index_;

		if(!done())
		{
			face_mesh_ = std::make_shared<convex_polygon_mesh_t>(makevertices_t(face_index_),
																 mesh_->segments_, mesh_->rings_);

			vertices_ = face_mesh_->vertices();
		}
	}
}

dodecahedron_mesh_t::dodecahedron_mesh_t(double radius, int segments, int rings) noexcept
	: radius_{radius}
	, segments_{segments}
	, rings_{rings}
	, face_vertex_count_{count(convex_polygon_mesh_t{1.0, 5u, segments, rings}.vertices())}
{
}

dodecahedron_mesh_t::triangles_t dodecahedron_mesh_t::triangles() const noexcept
{
	return triangles_t{*this};
}

dodecahedron_mesh_t::vertices_t dodecahedron_mesh_t::vertices() const noexcept
{
	return vertices_t{*this};
}
