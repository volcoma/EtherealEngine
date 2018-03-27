#include "rounded_box_mesh.hpp"

using namespace generator;
using namespace generator::detail;

box_edge_t::box_edge_t(const gml::dvec2& position, double radius, double size, int slices, int segments)
	: translate_mesh_{{radius, size, slices, segments, 0.0, gml::radians(90.0)}, gml::dvec3{position, 0.0}}
{
}

box_edges_t::box_edges_t(const gml::dvec3& size, double radius, int slices, int segments)
	: mirrorMesh_{{{gml::dvec2{size}, radius, size[2], slices, segments}, axis_t::Y}, axis_t::X}
{
}

box_corner_t::box_corner_t(const gml::dvec3& position, double radius, int slices)
	: translate_mesh_{
		  {gml::dvec3{radius, 0.0, 0.0}, gml::dvec3{0.0, radius, 0.0}, gml::dvec3{0.0, 0.0, radius}, slices},
		  position}
{
}

box_corners_t::box_corners_t(const gml::dvec3& size, double radius, int slices)
	: mirrorMesh_{{{{size, radius, slices}, axis_t::X}, axis_t::Y}, axis_t::Z}
{
}

rounded_box_mesh_t::rounded_box_mesh_t(double radius, const gml::dvec3& size, int slices,
									   const gml::ivec3& segments)
	: merge_mesh_{
		  {{{size[1], size[2]}, {segments[1], segments[2]}, size[0] + radius},
		   axis_t::Z,
		   axis_t::X,
		   axis_t::Y},
		  {{{{size[0], size[2]}, {segments[0], segments[2]}, size[1] + radius},
			axis_t::X,
			axis_t::Z,
			axis_t::Y},
		   true,
		   false},
		  {{size[0], size[1]}, {segments[0], segments[1]}, size[2] + radius},
		  {{{size[2], size[1], size[0]}, radius, slices, segments[0]}, axis_t::Z, axis_t::Y, axis_t::X},
		  {{{size[0], size[2], size[1]}, radius, slices, segments[1]}, axis_t::X, axis_t::Z, axis_t::Y},
		  {size, radius, slices, segments[2]},
		  {size, radius, slices}}
{
}
