#ifndef GENERATOR_SUBDIVIDEMESH_HPP
#define GENERATOR_SUBDIVIDEMESH_HPP

#include <map>
#include <vector>

#include "edge.hpp"
#include "mesh_vertex.hpp"
#include "triangle.hpp"
#include "utils.hpp"

namespace generator
{

template <typename mesh_t, int Iterations>
class subdivide_mesh_t
{
	static_assert(Iterations > 0, "Iterations must be greater than zero!");

private:
	using impl_t = subdivide_mesh_t<subdivide_mesh_t<mesh_t, Iterations - 1>, 1>;
	impl_t subdivide_mesh_;

public:
	subdivide_mesh_t(mesh_t mesh)
		: subdivide_mesh_{subdivide_mesh_t<mesh_t, Iterations - 1>{std::move(mesh)}}
	{
	}

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return subdivide_mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return subdivide_mesh_.vertices();
	}
};

template <typename mesh_t>
class subdivide_mesh_t<mesh_t, 0>
{
private:
	using impl_t = mesh_t;
	impl_t mesh_;

public:
	subdivide_mesh_t(mesh_t mesh)
		: mesh_{std::move(mesh)}
	{
	}

	using triangles_t = typename impl_t::triangles_t;

	triangles_t triangles() const noexcept
	{
		return mesh_.triangles();
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return mesh_.vertices();
	}
};

/// Subdivides each triangle to 4 parts by splitting edges.
template <typename mesh_t>
class subdivide_mesh_t<mesh_t, 1>
{
public:
	class triangles_t
	{
	public:
		bool done() const noexcept
		{
			return triangles_.done();
		}

		triangle_t generate() const
		{
			if(i_ == 0)
				triangle_ = triangles_.generate();

			if(i_ == 3)
			{
				return triangle_t{{vertexFromEdge(triangle_.vertices[0], triangle_.vertices[1]),
								   vertexFromEdge(triangle_.vertices[1], triangle_.vertices[2]),
								   vertexFromEdge(triangle_.vertices[2], triangle_.vertices[0])}};
			}

			int j = (i_ + 1) % 3;
			int k = (i_ + 2) % 3;
			return triangle_t{{triangle_.vertices[i_],
							   vertexFromEdge(triangle_.vertices[i_], triangle_.vertices[j]),
							   vertexFromEdge(triangle_.vertices[k], triangle_.vertices[i_])}};
		}

		void next()
		{
			++i_;
			if(i_ == 4)
			{
				i_ = 0;
				triangles_.next();
			}
		}

	private:
		const subdivide_mesh_t* mesh_;

		int i_;

		typename triangle_generator_type<mesh_t>::type triangles_;

		mutable triangle_t triangle_;

		explicit triangles_t(const subdivide_mesh_t& mesh)
			: mesh_{&mesh}
			, i_{0}
			, triangles_{mesh.mesh_.triangles()}
			, triangle_{}
		{
		}

		int vertexFromEdge(int a, int b) const
		{
			if(a > b)
				std::swap(a, b);
			return static_cast<int>(mesh_->vertex_cache_.size()) + mesh_->edge_map_.at({a, b});
		}

		friend class subdivide_mesh_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept
		{
			return vertex_index_ == mesh_->vertex_cache_.size() && edge_index_ == mesh_->edge_cache_.size();
		}

		mesh_vertex_t generate() const
		{
			if(vertex_index_ < mesh_->vertex_cache_.size())
				return mesh_->vertex_cache_[vertex_index_];

			const mesh_vertex_t& v1 = mesh_->vertex_cache_[mesh_->edge_cache_[edge_index_].vertices[0]];
			const mesh_vertex_t& v2 = mesh_->vertex_cache_[mesh_->edge_cache_[edge_index_].vertices[1]];

			mesh_vertex_t vertex;
			vertex.position = gml::mix(v1.position, v2.position, 0.5);
			vertex.tex_coord = gml::mix(v1.tex_coord, v2.tex_coord, 0.5);
			vertex.normal = gml::normalize(gml::mix(v1.normal, v2.normal, 0.5));
			return vertex;
		}

		void next()
		{
			if(vertex_index_ < mesh_->vertex_cache_.size())
				++vertex_index_;
			else
				++edge_index_;
		}

	private:
		const subdivide_mesh_t* mesh_;

		int edge_index_;
		int vertex_index_;

		explicit vertices_t(const subdivide_mesh_t& mesh)
			: mesh_{&mesh}
			, edge_index_{0}
			, vertex_index_{0}
		{
		}

		friend class subdivide_mesh_t;
	};

	subdivide_mesh_t(mesh_t mesh)
		: mesh_{std::move(mesh)}
	{
		for(const mesh_vertex_t& vertex : mesh_.vertices())
		{
			vertex_cache_.push_back(vertex);
		}

		for(const triangle_t& triangle : mesh_.triangles())
		{
			for(int i = 0; i < 3; ++i)
			{
				int j = (i + 1) % 3;

				edge_t e{{triangle.vertices[i], triangle.vertices[j]}};
				if(e.vertices[0] > e.vertices[1])
					std::swap(e.vertices[0], e.vertices[1]);

				if(edge_map_.find(e.vertices) == edge_map_.end())
				{
					edge_map_[e.vertices] = static_cast<int>(edge_cache_.size());
					edge_cache_.push_back(e);
				}
			}
		}
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
	mesh_t mesh_;

	std::vector<edge_t> edge_cache_;

	std::map<gml::ivec2, int> edge_map_;

	std::vector<mesh_vertex_t> vertex_cache_;
};
}

#endif
