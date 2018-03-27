#ifndef GENERATOR_ANYMESH_HPP
#define GENERATOR_ANYMESH_HPP

#include <memory>

#include "any_generator.hpp"
#include "mesh_vertex.hpp"
#include "triangle.hpp"

namespace generator
{

/// A type erasing container that can store any mesh.
class any_mesh
{
public:
	template <typename mesh_t>
	any_mesh(mesh_t mesh)
		: base_{new derived<mesh_t>{std::move(mesh)}}
	{
	}

	any_mesh(const any_mesh& that);

	any_mesh(any_mesh&&) = default;

	any_mesh& operator=(const any_mesh& that);

	any_mesh& operator=(any_mesh&&) = default;

	any_generator<triangle_t> triangles() const noexcept;

	any_generator<mesh_vertex_t> vertices() const noexcept;

private:
	class base
	{
	public:
		virtual ~base();
		virtual std::unique_ptr<base> clone() const = 0;
		virtual any_generator<triangle_t> triangles() const = 0;
		virtual any_generator<mesh_vertex_t> vertices() const = 0;
	};

	template <typename mesh_t>
	class derived : public base
	{
	public:
		derived(mesh_t mesh)
			: mesh_(std::move(mesh))
		{
		}

		std::unique_ptr<base> clone() const override
		{
			return std::unique_ptr<base>{new derived{mesh_}};
		}

		any_generator<triangle_t> triangles() const override
		{
			return mesh_.triangles();
		}

		any_generator<mesh_vertex_t> vertices() const override
		{
			return mesh_.vertices();
		}

		mesh_t mesh_;
	};

	std::unique_ptr<base> base_;
};
}

#endif
