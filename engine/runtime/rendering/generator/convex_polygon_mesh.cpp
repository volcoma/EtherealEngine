#include "convex_polygon_mesh.hpp"

#include "circle_shape.hpp"
#include "iterator.hpp"

using namespace generator;

convex_polygon_mesh_t::triangles_t::triangles_t(const convex_polygon_mesh_t& mesh) noexcept
	: mesh_{&mesh}
	, odd_{false}
	, segment_index_{0}
	, side_index_{0}
	, ring_index_{0}
{
}

bool convex_polygon_mesh_t::triangles_t::done() const noexcept
{
	return mesh_->segments_ == 0 || mesh_->vertices_.size() < 3 || ring_index_ == mesh_->rings_;
}

triangle_t convex_polygon_mesh_t::triangles_t::generate() const
{
	if(done())
		throw std::runtime_error("Done!");

	triangle_t triangle{};

	const int verticesPerRing = mesh_->segments_ * int(mesh_->vertices_.size());
	const int delta = ring_index_ * verticesPerRing + 1;

	const int n1 = side_index_ * mesh_->segments_ + segment_index_;
	const int n2 = (n1 + 1) % verticesPerRing;

	if(ring_index_ == mesh_->rings_ - 1)
	{
		triangle.vertices[0] = 0;
		triangle.vertices[1] = n1 + delta;
		triangle.vertices[2] = n2 + delta;
	}
	else
	{
		if(!odd_)
		{
			triangle.vertices[0] = n1 + delta;
			triangle.vertices[1] = n2 + delta;
			triangle.vertices[2] = n1 + verticesPerRing + delta;
		}
		else
		{
			triangle.vertices[0] = n2 + delta;
			triangle.vertices[1] = n2 + verticesPerRing + delta;
			triangle.vertices[2] = n1 + verticesPerRing + delta;
		}
	}

	return triangle;
}

void convex_polygon_mesh_t::triangles_t::next()
{
	if(done())
		throw std::runtime_error("Done!");

	if(ring_index_ == mesh_->rings_ - 1)
	{
		++segment_index_;

		if(segment_index_ == mesh_->segments_)
		{
			segment_index_ = 0;

			++side_index_;

			if(side_index_ == static_cast<int>(mesh_->vertices_.size()))
			{
				++ring_index_;
			}
		}
	}
	else
	{

		odd_ = !odd_;

		if(!odd_)
		{

			++segment_index_;

			if(segment_index_ == mesh_->segments_)
			{
				segment_index_ = 0;

				++side_index_;

				if(side_index_ == static_cast<int>(mesh_->vertices_.size()))
				{
					side_index_ = 0;
					odd_ = false;

					++ring_index_;
				}
			}
		}
	}
}

convex_polygon_mesh_t::vertices_t::vertices_t(const convex_polygon_mesh_t& mesh) noexcept
	: mesh_{&mesh}
	, center_done_{false}
	, segment_index_{0}
	, side_index_{0}
	, ring_index_{0}
{
}

bool convex_polygon_mesh_t::vertices_t::done() const noexcept
{
	return mesh_->segments_ == 0 || mesh_->rings_ == 0 || mesh_->vertices_.size() < 3 ||
		   ring_index_ == mesh_->rings_;
}

mesh_vertex_t convex_polygon_mesh_t::vertices_t::generate() const
{
	if(done())
		throw std::runtime_error("Done!");

	mesh_vertex_t vertex{};

	if(center_done_)
	{

		const double ringDelta = static_cast<double>(ring_index_) / mesh_->rings_;
		const double segmentDelta = static_cast<double>(segment_index_) / mesh_->segments_;

		const int nextSide = (side_index_ + 1) % mesh_->vertices_.size();
		const gml::dvec3 a = gml::mix(mesh_->vertices_.at(side_index_), mesh_->center_, ringDelta);
		const gml::dvec3 b = gml::mix(mesh_->vertices_.at(nextSide), mesh_->center_, ringDelta);

		vertex.position = gml::mix(a, b, segmentDelta);
	}
	else
	{
		vertex.position = mesh_->center_;
	}

	vertex.normal = mesh_->normal_;

	const gml::dvec3 delta = vertex.position - mesh_->center_;

	vertex.tex_coord[0] = gml::dot(mesh_->tangent_, delta);
	vertex.tex_coord[1] = gml::dot(mesh_->bitangent_, delta);

	vertex.tex_coord -= mesh_->tex_delta_;

	return vertex;
}

