#include "graphics.h"

namespace gfx
{
	bgfx::VertexDecl PosTexCoord0Vertex::decl;

	void stretchRect(float _destWidth, float _destHeight, float _width /*= 1.0f*/, float _height /*= 1.0f*/)
	{
		const bgfx::RendererType::Enum renderer = bgfx::getRendererType();
		float _texelHalf = bgfx::RendererType::Direct3D9 == renderer ? 0.5f : 0.0f;
		bool _originBottomLeft = bgfx::getCaps()->originBottomLeft;

		if (bgfx::getAvailTransientVertexBuffer(3, PosTexCoord0Vertex::decl) == 3)
		{
			bgfx::TransientVertexBuffer vb;
			bgfx::allocTransientVertexBuffer(&vb, 3, PosTexCoord0Vertex::decl);
			PosTexCoord0Vertex* vertex = (PosTexCoord0Vertex*)vb.data;

			const float minx = -_width;
			const float maxx = _width;
			const float miny = 0.0f;
			const float maxy = _height*2.0f;

			const float texelHalfW = _texelHalf / _destWidth;
			const float texelHalfH = _texelHalf / _destHeight;
			const float minu = -1.0f + texelHalfW;
			const float maxu = 1.0f + texelHalfH;

			const float zz = 0.0f;

			float minv = texelHalfH;
			float maxv = 2.0f + texelHalfH;

			if (_originBottomLeft)
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

			bgfx::setVertexBuffer(&vb);
		}
	}

}