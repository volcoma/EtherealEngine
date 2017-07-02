#pragma once
#include <vector>
#include "core/math/math_includes.h"
#include "core/common/assert.hpp"

struct triangle_mesh_tools
{
	// all of these produce shapes with center in (0, 0, 0) and each vertex magnitude of 1 (normalized), except where specified otherwise
	// front faces are counter-clockwise
	static void create_plane(std::vector<math::vec3>& out_vertices, std::vector<std::uint32_t>& out_indices, float size_x, float size_y, bool y_up = true);
	static void create_tetrahedron(std::vector<math::vec3>& out_vertices, std::vector<std::uint32_t>& out_indices, bool share_vertices, bool y_up = true);
	static void create_cube(std::vector<math::vec3>& out_vertices, std::vector<std::uint32_t>& out_indices, bool share_vertices, float edge_half_length = 0.7071067811865475f, bool y_up = true);
	static void create_octahedron(std::vector<math::vec3>& out_vertices, std::vector<std::uint32_t>& out_indices, bool share_vertices, bool y_up = true);
	static void create_icosahedron(std::vector<math::vec3>& out_vertices, std::vector<std::uint32_t>& out_indices, bool share_vertices, bool y_up = true);
	static void create_dodecahedron(std::vector<math::vec3>& out_vertices, std::vector<std::uint32_t>& out_indices, bool share_vertices, bool y_up = true);
	static void create_icosphere(std::vector<math::vec3>& out_vertices, std::vector<std::uint32_t>& out_indices, int tessellation_level, bool share_vertices, bool y_up = true);
	static void create_cylinder(std::vector<math::vec3>& out_vertices, std::vector<std::uint32_t>& out_indices, float height, float radius_bottom, float radius_top, int tessellation, bool open_top_bottom, bool share_vertices, bool y_up = true);
	static void create_teapot(std::vector<math::vec3>& out_vertices, std::vector<std::uint32_t>& out_indices, bool y_up = true);

	template< typename VertexType >
	static inline int add_vertex(std::vector<VertexType>& out_vertices, const VertexType& vert)
	{
		out_vertices.push_back(vert);
		return (int)out_vertices.size() - 1;
	}

	template< typename VertexType >
	static inline int find_or_add(std::vector<VertexType>& vertices, const VertexType& vert, int searchForDuplicates)
	{
		for (int i = (int)(vertices.size()) - 1; (i >= 0) && (i >= (int)vertices.size() - searchForDuplicates); i--)
		{
			if (vertices[i] == vert)
				return i;
		}

		vertices.push_back(vert);
		return (int)(vertices.size()) - 1;
	}

	static inline void add_triangle(std::vector<std::uint32_t>& out_indices, int a, int b, int c)
	{
		expects((a >= 0) && (b >= 0) && (c >= 0));
		out_indices.push_back(a);
		out_indices.push_back(b);
		out_indices.push_back(c);
	}

	template< typename VertexType >
	static inline void add_triangle(std::vector<VertexType>& out_vertices, std::vector<std::uint32_t>& out_indices, const VertexType& v0, const VertexType& v1, const VertexType& v2)
	{
		int i0 = add_vertex(out_vertices, v0);
		int i1 = add_vertex(out_vertices, v1);
		int i2 = add_vertex(out_vertices, v2);

		add_triangle(out_indices, i0, i1, i2);
	}

	template< typename VertexType >
	static inline void add_triangle_merge_same_position_vertices(std::vector<VertexType>& out_vertices, std::vector<std::uint32_t>& out_indices, const VertexType& v0, const VertexType& v1, const VertexType& v2, int vertex_merging_look_from_vertex_offset = 0)
	{
		int i0 = find_or_add(out_vertices, v0, (int)out_vertices.size() - vertex_merging_look_from_vertex_offset);
		int i1 = find_or_add(out_vertices, v1, (int)out_vertices.size() - vertex_merging_look_from_vertex_offset);
		int i2 = find_or_add(out_vertices, v2, (int)out_vertices.size() - vertex_merging_look_from_vertex_offset);

		add_triangle(out_indices, i0, i1, i2);
	}

