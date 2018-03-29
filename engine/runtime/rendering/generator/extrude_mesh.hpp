#ifndef GENERATOR_EXTRUDEMESH_HPP
#define GENERATOR_EXTRUDEMESH_HPP

#include "edge.hpp"
#include "iterator.hpp"
#include "mesh_vertex.hpp"
#include "path_vertex.hpp"
#include "shape_vertex.hpp"
#include "triangle.hpp"

namespace generator
{

/// Extrude a shape along a path.
/// The shape normal becomes the mesh normal.
/// u-texture coordinate is taken from the shape and v from the path.
template <typename shape_t, typename path_t>
class extrude_mesh_t
{
public:
	class triangles_t
	{
	public:
		triangle_t generate() const
		{
			triangle_t triangle;

			const auto& shapeEdge = shape_edges_.generate();
			const auto& pathEdge = path_edges_.generate();

			if(odd_ == 0)
			{
				triangle.vertices[0] =
					shapeEdge.vertices[0] + pathEdge.vertices[0] * mesh_->shape_vertex_count_;
				triangle.vertices[1] =
					shapeEdge.vertices[1] + pathEdge.vertices[1] * mesh_->shape_vertex_count_;
				triangle.vertices[2] =
					shapeEdge.vertices[0] + pathEdge.vertices[1] * mesh_->shape_vertex_count_;
			}
			else
			{
				triangle.vertices[0] =
					shapeEdge.vertices[0] + pathEdge.vertices[0] * mesh_->shape_vertex_count_;
				triangle.vertices[1] =
					shapeEdge.vertices[1] + pathEdge.vertices[0] * mesh_->shape_vertex_count_;
				triangle.vertices[2] =
					shapeEdge.vertices[1] + pathEdge.vertices[1] * mesh_->shape_vertex_count_;
			}

			return triangle;
		}

		bool done() const noexcept
		{
			return path_edges_.done();
		}

		void next()
		{
			odd_ = !odd_;

			if(odd_)
			{
				shape_edges_.next();
				if(shape_edges_.done())
				{
					path_edges_.next();
					shape_edges_ = mesh_->shape_.edges();
				}
			}
		}

	private:
		const extrude_mesh_t* mesh_;

		typename shape_t::edges_t shape_edges_;

        typename path_t::edges_t path_edges_;

		bool odd_;

		explicit triangles_t(const extrude_mesh_t& mesh)
			: mesh_{&mesh}
			, shape_edges_{mesh.shape_.edges()}
			, path_edges_{mesh.path_.edges()}
			, odd_{true}
		{
		}

		friend class extrude_mesh_t;
	};

	class vertices_t
	{
	public:
		mesh_vertex_t generate() const
		{
			mesh_vertex_t vertex;

			const auto& shapeVertex = shape_vertices_.generate();
			const auto& pathVertex = path_vertices_.generate();

			gml::dvec3 pathBinormal = pathVertex.binormal();
			vertex.position = pathVertex.position + shapeVertex.position[0] * pathVertex.normal +
							  shapeVertex.position[1] * pathBinormal;

			gml::dvec2 shapeNormal = shapeVertex.normal();
			vertex.normal = shapeNormal[0] * pathVertex.normal + shapeNormal[1] * pathBinormal;

			vertex.tex_coord[0] = shapeVertex.texCoord;
			vertex.tex_coord[1] = pathVertex.texCoord;

			return vertex;
		}

		bool done() const noexcept
		{
			return path_vertices_.done();
		}

		void next()
		{
			shape_vertices_.next();
			if(shape_vertices_.done())
			{
				path_vertices_.next();
				shape_vertices_ = mesh_->shape_.vertices();
			}
		}

	private:
		const extrude_mesh_t* mesh_;

        typename shape_t::vertices_t shape_vertices_;

        typename path_t::vertices_t path_vertices_;

		explicit vertices_t(const extrude_mesh_t& mesh)
			: mesh_{&mesh}
			, shape_vertices_{mesh.shape_.vertices()}
			, path_vertices_{mesh.path_.vertices()}
		{
		}

		friend class extrude_mesh_t;
	};

	triangles_t triangles() const noexcept
	{
        return triangles_t{*this};
	}

	vertices_t vertices() const noexcept
	{
        return vertices_t{*this};
	}

	/// @param shape shape_t to be extruded.
	/// @param path Path to extrude along.
	extrude_mesh_t(shape_t shape, path_t path)
		: shape_{std::move(shape)}
		, path_{std::move(path)}
		, shape_vertex_count_{count(shape_.vertices())}
	{
	}

private:
	shape_t shape_;

	path_t path_;

	int shape_vertex_count_;
};

template <typename shape_t, typename path_t>
extrude_mesh_t<shape_t, path_t> extrude_mesh(shape_t shape, path_t path)
{
	return extrude_mesh_t<shape_t, path_t>{std::move(shape), std::move(path)};
}
}

#endif
