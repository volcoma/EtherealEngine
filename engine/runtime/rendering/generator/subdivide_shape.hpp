#ifndef GENERATOR_SUBDIVIDESHAPE_HPP
#define GENERATOR_SUBDIVIDESHAPE_HPP

#include "edge.hpp"
#include "shape_vertex.hpp"
#include "utils.hpp"

namespace generator
{

/// Cuts each edge in half
template <typename shape_t>
class subdivide_shape_t
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
				return edge_t{edge_.vertices[0], static_cast<int>(shape_->vertex_cache_.size()) + i_ / 2};

			return edge_t{static_cast<int>(shape_->vertex_cache_.size()) + i_ / 2, edge_.vertices[1]};
		}

		void next()
		{
			++i_;
			if(i_ % 2 == 0)
				edges_.next();
		}

	private:
		const subdivide_shape_t* shape_;

		typename edge_generator_type<shape_t>::type edges_;

		int i_;

		explicit edges_t(const subdivide_shape_t& shape)
			: shape_{&shape}
			, edges_{shape.shape_.edges()}
			, i_{0}
		{
		}

		friend class subdivide_shape_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept
		{
			return vertex_index_ == shape_->vertex_cache_.size() && edges_.done();
		}

		shape_vertex_t generate() const
		{
			if(vertex_index_ < shape_->vertex_cache_.size())
				return shape_->vertex_cache_[vertex_index_];

			const edge_t edge = edges_.generate();
			const shape_vertex_t& v1 = shape_->vertex_cache_[edge.vertices[0]];
			const shape_vertex_t& v2 = shape_->vertex_cache_[edge.vertices[1]];

			shape_vertex_t vertex;
			vertex.position = gml::mix(v1.position, v2.position, 0.5);
			vertex.tangent = gml::normalize(gml::mix(v1.tangent, v2.tangent, 0.5));
			vertex.tex_coord = 0.5 * v1.tex_coord + 0.5 * v2.tex_coord;
			return vertex;
		}

		void next()
		{
			if(vertex_index_ < shape_->vertex_cache_.size())
				++vertex_index_;
			else
				edges_.next();
		}

	private:
		const subdivide_shape_t* shape_;

		int vertex_index_;

		typename edge_generator_type<shape_t>::type edges_;

		explicit vertices_t(const subdivide_shape_t& shape)
			: shape_{&shape}
			, vertex_index_{0}
			, edges_{shape.shape_.edges()}
		{
		}

		friend class subdivide_shape_t;
	};

	subdivide_shape_t(shape_t shape)
		: shape_(std::move(shape))
		, vertex_cache_{}
	{
		for(const shape_vertex_t& vertex : shape_.vertices())
		{
			vertex_cache_.push_back(vertex);
		}
	}

	edges_t edges() const noexcept
	{
        return edges_t{*this};
	}

	vertices_t vertices() const noexcept
	{
        return vertices_t{*this};
	}

private:
	shape_t shape_;

	std::vector<shape_vertex_t> vertex_cache_;
};
}

#endif
