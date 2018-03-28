#ifndef GENERATOR_SUBDIVIDEPATH_HPP
#define GENERATOR_SUBDIVIDEPATH_HPP

#include "edge.hpp"
#include "path_vertex.hpp"
#include "shape_vertex.hpp"
#include "utils.hpp"

namespace generator
{

template <typename path_t>
class subdivide_path_t
{
public:
	class edges_t
	{
	public:
		bool done() const noexcept
		{
			return edges_.done();
		}

		edge_t generate() const
		{
			edge_t edge_ = edges_.generate();

			if(i_ % 2 == 0)
				return edge_t{{edge_.vertices[0], static_cast<int>(path_->vertex_cache_.size()) + i_ / 2}};

			return edge_t{{static_cast<int>(path_->vertex_cache_.size()) + i_ / 2, edge_.vertices[1]}};
		}

		void next()
		{
			++i_;
			if(i_ % 2 == 0)
				edges_.next();
		}

	private:
		const subdivide_path_t* path_;

		typename edge_generator_type<path_t>::type edges_;

		int i_;

		explicit edges_t(const subdivide_path_t& path)
			: path_{&path}
			, edges_{path.path_.edges()}
			, i_{0}
		{
		}

		friend class subdivide_path_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept
		{
			return vertex_index_ == path_->vertex_cache_.size() && edges_.done();
		}

		path_vertex_t generate() const
		{
			if(vertex_index_ < path_->vertex_cache_.size())
				return path_->vertex_cache_[vertex_index_];

			const edge_t edge = edges_.generate();
			const path_vertex_t& v1 = path_->vertex_cache_[edge.vertices[0]];
			const path_vertex_t& v2 = path_->vertex_cache_[edge.vertices[1]];

			path_vertex_t vertex;
			vertex.position = gml::mix(v1.position, v2.position, 0.5);
			vertex.tangent = gml::normalize(gml::mix(v1.tangent, v2.tangent, 0.5));
			vertex.normal = gml::normalize(gml::mix(v1.normal, v2.normal, 0.5));
			vertex.tex_coord = 0.5 * v1.tex_coord + 0.5 * v2.tex_coord;
			return vertex;
		}

		void next()
		{
			if(vertex_index_ < path_->vertex_cache_.size())
				++vertex_index_;
			else
				edges_.next();
		}

	private:
		const subdivide_path_t* path_;

		int vertex_index_;

		typename edge_generator_type<path_t>::type edges_;

		explicit vertices_t(const subdivide_path_t& path)
			: path_{&path}
			, vertex_index_{0}
			, edges_{path.path_.edges()}
		{
		}

		friend class subdivide_path_t;
	};

	subdivide_path_t(path_t path)
		: path_(std::move(path))
		, vertex_cache_{}
	{
		for(const path_vertex_t& vertex : path_.vertices())
		{
			vertex_cache_.push_back(vertex);
		}
	}

	edges_t edges() const
	{
        return edges_t{*this};
	}

	vertices_t vertices() const
	{
        return vertices_t{*this};
	}

private:
	path_t path_;

	std::vector<path_vertex_t> vertex_cache_;
};

template <typename path_t>
subdivide_path_t<path_t> subdivide_path(path_t path)
{
	return subdivide_path_t<path_t>{std::move(path)};
}
}

#endif
