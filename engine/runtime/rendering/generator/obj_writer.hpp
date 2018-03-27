#ifndef GENERATOR_OBJWRITER_HPP
#define GENERATOR_OBJWRITER_HPP

#include <sstream>

#include "mesh_vertex.hpp"
#include "triangle.hpp"

namespace generator
{

/// A class for generating obj files for preview and debug purposes.
class obj_writer_t
{
public:
	obj_writer_t();

	template <typename Mesh>
	void write_mesh(const Mesh& mesh)
	{
		int newBase = base_;

		for(const mesh_vertex_t& vertex : mesh.vertices())
		{
			++newBase;

			ss_ << "v " << vertex.position[0] << " " << vertex.position[1] << " " << vertex.position[2]
				<< "\n";

			ss_ << "vn " << vertex.normal[0] << " " << vertex.normal[1] << " " << vertex.normal[2] << "\n";

			ss_ << "vt " << vertex.tex_coord[0] << " " << vertex.tex_coord[1] << "\n";
		}

		for(const triangle_t& triangle : mesh.triangles())
		{
			auto t = triangle.vertices + base_;
			ss_ << "f " << t[0] << "/" << t[0] << "/" << t[0] << " " << t[1] << "/" << t[1] << "/" << t[1]
				<< " " << t[2] << "/" << t[2] << "/" << t[2] << "\n";
		}

		base_ = newBase;
	}

	std::string str()
	{
		return ss_.str();
	}

private:
	int base_;

	std::stringstream ss_;
};
}

#endif