	// This adds quad triangles in strip order ( (0, 0), (1, 0), (0, 1), (1, 1) ) - so swap the last two if doing clockwise/counterclockwise
	// (this is a bit inconsistent with add_pentagon below)
	static inline void add_quad(std::vector<std::uint32_t>& out_indices, int i0, int i1, int i2, int i3)
	{
		out_indices.push_back(i0);
		out_indices.push_back(i1);
		out_indices.push_back(i2);
		out_indices.push_back(i1);
		out_indices.push_back(i3);
		out_indices.push_back(i2);
	}

	// This adds quad triangles in strip order ( (0, 0), (1, 0), (0, 1), (1, 1) ) - so swap the last two if doing clockwise/counterclockwise
	// (this is a bit inconsistent with add_pentagon below)
	template< typename VertexType >
	static inline void add_quad(std::vector<VertexType>& out_vertices, std::vector<std::uint32_t>& out_indices, const VertexType& v0, const VertexType& v1, const VertexType& v2, const VertexType& v3)
	{
		int i0 = add_vertex(out_vertices, v0);
		int i1 = add_vertex(out_vertices, v1);
		int i2 = add_vertex(out_vertices, v2);
		int i3 = add_vertex(out_vertices, v3);

		add_quad(out_indices, i0, i1, i2, i3);
	}

	// This adds triangles in clockwise, fan-like order
	// (this is a bit inconsistent with add_quad above)
	static inline void add_pentagon(std::vector<std::uint32_t>& out_indices, int i0, int i1, int i2, int i3, int i4)
	{
		add_triangle(out_indices, i0, i1, i2);
		add_triangle(out_indices, i0, i2, i3);
		add_triangle(out_indices, i0, i3, i4);
	}

	// This adds triangles in clockwise, fan-like order
	// (this is a bit inconsistent with add_quad above)
	template< typename VertexType >
	static inline void add_pentagon(std::vector<VertexType>& out_vertices, std::vector<std::uint32_t>& out_indices, const VertexType& v0, const VertexType& v1, const VertexType& v2, const VertexType& v3, const VertexType& v4)
	{
		int i0 = add_vertex(out_vertices, v0);
		int i1 = add_vertex(out_vertices, v1);
		int i2 = add_vertex(out_vertices, v2);
		int i3 = add_vertex(out_vertices, v3);
		int i4 = add_vertex(out_vertices, v4);

		add_triangle(out_indices, i0, i1, i2);
		add_triangle(out_indices, i0, i2, i3);
		add_triangle(out_indices, i0, i3, i4);
	}

	static inline void generate_normals(std::vector<math::vec4>& out_normals, const std::vector<math::vec3>& vertices, const std::vector<std::uint32_t>& indices, bool counter_clockwise, int index_from = 0, int index_count = -1, bool fix_broken_normals = true)
	{
		expects(out_normals.size() == vertices.size());
		if (index_count == -1)
			index_count = (int)indices.size();

		for (int i = 0; i < (int)vertices.size(); i++)
			out_normals[i] = math::vec4(0.0f, 0.0f, 0.0, 0.0f);

		for (int i = index_from; i < index_count; i += 3)
		{
			const math::vec3 & a = vertices[indices[i + 0]];
			const math::vec3 & b = vertices[indices[i + 1]];
			const math::vec3 & c = vertices[indices[i + 2]];

			math::vec3 norm;
			if (counter_clockwise)
				norm = math::cross(c - a, b - a);
			else
				norm = math::cross(b - a, c - a);

			float tri_area_x2 = math::length(norm);
			if (tri_area_x2 < math::epsilon<float>())
			{
				if (!fix_broken_normals)
					continue;

				if (tri_area_x2 != 0.0f)
					norm /= tri_area_x2 * 10000.0f;
			}

			// don't normalize, leave it weighted by area
			out_normals[indices[i + 0]] += math::vec4(norm.x, norm.y, norm.z, 0.0f);
			out_normals[indices[i + 1]] += math::vec4(norm.x, norm.y, norm.z, 0.0f);
			out_normals[indices[i + 2]] += math::vec4(norm.x, norm.y, norm.z, 0.0f);
		}

		for (int i = 0; i < (int)vertices.size(); i++)
		{
			float length = math::length(out_normals[i]);

			if (length < math::epsilon<float>())
				out_normals[i] = math::vec4(0.0f, 0.0f, (fix_broken_normals) ? (1.0f) : (0.0f), 0.0f);
			else
				out_normals[i] *= 1.0f / length;
		}
	}

