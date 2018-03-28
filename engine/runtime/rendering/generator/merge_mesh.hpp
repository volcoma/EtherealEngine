#ifndef GENERATOR_MERGEMESH_HPP
#define GENERATOR_MERGEMESH_HPP

#include "empty_mesh.hpp"
#include "mesh_vertex.hpp"
#include "utils.hpp"

namespace generator
{

/// Merges (concatenates) multiple meshes to to together.
template <typename... Mesh>
class merge_mesh_t; // undefined

template <>
class merge_mesh_t<> : public empty_mesh
{
};

template <typename Head, typename... Tail>
class merge_mesh_t<Head, Tail...>
{
public:
	class triangles_t
	{
	public:
		triangle_t generate() const
		{
			if(!head_.done())
				return head_.generate();

			triangle_t triangle = tail_.generate();
			triangle.vertices += head_vertex_count_;
			return triangle;
		}

		bool done() const noexcept
		{
			return all_done_;
		}

		void next()
		{
			if(!head_.done())
				head_.next();
			else
				tail_.next();

			all_done_ = tail_.done() && head_.done();
		}

	private:
		typename triangle_generator_type<Head>::type head_;
		typename triangle_generator_type<merge_mesh_t<Tail...>>::type tail_;

		int head_vertex_count_;

		bool all_done_;

		explicit triangles_t(const merge_mesh_t& mesh)
			: head_{mesh.head_.triangles()}
			, tail_(mesh.tail_.triangles())
			, head_vertex_count_{count(mesh.head_.vertices())}
			, all_done_{tail_.done() && head_.done()}
		{
		}

		friend class merge_mesh_t<Head, Tail...>;
	};

	class vertices_t
	{
	public:
		mesh_vertex_t generate() const
		{
			if(!head_.done())
				return head_.generate();
			return tail_.generate();
		}

		bool done() const noexcept
		{
			return all_done_;
		}

		void next()
		{
			if(!head_.done())
				head_.next();
			else
				tail_.next();

			all_done_ = tail_.done() && head_.done();
		}

	private:
		typename vertex_generator_type<Head>::type head_;
		typename vertex_generator_type<merge_mesh_t<Tail...>>::type tail_;

		bool all_done_;

		explicit vertices_t(const merge_mesh_t& mesh)
			: head_{mesh.head_.vertices()}
			, tail_(mesh.tail_.vertices())
			, all_done_{tail_.done() && head_.done()}
		{
		}

		friend class merge_mesh_t<Head, Tail...>;
	};

	merge_mesh_t(Head head, Tail... tail)
		: head_{std::move(head)}
		, tail_{std::move(tail)...}
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
	Head head_;
	merge_mesh_t<Tail...> tail_;
};

template <typename... Mesh>
merge_mesh_t<Mesh...> merge_mesh(Mesh... meshes)
{
	return merge_mesh_t<Mesh...>{std::move(meshes)...};
}
}

#endif
