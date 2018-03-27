#ifndef GENERATOR_REPEATMESH_HPP
#define GENERATOR_REPEATMESH_HPP

#include "mesh_vertex.hpp"
#include "triangle.hpp"
#include "utils.hpp"

namespace generator
{

/**
 * Repeats the same mesh a given number of time at given intervals.
 */
template <typename mesh_t>
class repeat_mesh_t
{
public:
	class triangles_t
	{
	public:
		bool done() const noexcept
		{
			return index_ >= repeat_mesh_->instances_;
		}

		triangle_t generate() const
		{
			triangle_t temp = triangles_.generate();
			temp.vertices += delta_;
			return temp;
		}

		void next() noexcept
		{
			triangles_.next();

			if(triangles_.done())
			{
				++index_;
				delta_ += repeat_mesh_->vertex_count_;
				triangles_ = repeat_mesh_->mesh_.triangles();
			}
		}

	private:
		const repeat_mesh_t* repeat_mesh_;

		typename triangle_generator_type<mesh_t>::type triangles_;

		int index_;

		int delta_;

		explicit triangles_t(const repeat_mesh_t* repeatMesh) noexcept
			: repeat_mesh_{repeatMesh}
			, triangles_{repeatMesh->mesh_.triangles()}
			, index_{repeatMesh->vertex_count_ > 0 ? 0 : repeatMesh->instances_}
			, delta_{0}
		{
		}

		int counttriangles_t() const noexcept
		{
			if(repeat_mesh_->instances_ < 1)
				return 0;

			return count(repeat_mesh_->mesh_.triangles()) * (repeat_mesh_->instances_ - index_ - 1) +
				   count(triangles_);
		}

		friend int count(const triangles_t& generator) noexcept
		{
			return generator.counttriangles_t();
		}

		friend class repeat_mesh_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept
		{
			return index_ >= repeat_mesh_->instances_;
		}

		mesh_vertex_t generate() const
		{
			mesh_vertex_t temp = vertices_.generate();
			temp.position += delta_;
			return temp;
		}

		void next()
		{
			vertices_.next();

			if(vertices_.done())
			{
				++index_;
				delta_ += repeat_mesh_->delta_;
				vertices_ = repeat_mesh_->mesh_.vertices();
			}
		}

	private:
		explicit vertices_t(const repeat_mesh_t* repeatMesh)
			: repeat_mesh_{repeatMesh}
			, vertices_{repeatMesh->mesh_.vertices()}
			, index_{repeatMesh->vertex_count_ > 0 ? 0 : repeatMesh->instances_}
			, delta_{}
		{
		}

		const repeat_mesh_t* repeat_mesh_;

		typename vertex_generator_type<mesh_t>::type vertices_;

		int index_;

		gml::dvec3 delta_;

		int countvertices_t() const noexcept
		{
			if(repeat_mesh_->instances_ < 1)
				return 0;

			return repeat_mesh_->vertex_count_ * (repeat_mesh_->instances_ - index_ - 1) + count(vertices_);
		}

		friend int count(const vertices_t& generator) noexcept
		{
			return generator.countvertices_t();
		}

		friend class repeat_mesh_t;
	};

	/// @param mesh The mesh to repeat.
	/// @param instances Number of times to repeat. If <1 an empty mesh results.
	/// @param delta An offset aplied to each copy.
	explicit repeat_mesh_t(mesh_t mesh, int instances, const gml::dvec3& delta) noexcept
		: mesh_{std::move(mesh)}
		, instances_{instances}
		, delta_{delta}
		, vertex_count_{count(mesh_.vertices())}
	{
	}

	triangles_t triangles() const noexcept
	{
		return triangles_t{this};
	}

	vertices_t vertices() const noexcept
	{
		return vertices_t{this};
	}

private:
	mesh_t mesh_;

	int instances_;

	gml::dvec3 delta_;

	int vertex_count_;
};

template <typename mesh_t>
repeat_mesh_t<mesh_t> repeat_mesh(mesh_t mesh, int instances, const gml::dvec3& delta) noexcept
{
	return repeat_mesh_t<mesh_t>{std::move(mesh), instances, delta};
}
}

#endif
