#ifndef GENERATOR_SVG_HPP
#define GENERATOR_SVG_HPP

#include <algorithm>
#include <array>
#include <map>
#include <memory>
#include <sstream>
#include <vector>

#include "math.hpp"

#include "iterator.hpp"
#include "mesh_vertex.hpp"
#include "path_vertex.hpp"
#include "shape_vertex.hpp"
#include "triangle.hpp"

namespace generator
{

/// A simple svg writer class for generating preview and debug images.
class svg_writer_t
{
private:
	class BaseElem
	{
	public:
		double z_;

		gml::dvec3 color_;

		BaseElem(double z, const gml::dvec3& color);

		virtual ~BaseElem();

		// Writes this svg element to a stream.
		virtual void stream(std::ostream&) const = 0;
	};

	class VertexElem : public BaseElem
	{
	public:
		gml::dvec3 p_;

		VertexElem(const gml::dvec3& p, const gml::dvec3& color);

		virtual void stream(std::ostream& os) const override;
	};

	class LineElem : public BaseElem
	{
	public:
		gml::dvec3 p1_, p2_;

		LineElem(const gml::dvec3& p1, const gml::dvec3& p2, const gml::dvec3& color);

		virtual void stream(std::ostream& os) const override;
	};

	class TriangleElem : public BaseElem
	{
	public:
		std::array<gml::dvec3, 3> p_;

		TriangleElem(const gml::dvec3& p1, const gml::dvec3& p2, const gml::dvec3& p3,
					 const gml::dvec3& color);

		virtual void stream(std::ostream& os) const override;
	};

	gml::dvec3 project(const gml::dvec3& p) const;

	gml::dvec3 normalToColor(const gml::dvec3& normal) const;

	gml::ivec2 size_;

	gml::dmat4 viewMatrix_;
	gml::dmat4 projMatrix_;
	gml::dmat4 viewProjMatrix_;
	gml::ivec2 viewportOrigin_;
	gml::ivec2 viewportSize_;

	gml::dvec3 lightDir_;

	bool cullface_;

	mutable std::vector<std::unique_ptr<BaseElem>> elems_;

public:
	/// @param width Width of the image in pixels
	/// @param height Height of the iamge in pixels
	svg_writer_t(int width, int height);

	/// Sets the model view matrix. Default is the identity matrix.
	void modelView(const gml::dmat4& matrix);

	/// Sets the projection mode to perspective projection.
	/// Default is the orthographic.
	/// @param fovy Field of view along the y-axis.
	/// @param aspect aspect ratio (should usually match the vieport)
	void perspective(double fovy, double aspect, double zNear, double zFar);

	/// Sets the projection mode to orthographic projection.
	/// This is the default.
	/// @param left Coordinate that maps to the left edge.
	/// @param right Coordinate that maps to the right edge.
	void ortho(double left, double right, double bottom, double top);

	/// Sets the viewport. Default fills the whole image.
	void viewport(int x, int y, int width, int height);

	/// Sets if backfacing triangles should be culled. Default is true.
	void cullface(bool cullface);

	/// Write one point. Drawn as a circle.
	void writePoint(const gml::dvec3& p, const gml::dvec3& color = {0.0, 0.0, 0.0});

	/// Write one line.
	void writeLine(const gml::dvec3& p1, const gml::dvec3& p2, const gml::dvec3& color = {0.0, 0.0, 0.0});

	/// Write one triangle.
	void writeTriangle(const gml::dvec3& p1, const gml::dvec3& p2, const gml::dvec3& p3,
					   const gml::dvec3& color);

	/// Write one triangle with color automatically calculated from light.
	void writeTriangle(const gml::dvec3& p1, const gml::dvec3& p2, const gml::dvec3& p3);

	/// Write all shaped edges and optionally vertices, tangents and normals.
	template <typename shape_t>
	void writeShape(const shape_t& shape, bool writeVertices = false, bool writeAxis = false)
	{

		std::vector<shape_vertex_t> vertices{};
		for(const auto& vertex : shape.vertices())
		{
			vertices.push_back(vertex);
		}

		for(auto e : shape.edges())
		{
			auto p1 = gml::dvec3{vertices[e.vertices[0]].position, 0.0};
			auto p2 = gml::dvec3{vertices[e.vertices[1]].position, 0.0};

			writeLine(p1, p2, {0.5, 0.5, 0.5});
		}

		if(writeAxis)
		{
			for(auto v : vertices)
			{
				auto p1 = gml::dvec3{v.position, 0.0};
				auto p2 = gml::dvec3{v.position + 0.1 * v.tangent, 0.0};
				auto p3 = gml::dvec3{v.position + 0.1 * v.normal(), 0.0};

				writeLine(p1, p2, {0.0, 1.0, 0.0});
				writeLine(p1, p3, {1.0, 0.0, 0.0});
			}
		}

		if(writeVertices)
		{
			for(auto v : shape.vertices())
			{
				writePoint(gml::dvec3{v.position, 0.0});
			}
		}
	}

	/// Write all path edges as lines and optionally vertices, tangents, normals
	/// and binormals.
	template <typename Path>
	void writePath(const Path& path, bool writeVertices = false, bool writeAxis = false)
	{

		std::vector<path_vertex_t> vertices{};
		for(const auto& temp : path.vertices())
		{
			vertices.push_back(temp);
		}

		if(writeAxis)
		{
			for(const auto& v : path.vertices())
			{
				writeLine(v.position, v.position + 0.1 * v.tangent, {0.0, 0.0, 1.0});
				writeLine(v.position, v.position + 0.1 * v.normal, {1.0, 0.0, 0.0});
				writeLine(v.position, v.position + 0.1 * v.binormal(), {0.0, 1.0, 0.0});
			}
		}

		if(writeVertices)
		{
			for(const auto& v : path.vertices())
			{
				writePoint(v.position + 0.001 * v.normal);
			}
		}

		for(const auto& e : path.edges())
		{
			writeLine(vertices[e.vertices[0]].position, vertices[e.vertices[1]].position);
		}
	}

	/// Write all triangles from a mesh.
	template <typename Mesh>
	void writeMesh(const Mesh& mesh, bool writeVertices = false, bool writeNormals = false)
	{

		std::vector<mesh_vertex_t> vertices{};
		for(const mesh_vertex_t& vertex : mesh.vertices())
		{
			vertices.push_back(vertex);
		}

		for(triangle_t t : mesh.triangles())
		{
			writeTriangle(vertices[t.vertices[0]].position, vertices[t.vertices[1]].position,
						  vertices[t.vertices[2]].position);
		}

		if(writeVertices)
		{
			for(const auto& v : vertices)
			{
				writePoint(v.position);
			}
		}

		// Normals
		if(writeNormals)
		{
			for(const auto& v : vertices)
			{
				writeLine(v.position, v.position + 0.1 * v.normal, {0.0, 0.0, 1.0});
			}
		}
	}

	/// Generates svg xml from the data written so far.
	std::string str() const;
};
}

#endif
