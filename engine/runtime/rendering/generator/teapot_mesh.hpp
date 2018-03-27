#ifndef GENERATOR_TEAPOTMESH_HPP
#define GENERATOR_TEAPOTMESH_HPP

#include <array>
#include <memory>

#include "bezier_mesh.hpp"
#include "mesh_vertex.hpp"
#include "triangle.hpp"
#include "utils.hpp"

namespace generator
{

/// The Utah Teapot.
/// https://en.wikipedia.org/wiki/Utah_teapot
/// @image html TeapotMesh.svg
class teapot_mesh_t
{
public:
	class triangles_t
	{
	public:
		bool done() const noexcept;
		triangle_t generate() const;
		void next();

	private:
		const teapot_mesh_t* mesh_t;

		int index_;

		std::shared_ptr<const bezier_mesh_t<4, 4>> patch_mesh_;

		typename triangle_generator_type<bezier_mesh_t<4, 4>>::type triangles_;

		explicit triangles_t(const teapot_mesh_t& mesh) noexcept;

		friend class teapot_mesh_t;
	};

	class vertices_t
	{
	public:
		bool done() const noexcept;
		mesh_vertex_t generate() const;
		void next();

	private:
		const teapot_mesh_t* mesh_;

		int index_;

		// Needs be a shared_ptr in order to make copy/move not to mess up the
		// internal pointer in triangles_.
		std::shared_ptr<const bezier_mesh_t<4, 4>> patch_mesh_;

		typename vertex_generator_type<bezier_mesh_t<4, 4>>::type vertices_;

		explicit vertices_t(const teapot_mesh_t& mesh) noexcept;

		friend class teapot_mesh_t;
	};

	/// Generates the Utah teapot using the original data.
	/// The lid is pointing towards the z axis and the spout towards the x axis.
	/// @param segments The number segments along each patch. Should be >= 1.
	/// If zero empty mesh is generated.
	explicit teapot_mesh_t(int segments = 8) noexcept;

	triangles_t triangles() const noexcept;

	vertices_t vertices() const noexcept;

private:
	int segments_;

	int patch_vertex_count_;
};
}

#endif
