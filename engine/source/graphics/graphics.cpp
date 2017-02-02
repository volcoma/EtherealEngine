#include "graphics.h"

namespace gfx
{
	VertexDecl PosTexCoord0Vertex::decl;
	VertexDecl MeshVertex::decl;

	void screen_quad(float dest_width, float dest_height, float width, float height)
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

			const float zz = 0.0f;

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
	}

}