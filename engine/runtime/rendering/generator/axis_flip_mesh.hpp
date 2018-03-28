#ifndef GENERATOR_AXISFLIPMESH_HPP
#define GENERATOR_AXISFLIPMESH_HPP

#include "transform_mesh.hpp"
#include "triangle.hpp"

namespace generator
{

/// Flips (mirrors) the mesh along one or more axis.
/// Texture coordinates are not flipped.
/// Also reverses triangle vertex order if needed.
template <typename mesh_t>
class axis_flip_mesh_t
{
private:
	using impl_t = transform_mesh_t<mesh_t>;
	impl_t transform_mesh_;

public:
	class triangles_t
	{
	public:
		triangle_t generate() const
		{
			triangle_t triangle = triangles_.generate();
			if(mesh_->flip_)
				std::swap(triangle.vertices[0], triangle.vertices[2]);
			return triangle;
		}

		bool done() const noexcept
		{
			return triangles_.done();
		}

		void next()
		{
			triangles_.next();
		}

	private:
		const axis_flip_mesh_t* mesh_;

		typename triangle_generator_type<transform_mesh_t<mesh_t>>::type triangles_;

		explicit triangles_t(const axis_flip_mesh_t& mesh)
			: mesh_{&mesh}
			, triangles_{mesh.transform_mesh_.triangles()}
		{
		}

		friend class axis_flip_mesh_t;
	};

	///@param mesh Source data mesh.
	///@param x Flip x
	///@param y Flip y
	///@param z Flip z
	axis_flip_mesh_t(mesh_t mesh, bool x, bool y, bool z)
		: transform_mesh_{std::move(mesh),
						  [x, y, z](mesh_vertex_t& vertex) {
							  if(x)
							  {
								  vertex.position[0] *= -1.0;
								  vertex.normal[0] *= -1.0;
							  }

							  if(y)
							  {
								  vertex.position[1] *= -1.0;
								  vertex.normal[1] *= -1.0;
							  }

							  if(z)
							  {
								  vertex.position[2] *= -1.0;
								  vertex.normal[2] *= -1.0;
							  }
						  }}
		, flip_{false}
	{
		if(x)
			flip_ = !flip_;
		if(y)
			flip_ = !flip_;
		if(z)
			flip_ = !flip_;
	}

	triangles_t triangles() const noexcept
	{
		return triangles_t{*this};
	}

	using vertices_t = typename impl_t::vertices_t;

	vertices_t vertices() const noexcept
	{
		return transform_mesh_.vertices();
	}

private:
	bool flip_ = false;
};

template <typename mesh_t>
axis_flip_mesh_t<mesh_t> axis_flip_mesh(mesh_t mesh, bool x, bool y, bool z)
{
	return axis_flip_mesh_t<mesh_t>{std::move(mesh), x, y, z};
}
}

#endif
