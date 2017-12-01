#include "vertex_decl.h"

namespace gfx
{
void pos_texcoord0_vertex::init(gfx::vertex_layout& decl)
{
	decl.begin()
		.add(attribute::Position, 3, attribute_type::Float)
		.add(attribute::TexCoord0, 2, attribute_type::Float)
		.end();
}

void mesh_vertex::init(vertex_layout &decl)
{
    decl.begin()
            .add(attribute::Position, 3, attribute_type::Float)
            .add(attribute::Color1, 4, attribute_type::Uint8, true)
            .add(attribute::Normal, 4, attribute_type::Uint8, true, true)
            .add(attribute::Tangent, 4, attribute_type::Uint8, true, true)
            .add(attribute::Bitangent, 4, attribute_type::Uint8, true, true)
            .add(attribute::TexCoord0, 2, attribute_type::Float)
            .end();
}

void pos_texcoord0_color0_vertex::init(vertex_layout &decl)
{
    decl.begin()
        .add(attribute::Position, 2, attribute_type::Float)
        .add(attribute::TexCoord0, 2, attribute_type::Float)
        .add(attribute::Color0, 4, attribute_type::Uint8, true)
        .end();
}

}