void convex_polygon_mesh_t::vertices_t::next()
{
	if(done())
		throw std::runtime_error("Done!");

	if(!center_done_)
	{
		center_done_ = true;
	}
	else
	{

		++segment_index_;

		if(segment_index_ == mesh_->segments_)
		{
			segment_index_ = 0;

			++side_index_;

			if(side_index_ == static_cast<int>(mesh_->vertices_.size()))
			{

				side_index_ = 0;

				++ring_index_;
			}
		}
	}
}

namespace
{

std::vector<gml::dvec3> makevertices_t(double radius, int sides) noexcept
{
	std::vector<gml::dvec3> result{};

	circle_shape_t circle{radius, sides};
	for(const auto& v : circle.vertices())
	{
		result.push_back(gml::dvec3{v.position[0], v.position[1], 0.0});
	}
	result.pop_back(); // The last one is a dublicate with the first one

	return result;
}

std::vector<gml::dvec3> convertvertices_t(const std::vector<gml::dvec2>& vertices) noexcept
{
	std::vector<gml::dvec3> result(vertices.size());

	for(std::size_t i = 0; i < vertices.size(); ++i)
	{
		result.at(i) = gml::dvec3{vertices.at(i)[0], vertices.at(i)[1], 0.0};
	}

	return result;
}

gml::dvec3 calcCenter(const std::vector<gml::dvec3>& vertices) noexcept
{
	gml::dvec3 result{};
	for(const auto& v : vertices)
	{
		result += v;
	}
	return result / static_cast<double>(vertices.size());
}

gml::dvec3 calcNormal(const gml::dvec3& center, const std::vector<gml::dvec3>& vertices)
{
	gml::dvec3 normal{};
	for(int i = 0; i < static_cast<int>(vertices.size()); ++i)
	{
		normal += gml::normal(center, vertices[i], vertices[(i + 1) % vertices.size()]);
	}
	return gml::normalize(normal);
}
}

convex_polygon_mesh_t::convex_polygon_mesh_t(double radius, int sides, int segments, int rings) noexcept
	: convex_polygon_mesh_t{makevertices_t(radius, sides), segments, rings}
{
}

convex_polygon_mesh_t::convex_polygon_mesh_t(const std::vector<gml::dvec2>& vertices, int segments,
											 int rings) noexcept
	: convex_polygon_mesh_t{convertvertices_t(vertices), segments, rings}
{
}

convex_polygon_mesh_t::convex_polygon_mesh_t(std::vector<gml::dvec3> vertices, int segments,
											 int rings) noexcept
	: vertices_{std::move(vertices)}
	, segments_{segments}
	, rings_{rings}
	, center_{calcCenter(vertices_)}
	, normal_{calcNormal(center_, vertices_)}
	, tangent_{}
	, bitangent_{}
	, tex_delta_{}
{

	if(vertices_.size() > 0)
	{
		tangent_ = gml::normalize(vertices_.at(0) - center_);
		bitangent_ = gml::cross(normal_, tangent_);

		gml::dvec2 texMax{};

		for(const gml::dvec3& vertex : vertices_)
		{
			gml::dvec3 delta = vertex - center_;

			gml::dvec2 uv{gml::dot(tangent_, delta), gml::dot(bitangent_, delta)};

			tex_delta_ = gml::min(tex_delta_, uv);
			texMax = gml::max(texMax, uv);
		}

		gml::dvec2 size = texMax - tex_delta_;

		tangent_ /= size[0];
		bitangent_ /= size[1];

		tex_delta_ /= size;
	}
}

convex_polygon_mesh_t::triangles_t convex_polygon_mesh_t::triangles() const noexcept
{
	return triangles_t{*this};
}

convex_polygon_mesh_t::vertices_t convex_polygon_mesh_t::vertices() const noexcept
{
	return vertices_t{*this};
}
