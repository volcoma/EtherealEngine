#pragma  once

#include "transform.h"
#include "plane.h"
#include "bbox.h"
#include "bbox_extruded.h"
#include "bsphere.h"
#include "frustum.h"
#include "math_types.h"

namespace math
{
	using namespace glm;
	inline float halton(std::uint32_t Index, std::uint32_t Base)
	{
		float Result = 0.0f;
		float InvBase = 1.0f / Base;
		float Fraction = InvBase;
		while (Index > 0)
		{
			Result += (Index % Base) * Fraction;
			Index /= Base;
			Fraction *= InvBase;
		}
		return Result;
	}

	inline std::uint32_t power_of_n_round_down(std::uint32_t val, std::uint32_t n)
	{
		std::uint32_t currentVal = n;
		std::uint32_t iter = 1;
		while (currentVal < val)
		{
			currentVal *= n;
			++iter;
		}

		return iter;
	}

	struct color
	{
		vec4 Value;

		color() { Value.x = Value.y = Value.z = Value.w = 0.0f; }
		color(int r, int g, int b, int a = 255) { float sc = 1.0f / 255.0f; Value.x = (float)r * sc; Value.y = (float)g * sc; Value.z = (float)b * sc; Value.w = (float)a * sc; }
		color(std::uint32_t rgba) { float sc = 1.0f / 255.0f; Value.x = (float)(rgba & 0xFF) * sc; Value.y = (float)((rgba >> 8) & 0xFF) * sc; Value.z = (float)((rgba >> 16) & 0xFF) * sc; Value.w = (float)(rgba >> 24) * sc; }
		color(float r, float g, float b, float a = 1.0f) { Value.x = r; Value.y = g; Value.z = b; Value.w = a; }
		color(const vec4& col) { Value = col; }
		inline operator std::uint32_t() const { return Float4ToU32(Value); }
		inline operator vec4() const { return Value; }

		inline void SetHSV(float h, float s, float v, float a = 1.0f) { HSVtoRGB(h, s, v, Value.x, Value.y, Value.z); Value.w = a; }

		static color HSV(float h, float s, float v, float a = 1.0f) { float r, g, b; HSVtoRGB(h, s, v, r, g, b); return color(r, g, b, a); }

		static vec4 U32ToFloat4(std::uint32_t in)
		{
			float s = 1.0f / 255.0f;
			return vec4((in & 0xFF) * s, ((in >> 8) & 0xFF) * s, ((in >> 16) & 0xFF) * s, (in >> 24) * s);
		}

		static std::uint32_t Float4ToU32(const vec4& in)
		{
			std::uint32_t out;
			out =  ((std::uint32_t)(saturate<float, highp>(in.x) * 255.0f));
			out |= ((std::uint32_t)(saturate<float, highp>(in.y) * 255.0f)) << 8;
			out |= ((std::uint32_t)(saturate<float, highp>(in.z) * 255.0f)) << 16;
			out |= ((std::uint32_t)(saturate<float, highp>(in.w) * 255.0f)) << 24;
			return out;
		}

		// Convert rgb floats ([0-1],[0-1],[0-1]) to hsv floats ([0-1],[0-1],[0-1]), from Foley & van Dam p592
		// Optimized http://lolengine.net/blog/2013/01/13/fast-rgb-to-hsv
		static void RGBtoHSV(float r, float g, float b, float& out_h, float& out_s, float& out_v)
		{
			float K = 0.f;
			if (g < b)
			{
				const float tmp = g; g = b; b = tmp;
				K = -1.f;
			}
			if (r < g)
			{
				const float tmp = r; r = g; g = tmp;
				K = -2.f / 6.f - K;
			}

			const float chroma = r - (g < b ? g : b);
			out_h = glm::abs(K + (g - b) / (6.f * chroma + 1e-20f));
			out_s = chroma / (r + 1e-20f);
			out_v = r;
		}

		// Convert hsv floats ([0-1],[0-1],[0-1]) to rgb floats ([0-1],[0-1],[0-1]), from Foley & van Dam p593
		// also http://en.wikipedia.org/wiki/HSL_and_HSV
		static void HSVtoRGB(float h, float s, float v, float& out_r, float& out_g, float& out_b)
		{
			if (s == 0.0f)
			{
				// gray
				out_r = out_g = out_b = v;
				return;
			}

			h = glm::mod(h, 1.0f) / (60.0f / 360.0f);
			int   i = (int)h;
			float f = h - (float)i;
			float p = v * (1.0f - s);
			float q = v * (1.0f - s * f);
			float t = v * (1.0f - s * (1.0f - f));

			switch (i)
			{
			case 0: out_r = v; out_g = t; out_b = p; break;
			case 1: out_r = q; out_g = v; out_b = p; break;
			case 2: out_r = p; out_g = v; out_b = t; break;
			case 3: out_r = p; out_g = q; out_b = v; break;
			case 4: out_r = t; out_g = p; out_b = v; break;
			case 5: default: out_r = v; out_g = p; out_b = q; break;
			}
		}

	};
}