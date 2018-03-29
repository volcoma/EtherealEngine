#ifndef GENERATOR_LATHEMESH_HPP
#define GENERATOR_LATHEMESH_HPP

#include <memory>
#include <vector>

#include "axis.hpp"
#include "mesh_vertex.hpp"
#include "shape_vertex.hpp"
#include "triangle.hpp"

namespace generator
{

/// Spins a shape around an axis to create a mesh.
/// u-texture coordinate is taken from the shape and v runs from 0 to 1 around
/// the axis counterclockwise.
template <typename shape_t>
class lathe_mesh_t
{
public:
	class triangles_t
	{
	public:
		triangle_t generate() const
		{
			triangle_t triangle;

			const auto& shapeEdge = shape_edges_.generate();

			int slice = i_ / 2;

			const int delta = mesh_->slices_ + 1;

			if(i_ % 2 == 0)
			{
				triangle.vertices[0] = shapeEdge.vertices[0] * delta + slice;
				triangle.vertices[1] = shapeEdge.vertices[1] * delta + slice;
				triangle.vertices[2] = shapeEdge.vertices[1] * delta + slice + 1;
			}
			else
			{
				triangle.vertices[0] = shapeEdge.vertices[0] * delta + slice;
				triangle.vertices[1] = shapeEdge.vertices[1] * delta + slice + 1;
				triangle.vertices[2] = shapeEdge.vertices[0] * delta + slice + 1;
			}

			return triangle;
		}

		bool done() const noexcept
		{
			return shape_edges_.done();
		}

		void next()
		{
			++i_;
			if(i_ == 2 * mesh_->slices_)
			{
				i_ = 0;
				shape_edges_.next();
			}
		}

	private:
		const lathe_mesh_t* mesh_;

		typename shape_t::edges_t shape_edges_;

		int i_;

		explicit triangles_t(const lathe_mesh_t& mesh)
			: mesh_{&mesh}
			, shape_edges_{mesh.shape_.edges()}
			, i_{0}
		{
		}

		friend class lathe_mesh_t;
	};

	class vertices_t
	{
	public:
		mesh_vertex_t generate() const
		{
			mesh_vertex_t vertex;

			const auto shapeVertex = shape_vertices_.generate();
			const gml::dvec2 normal = shapeVertex.normal();

			double deltaAngle = mesh_->sweep_ / mesh_->slices_;
			double angle = i_ * deltaAngle + mesh_->start_;

			const gml::dquat q = gml::qrotate(angle, mesh_->axis_);

			vertex.position =
				gml::transform(q, gml::dvec3{shapeVertex.position[0], shapeVertex.position[1], 0.0});

			vertex.normal = gml::transform(q, gml::dvec3{normal[0], normal[1], 0.0});

			vertex.tex_coord[0] = shapeVertex.tex_coord;
			vertex.tex_coord[1] = angle / mesh_->sweep_;

			return vertex;
		}

		bool done() const noexcept
		{
			return shape_vertices_.done();
		}

		void next()
		{
			++i_;

			if(i_ == mesh_->slices_ + 1)
			{
				i_ = 0;
				shape_vertices_.next();
			}
		}

	private:
		const lathe_mesh_t* mesh_;

        typename shape_t::vertices_t shape_vertices_;

		int i_;

		explicit vertices_t(const lathe_mesh_t& mesh)
			: mesh_{&mesh}
			, shape_vertices_{mesh.shape_.vertices()}
			, i_{0}
		{
		}

		friend class lathe_mesh_t;
	};

	/// @param shape The shape to spin.
	/// @param axis Unit length axis to spin around.
	/// @param slices Number of subdivisions around the axis.
	/// @param start Counterclockwise angle around the given axis relative to the xy-plane.
	/// relative to the xy-plane.
	/// @param sweep Counterclockwise angle around the given axis.
	lathe_mesh_t(shape_t shape, const gml::dvec2& axis, int slices = 32, double start = 0.0,
				 double sweep = gml::radians(360.0))
		: axis_{axis, 0.0}
		, shape_{std::move(shape)}
		, slices_{slices}
		, start_{start}
		, sweep_{sweep}
	{
	}

	triangles_t triangles() const noexcept
	{
        return triangles_t{*this};
	}

	vertices_t vertices() const noexcept
	{
        return vertices_t{*this};
	}

private:
	gml::dvec3 axis_;

	shape_t shape_;

	int slices_;

	double start_;

	double sweep_;
};

template <typename shape_t>
lathe_mesh_t<shape_t> lathe(shape_t shape, axis_t axis = axis_t::X, int slices = 32, double start = 0.0,
							double sweep = gml::radians(360.0))
{
	return lathe_mesh_t<shape_t>{std::move(shape), axis, slices, start, sweep};
}
}

#endif
