#include "box_mesh.hpp"

using namespace generator;

using namespace generator::detail;

box_face_t::box_face_t(const gml::dvec2& size, const gml::ivec2& segments, double delta)
	: translate_mesh_{{size, segments}, {0.0, 0.0, delta}}
{
}

box_faces_t::box_faces_t(const gml::dvec2& size, const gml::ivec2& segments, double delta)
	: merge_mesh_{{size, segments, delta}, {{{size, segments, -delta}}, true, false}}
{
}

box_mesh_t::box_mesh_t(const gml::dvec3& size, const gml::ivec3& segments) noexcept
	: merge_mesh_{
		  {{{size[1], size[2]}, {segments[1], segments[2]}, size[0]}, axis_t::Z, axis_t::X, axis_t::Y},
		  {{{{size[0], size[2]}, {segments[0], segments[2]}, size[1]}, axis_t::X, axis_t::Z, axis_t::Y},
		   true,
		   false},
		  {{size[0], size[1]}, {segments[0], segments[1]}, size[2]}}
{
}
