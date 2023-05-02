#pragma once

#include "bgfx/bgfx.h"

namespace gfx
{

using vertex_layout = bgfx::VertexLayout;
using attribute = bgfx::Attrib::Enum;
using attribute_type = bgfx::AttribType::Enum;

template<typename T>
struct vertex
{
    static const vertex_layout& get_layout()
    {
        static vertex_layout s_decl = []()
        {
            vertex_layout decl;
            T::init(decl);
            return decl;
        }();
        return s_decl;
    }
};

struct pos_texcoord0_vertex : vertex<pos_texcoord0_vertex>
{
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;

    float u = 0.0f;
    float v = 0.0f;

    static void init(vertex_layout& decl);
};

struct mesh_vertex : vertex<mesh_vertex>
{
    static void init(vertex_layout& decl);
};

struct pos_texcoord0_color0_vertex : vertex<pos_texcoord0_color0_vertex>
{
    static void init(vertex_layout& decl);
};
} // namespace gfx