	// based on http://www.terathon.com/code/tangent.html
	static void generate_tangents(std::vector<math::vec4>& out_tangents, std::vector<math::vec4>& out_bitangents, const std::vector<math::vec3>& vertices, const std::vector<math::vec4>& normals, const std::vector<math::vec2>& texturecoords, const std::vector<std::uint32_t>& indices)
	{
		expects(out_tangents.size() == vertices.size());

		std::vector<math::vec3> tempTans;
		tempTans.resize(vertices.size() * 2, math::vec3(0.0f, 0.0f, 0.0f));

		math::vec3 * tan1 = &tempTans[0];
		math::vec3 * tan2 = &tempTans[vertices.size()];

		expects((indices.size() % 3) == 0);
		int triangleCount = (int)indices.size() / 3;

		for (long a = 0; a < triangleCount; a++)
		{
			long i1 = indices[a * 3 + 0];
			long i2 = indices[a * 3 + 1];
			long i3 = indices[a * 3 + 2];

			const math::vec3 & v1 = vertices[i1];
			const math::vec3 & v2 = vertices[i2];
			const math::vec3 & v3 = vertices[i3];

			const math::vec2 & w1 = texturecoords[i1];
			const math::vec2 & w2 = texturecoords[i2];
			const math::vec2 & w3 = texturecoords[i3];

			float x1 = v2.x - v1.x;
			float x2 = v3.x - v1.x;
			float y1 = v2.y - v1.y;
			float y2 = v3.y - v1.y;
			float z1 = v2.z - v1.z;
			float z2 = v3.z - v1.z;

			float s1 = w2.x - w1.x;
			float s2 = w3.x - w1.x;
			float t1 = w2.y - w1.y;
			float t2 = w3.y - w1.y;

			float r = 1.0f / (s1 * t2 - s2 * t1 + math::epsilon<float>());
			math::vec3 sdir((t2 * x1 - t1 * x2) * r, (t2 * y1 - t1 * y2) * r, (t2 * z1 - t1 * z2) * r);
			math::vec3 tdir((s1 * x2 - s2 * x1) * r, (s1 * y2 - s2 * y1) * r, (s1 * z2 - s2 * z1) * r);

			tan1[i1] += sdir;
			tan1[i2] += sdir;
			tan1[i3] += sdir;

			tan2[i1] += tdir;
			tan2[i2] += tdir;
			tan2[i3] += tdir;
		}

		for (size_t a = 0; a < vertices.size(); a++)
		{
			const math::vec3 & n = normals[a];
			const math::vec3 & t = tan1[a];

			// Gram-Schmidt orthogonalize
			out_tangents[a] = math::vec4((t - n * math::normalize(math::dot(n, t))), 1.0f);

			// Calculate handedness
			out_tangents[a].w = (math::dot(math::cross(n, t), tan2[a]) < 0.0f) ? (-1.0f) : (1.0f);

			// Calculate the new orthogonal binormal
			math::vec3 tt = out_tangents[a];
			out_bitangents[a] = math::vec4(math::normalize(math::cross(n, tt)) * out_tangents[a].w, 1.0f);
		}
	}

	// dummy tangents, for better, http://www.terathon.com/code/tangent.html or http://developer.nvidia.com/object/NVMeshMender.html 
	static void fill_dummy_ttt(const std::vector<math::vec3> & vertices, const std::vector<math::vec4> & normals, std::vector<math::vec4> & tangents, std::vector<math::vec2> & texcoords0, std::vector<math::vec2> & texcoords1)
	{
		for (size_t i = 0; i < vertices.size(); i++)
		{
			math::vec3 bitangent = math::normalize(vertices[i] + math::vec3(0.0f, 0.0f, -5.0f));
			if (math::dot(bitangent, math::vec3(normals[i])) > 0.9f)
				bitangent = math::normalize(vertices[i] + math::vec3(-5.0f, 0.0f, 0.0f));
			tangents[i] = math::vec4(math::normalize(math::cross(bitangent, math::vec3(normals[i]))), 1.0);
			texcoords0[i] = math::vec2(vertices[i].x / 2.0f + 0.5f, vertices[i].y / 2.0f + 0.5f);
			texcoords1[i] = texcoords0[i];
		}
	}
};



