#include "graphics.h"

namespace gfx
{
	VertexDecl PosTexCoord0Vertex::decl;
	VertexDecl MeshVertex::decl;

	std::uint64_t screen_quad(float dest_width, float dest_height, float depth, float width, float height)
	{
		float texture_half = get_half_texel();
		bool origin_bottom_left = is_origin_bottom_left();

		if (3 == getAvailTransientVertexBuffer(3, PosTexCoord0Vertex::decl))
		{
			TransientVertexBuffer vb;
			allocTransientVertexBuffer(&vb, 3, PosTexCoord0Vertex::decl);
			PosTexCoord0Vertex* vertex = (PosTexCoord0Vertex*)vb.data;

			const float minx = -width;
			const float maxx = width;
			const float miny = 0.0f;
			const float maxy = height*2.0f;

			const float texel_half_w = texture_half / dest_width;
			const float texel_half_h = texture_half / dest_height;
			const float minu = -1.0f + texel_half_w;
			const float maxu = 1.0f + texel_half_h;

			const float zz = depth;

			float minv = texel_half_h;
			float maxv = 2.0f + texel_half_h;

			if (origin_bottom_left)
			{
				float temp = minv;
				minv = maxv;
				maxv = temp;

				minv -= 1.0f;
				maxv -= 1.0f;
			}

			vertex[0].x = minx;
			vertex[0].y = miny;
			vertex[0].z = zz;
			vertex[0].u = minu;
			vertex[0].v = minv;

			vertex[1].x = maxx;
			vertex[1].y = miny;
			vertex[1].z = zz;
			vertex[1].u = maxu;
			vertex[1].v = minv;

			vertex[2].x = maxx;
			vertex[2].y = maxy;
			vertex[2].z = zz;
			vertex[2].u = maxu;
			vertex[2].v = maxv;

			setVertexBuffer(&vb);
		}

		return 0;
	}

	std::uint64_t clip_quad(float depth, float width, float height)
	{
		float texture_half = get_half_texel();
		bool origin_bottom_left = is_origin_bottom_left();

		if (4 == getAvailTransientVertexBuffer(4, PosTexCoord0Vertex::decl))
		{
			TransientVertexBuffer vb;
			allocTransientVertexBuffer(&vb, 4, PosTexCoord0Vertex::decl);
			PosTexCoord0Vertex* vertex = (PosTexCoord0Vertex*)vb.data;

			const float minx = -width;
			const float maxx = width;
			const float miny = -height;
			const float maxy = height*2.0f;

			const float texel_half_w = texture_half;
			const float texel_half_h = texture_half;
			const float minu = -1.0f + texel_half_w;
			const float maxu = 1.0f + texel_half_h;

			const float zz = depth;

			float minv = texel_half_h;
			float maxv = 2.0f + texel_half_h;

			if (origin_bottom_left)
			{
				float temp = minv;
				minv = maxv;
				maxv = temp;

				minv -= 1.0f;
				maxv -= 1.0f;
			}

			vertex[0].x = minx;
			vertex[0].y = maxy;
			vertex[0].z = zz;
			vertex[0].u = minu;
			vertex[0].v = maxv;

			vertex[1].x = maxx;
			vertex[1].y = maxy;
			vertex[1].z = zz;
			vertex[1].u = maxu;
			vertex[1].v = maxv;

			vertex[2].x = minx;
			vertex[2].y = miny;
			vertex[2].z = zz;
			vertex[2].u = minu;
			vertex[2].v = minv;

			vertex[3].x = maxx;
			vertex[3].y = miny;
			vertex[3].z = zz;
			vertex[3].u = maxu;
			vertex[3].v = minv;

			setVertexBuffer(&vb);
		}

		return BGFX_STATE_PT_TRISTRIP;
	}

}